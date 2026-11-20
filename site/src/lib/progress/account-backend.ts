// Progress for a signed-in learner (milestone W6): the account in the
// database is the record, and localStorage is a cache of it, so pages still
// render progress at once and keep working if the network drops.
//
// When a learner signs in, progress this browser made before (anonymously,
// or while signed out, or while the server could not be reached) is merged
// into the account. After that the account wins, so a lesson marked "not
// complete" on one device stays that way on the others.

import { diffOps, type ProgressOp } from './api.js';
import {
	EMPTY_PROGRESS,
	localStorageBackend,
	markComplete,
	markIncomplete,
	parseSnapshot,
	recordView,
	STORAGE_KEY,
	type ProgressBackend,
	type ProgressSnapshot
} from './progress.js';

export interface ProgressApi {
	get(): Promise<ProgressSnapshot>;
	apply(ops: ProgressOp[]): Promise<ProgressSnapshot>;
	merge(snapshot: ProgressSnapshot): Promise<ProgressSnapshot>;
}

/** The /api/progress endpoints (see api.ts). */
export function httpProgressApi(fetchFn: typeof fetch = fetch): ProgressApi {
	async function call(method: string, body?: unknown): Promise<ProgressSnapshot> {
		const response = await fetchFn('/api/progress', {
			method,
			headers: body === undefined ? undefined : { 'content-type': 'application/json' },
			body: body === undefined ? undefined : JSON.stringify(body)
		});
		if (!response.ok) throw new Error(`progress ${method}: ${response.status}`);
		return parseSnapshot(await response.text());
	}
	return {
		get: () => call('GET'),
		apply: (ops) => call('POST', { ops }),
		merge: (snapshot) => call('PUT', snapshot)
	};
}

/**
 * Whose progress the localStorage cache holds. Absent: anonymous progress.
 * `pending`: changes the account may not have yet.
 */
export interface SyncMeta {
	user: string;
	pending: boolean;
}

export const SYNC_KEY = 'cftgu-progress-account';

export function readMeta(storage: Storage): SyncMeta | null {
	try {
		const data = JSON.parse(storage.getItem(SYNC_KEY) ?? 'null') as unknown;
		if (typeof data === 'object' && data !== null && 'user' in data && 'pending' in data) {
			const { user, pending } = data;
			if (typeof user === 'string' && typeof pending === 'boolean') return { user, pending };
		}
	} catch {
		// Unreadable: treat as anonymous.
	}
	return null;
}

export function writeMeta(storage: Storage, meta: SyncMeta | null) {
	try {
		if (meta) storage.setItem(SYNC_KEY, JSON.stringify(meta));
		else storage.removeItem(SYNC_KEY);
	} catch {
		// Blocked storage: the next visit merges again, which is safe.
	}
}

/** Forget this browser's copy of the learner's progress (on sign-out). */
export function clearLocalProgress(storage: Storage) {
	try {
		storage.removeItem(STORAGE_KEY);
	} catch {
		// Nothing to clear.
	}
	writeMeta(storage, null);
}

/**
 * Progress in this browser before the account state is known, or while
 * signed out. If the cache belongs to an account, a change to which lessons
 * are complete marks it pending, so it is merged into that account at the
 * next sign-in instead of being replaced by the account's copy. (Views are
 * always merged, so they do not count.)
 */
export function browserBackend(storage: Storage, target: Window | null = null): ProgressBackend {
	const local = localStorageBackend(storage, target);
	return {
		...local,
		write(next) {
			const before = local.read();
			local.write(next);
			const meta = readMeta(storage);
			if (meta && !meta.pending && diffOps(before, next).some((op) => op.type !== 'view')) {
				writeMeta(storage, { ...meta, pending: true });
			}
		}
	};
}

export type InitialSync = 'merge-all' | 'merge-view' | 'replace';

/** How to reconcile this browser's cache with the account on sign-in. */
export function initialSync(meta: SyncMeta | null, userId: string): InitialSync {
	if (meta === null) return 'merge-all';
	if (meta.user !== userId) return 'replace';
	return meta.pending ? 'merge-all' : 'merge-view';
}

/** Apply operations to a snapshot, as the server does. */
export function applyOpsLocally(
	snapshot: ProgressSnapshot,
	ops: ProgressOp[],
	now: Date
): ProgressSnapshot {
	let next = snapshot;
	for (const op of ops) {
		if (op.type === 'complete') next = markComplete(next, op.slug, now);
		else if (op.type === 'incomplete') next = markIncomplete(next, op.slug);
		else next = recordView(next, op.slug, now);
	}
	return next;
}

export interface AccountBackend extends ProgressBackend {
	/** Settles when the first reconciliation with the account is done. */
	ready: Promise<void>;
}

export function accountBackend(options: {
	storage: Storage;
	target?: Window | null;
	userId: string;
	api: ProgressApi;
}): AccountBackend {
	const { storage, userId, api } = options;
	const local = localStorageBackend(storage, options.target ?? null);
	const listeners = new Set<(snapshot: ProgressSnapshot) => void>();
	let syncing = true;
	let failed = false;
	let inFlight = 0;
	const queued: ProgressOp[] = [];

	function publish(snapshot: ProgressSnapshot) {
		local.write(snapshot);
		for (const listener of listeners) listener(snapshot);
	}

	function send(ops: ProgressOp[]) {
		writeMeta(storage, { user: userId, pending: true });
		inFlight++;
		api.apply(ops).then(
			() => {
				inFlight--;
				if (inFlight === 0 && !failed) writeMeta(storage, { user: userId, pending: false });
			},
			() => {
				inFlight--;
				// Keep `pending`: the next visit merges this browser's copy in.
				failed = true;
			}
		);
	}

	const plan = initialSync(readMeta(storage), userId);
	// Another learner's cache: drop it before anything renders from it.
	if (plan === 'replace') local.write(EMPTY_PROGRESS);
	const cached = local.read();
	const first =
		plan === 'replace'
			? api.get()
			: api.merge(
					plan === 'merge-all' ? cached : { ...EMPTY_PROGRESS, lastViewed: cached.lastViewed }
				);

	const ready = first.then(
		(server) => {
			syncing = false;
			let snapshot = server;
			if (queued.length > 0) {
				const ops = queued.splice(0);
				snapshot = applyOpsLocally(server, ops, new Date());
				send(ops);
			} else {
				writeMeta(storage, { user: userId, pending: false });
			}
			publish(snapshot);
		},
		() => {
			syncing = false;
			failed = true;
			writeMeta(storage, { user: userId, pending: true });
			if (queued.length > 0) send(queued.splice(0));
		}
	);

	return {
		ready,
		read() {
			return local.read();
		},
		write(next) {
			const ops = diffOps(local.read(), next);
			local.write(next);
			if (ops.length === 0) return;
			if (syncing) queued.push(...ops);
			else send(ops);
		},
		subscribe(listener) {
			listeners.add(listener);
			const unsubscribe = local.subscribe?.(listener);
			return () => {
				listeners.delete(listener);
				unsubscribe?.();
			};
		}
	};
}
