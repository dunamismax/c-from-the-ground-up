import { OpsBody, SnapshotBody } from '#lib/server/progress-schema.js';
import { noStoreJson, readBody, requireUser } from '#lib/server/api.js';
import { getDb } from '#lib/server/db/index.js';
import { applyOps, mergeProgress, readProgress } from '#lib/server/progress.js';
import type { RequestHandler } from './$types';

// The signed-in learner's progress (see #lib/progress/api.ts for the protocol).
export const prerender = false;

export const GET: RequestHandler = async ({ locals }) => {
	const user = requireUser(locals);
	return noStoreJson(await readProgress(getDb(), user.id));
};

export const POST: RequestHandler = async ({ locals, request }) => {
	const user = requireUser(locals);
	const { ops } = await readBody(request, OpsBody);
	return noStoreJson(await applyOps(getDb(), user.id, ops));
};

export const PUT: RequestHandler = async ({ locals, request }) => {
	const user = requireUser(locals);
	const snapshot = await readBody(request, SnapshotBody);
	return noStoreJson(await mergeProgress(getDb(), user.id, snapshot));
};
