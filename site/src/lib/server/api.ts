import { error, json } from '@sveltejs/kit';
import * as v from 'valibot';
import type { SessionUser } from './auth.js';

/** The signed-in learner, or a 401 for the API caller. */
export function requireUser(locals: App.Locals): SessionUser {
	if (!locals.user) error(401, 'Not signed in');
	return locals.user;
}

/** Parse and validate a JSON request body, or a 400. */
export async function readBody<T extends v.GenericSchema>(
	request: Request,
	schema: T
): Promise<v.InferOutput<T>> {
	if (!request.headers.get('content-type')?.startsWith('application/json')) {
		error(415, 'Expected application/json');
	}
	let data: unknown;
	try {
		data = await request.json();
	} catch {
		error(400, 'Invalid JSON');
	}
	const result = v.safeParse(schema, data);
	if (!result.success) error(400, 'Invalid request body');
	return result.output;
}

export function noStoreJson(data: unknown, init?: ResponseInit) {
	return json(data, {
		...init,
		headers: { 'cache-control': 'private, no-store', ...init?.headers }
	});
}
