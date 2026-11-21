// Accounts in the end-to-end tests (milestone W6). The test server runs with
// these made-up GitHub credentials, so accounts are on but GitHub is never
// called: tests sign in by creating a session directly, with Better Auth's
// test helpers and the same options the app uses.

import { betterAuth } from 'better-auth';
import { testUtils } from 'better-auth/plugins';
import type { BrowserContext } from '@playwright/test';
import { drizzle } from 'drizzle-orm/node-postgres';
import pg from 'pg';
import { authOptions } from '../src/lib/server/auth-options.js';
import * as schema from '../src/lib/server/db/schema.js';

export const E2E_PORT = 4173;
export const E2E_ORIGIN = `http://127.0.0.1:${E2E_PORT}`;

/** Test values only: a throwaway secret and a GitHub app that does not exist. */
export const E2E_ACCOUNTS_ENV = {
	BETTER_AUTH_SECRET: 'e2e-only-secret-0123456789abcdef0123456789',
	BETTER_AUTH_URL: E2E_ORIGIN,
	GITHUB_CLIENT_ID: 'e2e-github-client-id',
	GITHUB_CLIENT_SECRET: 'e2e-github-client-secret'
};

export interface E2eDatabase {
	/** The role the server and the test helpers use (the web role). */
	url: string;
	/** The role the migrator uses; the web role itself when none is given. */
	migrationUrl: string;
	/** The owner the migrator switches to with SET ROLE, if any. */
	ownerRole: string | undefined;
}

/**
 * The test database, from the environment or site/.env: DATABASE_URL, plus
 * MIGRATION_DATABASE_URL and DB_OWNER_ROLE when the database has the host's
 * owner / web / migration roles (cfromthegroundup_test on the server).
 */
export function e2eDatabase(): E2eDatabase {
	if (!process.env.DATABASE_URL) {
		try {
			process.loadEnvFile();
		} catch {
			// No .env file.
		}
	}
	const url = process.env.DATABASE_URL;
	if (!url) throw new Error('DATABASE_URL is not set (copy .env.example to .env)');
	return {
		url,
		migrationUrl: process.env.MIGRATION_DATABASE_URL ?? url,
		ownerRole: process.env.DB_OWNER_ROLE
	};
}

let testAuth: ReturnType<typeof createTestAuth> | undefined;

function createTestAuth() {
	const db = drizzle({
		client: new pg.Pool({ connectionString: e2eDatabase().url, max: 2 }),
		schema
	});
	return betterAuth({
		...authOptions(
			{
				secret: E2E_ACCOUNTS_ENV.BETTER_AUTH_SECRET,
				url: E2E_ACCOUNTS_ENV.BETTER_AUTH_URL,
				githubClientId: E2E_ACCOUNTS_ENV.GITHUB_CLIENT_ID,
				githubClientSecret: E2E_ACCOUNTS_ENV.GITHUB_CLIENT_SECRET
			},
			db
		),
		plugins: [testUtils()]
	});
}

export interface TestLearner {
	id: string;
	name: string;
	email: string;
}

/** A new learner in the database, with a unique email address. */
export async function createLearner(name = 'Test Learner'): Promise<TestLearner> {
	testAuth ??= createTestAuth();
	const helpers = (await testAuth.$context).test;
	const unique = `${Date.now()}-${Math.random().toString(36).slice(2, 8)}`;
	const user = await helpers.saveUser(
		helpers.createUser({ name, email: `learner-${unique}@example.test`, emailVerified: true })
	);
	return { id: user.id, name: user.name, email: user.email };
}

/** Sign `context` in as `learner`, as if they had come back from GitHub. */
export async function signIn(context: BrowserContext, learner: TestLearner) {
	testAuth ??= createTestAuth();
	const helpers = (await testAuth.$context).test;
	const cookies = await helpers.getCookies({ userId: learner.id, domain: '127.0.0.1' });
	await context.addCookies(
		cookies.map((cookie) => ({
			name: cookie.name,
			value: cookie.value,
			domain: cookie.domain,
			path: cookie.path,
			httpOnly: cookie.httpOnly,
			secure: cookie.secure,
			sameSite: cookie.sameSite
		}))
	);
}
