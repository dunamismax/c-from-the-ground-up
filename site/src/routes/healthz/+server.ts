import { json } from '@sveltejs/kit';
import { sql } from 'drizzle-orm';
import { getDb } from '#lib/server/db/index.js';
import { checkHealth, readRevision } from '#lib/server/health.js';
import type { RequestHandler } from './$types';

// Never prerendered: it must reach the database on every request.
export const prerender = false;

// Read once: a release never changes after it is created.
const revision = readRevision();

// Liveness and readiness probe for the deploy's candidate check and Caddy:
// 200 with {"status":"ok","revision":"<commit>","database":"ok"} when
// `select 1` succeeds through the app's pool (as the web role), 503 otherwise.
export const GET: RequestHandler = async () => {
	const report = await checkHealth(() => getDb().execute(sql`select 1`), revision);
	return json(report, {
		status: report.status === 'ok' ? 200 : 503,
		headers: { 'cache-control': 'no-store' }
	});
};
