import { describe, expect, it } from 'vitest';
import { resolveAccounts } from './auth-options.js';

describe('resolveAccounts', () => {
	const all = {
		BETTER_AUTH_SECRET: 'x'.repeat(32),
		BETTER_AUTH_URL: 'https://cfromthegroundup.com',
		GITHUB_CLIENT_ID: 'id',
		GITHUB_CLIENT_SECRET: 'secret'
	};

	it('turns accounts off when nothing is configured', () => {
		expect(resolveAccounts({})).toBeNull();
	});

	it('turns accounts on when everything is configured', () => {
		expect(resolveAccounts(all)).toEqual({
			secret: all.BETTER_AUTH_SECRET,
			url: all.BETTER_AUTH_URL,
			githubClientId: 'id',
			githubClientSecret: 'secret'
		});
	});

	it('refuses a partial configuration', () => {
		expect(() => resolveAccounts({ ...all, GITHUB_CLIENT_SECRET: undefined })).toThrow(
			/all of BETTER_AUTH_SECRET/
		);
	});
});
