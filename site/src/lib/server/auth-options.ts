// Better Auth configuration (milestone W6, decision D7: GitHub sign-in only).
//
// Kept free of `$app/*` imports so the end-to-end tests can build an auth
// instance with the same options (and Better Auth's test helpers) outside
// SvelteKit. The app's instance is in auth.ts.

import type { BetterAuthOptions } from 'better-auth';
import { drizzleAdapter } from 'better-auth/adapters/drizzle';
import { SESSION_DAYS } from '../session.js';
import type { Database } from './db/index.js';
import { account, session, user, verification } from './db/schema.js';

export interface AccountsConfig {
	secret: string;
	url: string;
	githubClientId: string;
	githubClientSecret: string;
}

export interface AccountsEnv {
	BETTER_AUTH_SECRET?: string;
	BETTER_AUTH_URL?: string;
	GITHUB_CLIENT_ID?: string;
	GITHUB_CLIENT_SECRET?: string;
}

/**
 * Accounts are on when all four variables are set and off when none are.
 * Anything in between is a configuration mistake, reported at startup rather
 * than as a sign-in button that fails.
 */
export function resolveAccounts(env: AccountsEnv): AccountsConfig | null {
	const values = {
		secret: env.BETTER_AUTH_SECRET,
		url: env.BETTER_AUTH_URL,
		githubClientId: env.GITHUB_CLIENT_ID,
		githubClientSecret: env.GITHUB_CLIENT_SECRET
	};
	const set = Object.values(values).filter((value) => value !== undefined).length;
	if (set === 0) return null;
	if (set < 4) {
		throw new Error(
			'Accounts need all of BETTER_AUTH_SECRET, BETTER_AUTH_URL, GITHUB_CLIENT_ID and GITHUB_CLIENT_SECRET (or none of them)'
		);
	}
	return values as AccountsConfig;
}

export function authOptions(config: AccountsConfig, db: Database) {
	return {
		appName: 'C From The Ground Up',
		baseURL: config.url,
		basePath: '/api/auth',
		secret: config.secret,
		database: drizzleAdapter(db, {
			provider: 'pg',
			schema: { user, session, account, verification }
		}),
		socialProviders: {
			github: {
				clientId: config.githubClientId,
				clientSecret: config.githubClientSecret
			}
		},
		session: {
			expiresIn: 60 * 60 * 24 * SESSION_DAYS,
			// Extend a session at most once a day, not on every request.
			updateAge: 60 * 60 * 24
		},
		account: {
			// The GitHub token is never used after sign-in; store it encrypted.
			encryptOAuthTokens: true
		},
		// In memory: the site is one Node process. The IP comes from
		// X-Forwarded-For, which Caddy always replaces with the visitor's
		// address (BUILD.md, W5).
		rateLimit: {
			enabled: true,
			window: 60,
			max: 60,
			customRules: {
				'/sign-in/*': { window: 60, max: 10 },
				'/callback/*': { window: 60, max: 10 }
			}
		},
		telemetry: { enabled: false }
	} satisfies BetterAuthOptions;
}
