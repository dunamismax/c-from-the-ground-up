import { describe, expect, it } from 'vitest';
import { trackerAttributes, UMAMI_WEBSITE_ID } from './analytics.js';

describe('trackerAttributes', () => {
	it('is off without a website ID', () => {
		expect(trackerAttributes('')).toBeNull();
	});

	it('loads the tracker from this origin, for the real domain only, honouring Do Not Track', () => {
		expect(trackerAttributes('94db1cb1-74f4-4a40-ad6c-962362670409')).toEqual({
			src: '/stats/script.js',
			'data-website-id': '94db1cb1-74f4-4a40-ad6c-962362670409',
			'data-domains': 'cfromthegroundup.com',
			'data-do-not-track': 'true'
		});
	});

	it('rejects an ID that is not a UUID', () => {
		expect(() => trackerAttributes('not-an-id')).toThrow(/UUID/);
	});

	it('accepts the committed ID', () => {
		expect(() => trackerAttributes(UMAMI_WEBSITE_ID)).not.toThrow();
	});
});
