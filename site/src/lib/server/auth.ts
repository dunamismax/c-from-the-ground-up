import { betterAuth } from 'better-auth';
import { sveltekitCookies } from 'better-auth/svelte-kit';
import { getRequestEvent } from '$app/server';
import * as env from '$app/env/private';
import { authOptions, resolveAccounts } from './auth-options.js';
import { getDb } from './db/index.js';

function createAuth() {
	const config = resolveAccounts(env);
	if (!config) return null;
	return betterAuth({
		...authOptions(config, getDb()),
		// Must be the last plugin: lets auth calls made inside a request set cookies.
		plugins: [sveltekitCookies(getRequestEvent)]
	});
}

export type Auth = NonNullable<ReturnType<typeof createAuth>>;

let instance: Auth | null | undefined;

/**
 * The Better Auth instance, or null when accounts are not configured (then
 * the site behaves as in milestone W4: progress stays in the browser).
 * Created on first use, like the database client.
 */
export function getAuth(): Auth | null {
	if (instance === undefined) instance = createAuth();
	return instance;
}

export interface SessionUser {
	id: string;
	name: string;
	email: string;
}

/** The signed-in learner for a request, or null. */
export async function getSessionUser(headers: Headers): Promise<SessionUser | null> {
	const auth = getAuth();
	if (!auth) return null;
	const result = await auth.api.getSession({ headers });
	if (!result) return null;
	const { id, name, email } = result.user;
	return { id, name, email };
}
