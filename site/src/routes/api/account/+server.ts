import { eq } from 'drizzle-orm';
import { noStoreJson, requireUser } from '#lib/server/api.js';
import { getAuth } from '#lib/server/auth.js';
import { getDb } from '#lib/server/db/index.js';
import { user as userTable } from '#lib/server/db/schema.js';
import type { RequestHandler } from './$types';

// Account state for the browser: whether sign-in is available at all, and who
// is signed in. Pages are prerendered, so they ask here after loading.
export const prerender = false;

export interface AccountState {
	enabled: boolean;
	user: { id: string; name: string; email: string } | null;
}

export const GET: RequestHandler = ({ locals }) => {
	const state: AccountState = { enabled: getAuth() !== null, user: locals.user };
	return noStoreJson(state);
};

// Delete the account and everything stored with it (sessions, the GitHub
// link and progress all cascade from the user row), then end the session.
export const DELETE: RequestHandler = async ({ locals, request }) => {
	const user = requireUser(locals);
	// Clears the session cookie through the sveltekitCookies plugin, while the
	// session still exists.
	await getAuth()?.api.signOut({ headers: request.headers });
	await getDb().delete(userTable).where(eq(userTable.id, user.id));
	return new Response(null, { status: 204, headers: { 'cache-control': 'no-store' } });
};
