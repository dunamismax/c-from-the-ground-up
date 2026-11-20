import { describe, expect, it } from 'vitest';
import { diffOps, type ProgressOp } from './api.js';
import {
	accountBackend,
	applyOpsLocally,
	clearLocalProgress,
	initialSync,
	browserBackend,
	readMeta,
	SYNC_KEY,
	writeMeta,
	type ProgressApi
} from './account-backend.js';
import { MemoryStorage } from './memory-storage.js';
import {
	EMPTY_PROGRESS,
	markComplete,
	recordView,
	STORAGE_KEY,
	type ProgressSnapshot
} from './progress.js';

const now = new Date('2026-11-20T12:00:00Z');
const one = '01-hello-world';
const two = '02-variables-and-data-types';

/** A fake account: applies operations like the server and records calls. */
function fakeApi(initial: ProgressSnapshot = EMPTY_PROGRESS) {
	let server = initial;
	const calls: string[] = [];
	let gate: Promise<void> = Promise.resolve();
	const api: ProgressApi = {
		async get() {
			calls.push('get');
			await gate;
			return server;
		},
		apply(ops: ProgressOp[]) {
			calls.push(`apply ${ops.map((op) => `${op.type}:${op.slug}`).join(',')}`);
			server = applyOpsLocally(server, ops, now);
			return Promise.resolve(server);
		},
		async merge(snapshot) {
			calls.push(`merge ${Object.keys(snapshot.completed).join(',')}|${snapshot.lastViewed?.slug}`);
			await gate;
			const completed = { ...snapshot.completed, ...server.completed };
			const views = [server.lastViewed, snapshot.lastViewed].filter((view) => view !== null);
			views.sort((a, b) => a.at.localeCompare(b.at));
			server = { version: 1, completed, lastViewed: views.at(-1) ?? null };
			return server;
		}
	};
	return {
		api,
		calls,
		get server() {
			return server;
		},
		hold() {
			let release = () => {};
			gate = new Promise((resolve) => (release = resolve));
			return release;
		}
	};
}

function storageWith(snapshot: ProgressSnapshot, meta?: { user: string; pending: boolean }) {
	const storage = new MemoryStorage();
	storage.setItem(STORAGE_KEY, JSON.stringify(snapshot));
	if (meta) writeMeta(storage, meta);
	return storage;
}

describe('diffOps', () => {
	it('turns snapshot changes into operations', () => {
		const done = markComplete(EMPTY_PROGRESS, one, now);
		expect(diffOps(EMPTY_PROGRESS, done)).toEqual([{ type: 'complete', slug: one }]);
		expect(diffOps(done, EMPTY_PROGRESS)).toEqual([{ type: 'incomplete', slug: one }]);
		expect(diffOps(done, recordView(done, two, now))).toEqual([{ type: 'view', slug: two }]);
		expect(diffOps(done, done)).toEqual([]);
	});
});

describe('initialSync', () => {
	it('merges anonymous or unsynced progress, and trusts the account otherwise', () => {
		expect(initialSync(null, 'u1')).toBe('merge-all');
		expect(initialSync({ user: 'u1', pending: true }, 'u1')).toBe('merge-all');
		expect(initialSync({ user: 'u1', pending: false }, 'u1')).toBe('merge-view');
		expect(initialSync({ user: 'u2', pending: true }, 'u1')).toBe('replace');
	});
});

describe('sync metadata', () => {
	it('round-trips, ignores junk and is cleared on sign-out', () => {
		const storage = storageWith(EMPTY_PROGRESS, { user: 'u1', pending: false });
		expect(readMeta(storage)).toEqual({ user: 'u1', pending: false });
		storage.setItem(SYNC_KEY, '{"user":1}');
		expect(readMeta(storage)).toBeNull();
		clearLocalProgress(storage);
		expect(storage.getItem(STORAGE_KEY)).toBeNull();
		expect(storage.getItem(SYNC_KEY)).toBeNull();
	});
});

describe('browserBackend', () => {
	it("marks an account's cache pending when completions change, not on views", () => {
		const storage = storageWith(EMPTY_PROGRESS, { user: 'u1', pending: false });
		const backend = browserBackend(storage);
		backend.write(recordView(backend.read(), one, now));
		expect(readMeta(storage)).toEqual({ user: 'u1', pending: false });
		backend.write(markComplete(backend.read(), one, now));
		expect(readMeta(storage)).toEqual({ user: 'u1', pending: true });
	});

	it('leaves anonymous progress anonymous', () => {
		const storage = new MemoryStorage();
		browserBackend(storage).write(markComplete(EMPTY_PROGRESS, one, now));
		expect(readMeta(storage)).toBeNull();
	});
});

describe('accountBackend', () => {
	it('merges anonymous progress into the account on first sign-in', async () => {
		const storage = storageWith(markComplete(EMPTY_PROGRESS, one, now));
		const fake = fakeApi(markComplete(EMPTY_PROGRESS, two, now));
		const backend = accountBackend({ storage, userId: 'u1', api: fake.api });
		await backend.ready;
		expect(fake.calls).toEqual([`merge ${one}|undefined`]);
		expect(Object.keys(backend.read().completed).sort()).toEqual([one, two]);
		expect(readMeta(storage)).toEqual({ user: 'u1', pending: false });
	});

	it('trusts the account once synced, sending only the latest view', async () => {
		const cached = recordView(markComplete(EMPTY_PROGRESS, one, now), two, now);
		const storage = storageWith(cached, { user: 'u1', pending: false });
		// Lesson 1 was marked not complete on another device.
		const fake = fakeApi(EMPTY_PROGRESS);
		const backend = accountBackend({ storage, userId: 'u1', api: fake.api });
		await backend.ready;
		expect(fake.calls).toEqual([`merge |${two}`]);
		expect(backend.read().completed).toEqual({});
		expect(backend.read().lastViewed?.slug).toBe(two);
	});

	it("drops another learner's cache", async () => {
		const storage = storageWith(markComplete(EMPTY_PROGRESS, one, now), {
			user: 'u2',
			pending: true
		});
		const fake = fakeApi();
		const backend = accountBackend({ storage, userId: 'u1', api: fake.api });
		expect(backend.read()).toEqual(EMPTY_PROGRESS);
		await backend.ready;
		expect(fake.calls).toEqual(['get']);
	});

	it('sends changes as operations and notifies subscribers of the account state', async () => {
		const storage = storageWith(EMPTY_PROGRESS, { user: 'u1', pending: false });
		const fake = fakeApi(markComplete(EMPTY_PROGRESS, two, now));
		const backend = accountBackend({ storage, userId: 'u1', api: fake.api });
		const seen: ProgressSnapshot[] = [];
		backend.subscribe?.((snapshot) => seen.push(snapshot));
		await backend.ready;
		expect(Object.keys(seen.at(-1)?.completed ?? {})).toEqual([two]);

		backend.write(markComplete(backend.read(), one, now));
		await Promise.resolve();
		expect(fake.calls.at(-1)).toBe(`apply complete:${one}`);
		expect(Object.keys(fake.server.completed).sort()).toEqual([one, two]);
		await new Promise((resolve) => setTimeout(resolve));
		expect(readMeta(storage)).toEqual({ user: 'u1', pending: false });
	});

	it('keeps changes made while the first sync is still running', async () => {
		const storage = storageWith(EMPTY_PROGRESS, { user: 'u1', pending: false });
		const fake = fakeApi(markComplete(EMPTY_PROGRESS, two, now));
		const release = fake.hold();
		const backend = accountBackend({ storage, userId: 'u1', api: fake.api });
		backend.write(markComplete(backend.read(), one, now));
		expect(fake.calls).toEqual(['merge |undefined']);
		release();
		await backend.ready;
		expect(fake.calls.at(-1)).toBe(`apply complete:${one}`);
		expect(Object.keys(backend.read().completed).sort()).toEqual([one, two]);
	});

	it('marks the cache pending when the account cannot be reached', async () => {
		const storage = storageWith(EMPTY_PROGRESS, { user: 'u1', pending: false });
		const api: ProgressApi = {
			get: () => Promise.reject(new Error('offline')),
			apply: () => Promise.reject(new Error('offline')),
			merge: () => Promise.reject(new Error('offline'))
		};
		const backend = accountBackend({ storage, userId: 'u1', api });
		await backend.ready;
		expect(readMeta(storage)).toEqual({ user: 'u1', pending: true });
		backend.write(markComplete(backend.read(), one, now));
		await new Promise((resolve) => setTimeout(resolve));
		expect(readMeta(storage)).toEqual({ user: 'u1', pending: true });
		expect(backend.read().completed[one]).toBeDefined();
	});
});
