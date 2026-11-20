import { describe, expect, it } from 'vitest';
import { MemoryStorage } from './memory-storage.js';
import {
	continueSlug,
	EMPTY_PROGRESS,
	localStorageBackend,
	markComplete,
	markIncomplete,
	parseSnapshot,
	progressFor,
	recordView,
	STORAGE_KEY
} from './progress.js';

const now = new Date('2026-11-20T12:00:00Z');
const order = ['01-hello-world', '02-variables-and-data-types', '03-user-input'];

describe('progress', () => {
	it('marks lessons complete and incomplete without mutating', () => {
		const done = markComplete(EMPTY_PROGRESS, '01-hello-world', now);
		expect(done.completed).toEqual({ '01-hello-world': '2026-11-20T12:00:00.000Z' });
		expect(EMPTY_PROGRESS.completed).toEqual({});
		expect(markComplete(done, '01-hello-world', new Date())).toBe(done);
		expect(markIncomplete(done, '01-hello-world').completed).toEqual({});
	});

	it('counts progress for a set of lessons', () => {
		const snapshot = markComplete(markComplete(EMPTY_PROGRESS, order[0], now), order[2], now);
		expect(progressFor(snapshot, order)).toEqual({ done: 2, total: 3, percent: 67 });
		expect(progressFor(snapshot, [])).toEqual({ done: 0, total: 0, percent: 0 });
	});

	it('continues at the last lesson viewed, or the next unfinished one', () => {
		expect(continueSlug(EMPTY_PROGRESS, order)).toBeNull();
		const viewed = recordView(EMPTY_PROGRESS, order[0], now);
		expect(continueSlug(viewed, order)).toBe(order[0]);
		const finished = markComplete(markComplete(viewed, order[0], now), order[1], now);
		expect(continueSlug(finished, order)).toBe(order[2]);
		expect(continueSlug(markComplete(finished, order[2], now), order)).toBeNull();
	});

	it('survives malformed or foreign stored data', () => {
		expect(parseSnapshot(null)).toBe(EMPTY_PROGRESS);
		expect(parseSnapshot('not json')).toBe(EMPTY_PROGRESS);
		expect(parseSnapshot('{"version":2}')).toBe(EMPTY_PROGRESS);
		expect(
			parseSnapshot(
				JSON.stringify({
					version: 1,
					completed: {
						'01-hello-world': now.toISOString(),
						'../x': now.toISOString(),
						'02-a': 'never'
					},
					lastViewed: { slug: 42, at: now.toISOString() }
				})
			)
		).toEqual({ version: 1, completed: { '01-hello-world': now.toISOString() }, lastViewed: null });
	});

	it('round-trips through localStorage', () => {
		const storage = new MemoryStorage();
		const backend = localStorageBackend(storage);
		expect(backend.read()).toEqual(EMPTY_PROGRESS);
		const snapshot = recordView(markComplete(EMPTY_PROGRESS, order[1], now), order[1], now);
		backend.write(snapshot);
		expect(JSON.parse(storage.getItem(STORAGE_KEY) ?? '')).toEqual(snapshot);
		expect(backend.read()).toEqual(snapshot);
	});
});
