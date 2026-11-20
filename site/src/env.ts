import { defineEnvVars } from '@sveltejs/kit/env';
import { building } from '$app/env';

/** Unset is fine; an empty string is a mistake in the environment file. */
function optional(check?: (value: string) => void) {
	return (value: string | undefined) => {
		if (value === undefined) return value;
		if (value === '') throw new Error('is empty; remove it or give it a value');
		check?.(value);
		return value;
	};
}

// Environment variables, validated when the app builds and when it starts.
// Copy .env.example to .env for local development.
export const variables = defineEnvVars({
	DATABASE_URL: {
		description: 'PostgreSQL connection string for learner state.',
		// Optional while building (the build never touches the database, so CI
		// can build without one), required and checked when the server starts.
		schema: (value) => {
			if (building && value === undefined) return value;
			if (!value || !/^postgres(ql)?:\/\//.test(value)) {
				throw new Error('expected a postgres:// connection string');
			}
			return value;
		}
	},

	// Accounts (milestone W6) are on only when all four of these are set; see
	// #lib/server/auth.ts. Without them the site works as before, with
	// progress kept in the browser.
	BETTER_AUTH_SECRET: {
		description: 'Signs session cookies and encrypts stored OAuth tokens (32+ random bytes).',
		schema: optional((value) => {
			if (value.length < 32) throw new Error('must be at least 32 characters');
		})
	},
	BETTER_AUTH_URL: {
		description: 'Public origin of the site, for OAuth callbacks (https://cfromthegroundup.com).',
		schema: optional((value) => {
			if (new URL(value).origin !== value) throw new Error('expected an origin, without a path');
		})
	},
	GITHUB_CLIENT_ID: {
		description: 'Client ID of the GitHub OAuth app used for sign-in.',
		schema: optional()
	},
	GITHUB_CLIENT_SECRET: {
		description: 'Client secret of the GitHub OAuth app used for sign-in.',
		schema: optional()
	}
});
