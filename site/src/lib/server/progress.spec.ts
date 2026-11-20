import { describe, expect, it } from 'vitest';
import { clampTimestamp, snapshotFromRows } from './progress.js';

describe('snapshotFromRows', () => {
	it('collects completions and the most recent view', () => {
		const snapshot = snapshotFromRows([
			{
				lessonSlug: '01-hello-world',
				completedAt: new Date('2026-11-01T00:00:00Z'),
				lastViewedAt: new Date('2026-11-01T00:00:00Z')
			},
			{
				lessonSlug: '02-variables-and-data-types',
				completedAt: null,
				lastViewedAt: new Date('2026-11-02T00:00:00Z')
			}
		]);
		expect(snapshot).toEqual({
			version: 1,
			completed: { '01-hello-world': '2026-11-01T00:00:00.000Z' },
			lastViewed: { slug: '02-variables-and-data-types', at: '2026-11-02T00:00:00.000Z' }
		});
		expect(snapshotFromRows([])).toEqual({ version: 1, completed: {}, lastViewed: null });
	});
});

describe('clampTimestamp', () => {
	const now = new Date('2026-11-20T12:00:00Z');
	it('keeps plausible browser timestamps and replaces the rest with now', () => {
		expect(clampTimestamp('2026-11-01T00:00:00Z', now)).toEqual(new Date('2026-11-01T00:00:00Z'));
		expect(clampTimestamp('2030-01-01T00:00:00Z', now)).toBe(now);
		expect(clampTimestamp('1999-01-01T00:00:00Z', now)).toBe(now);
		expect(clampTimestamp('not a date', now)).toBe(now);
	});
});
