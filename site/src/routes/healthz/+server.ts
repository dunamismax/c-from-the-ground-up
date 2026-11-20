import { json } from '@sveltejs/kit';
import { sql } from 'drizzle-orm';
import { getDb } from '#lib/server/db/index.js';
import { checkHealth } from '#lib/server/health.js';
import type { RequestHandler } from './$types';

// Never prerendered: it must reach the database on every request.
export const prerender = false;

// Liveness and readiness probe for Caddy and the deploy script: 200 when the
// app is up and `select 1` succeeds against the database, 503 otherwise.
export const GET: RequestHandler = async () => {
	const report = await checkHealth(() => getDb().execute(sql`select 1`));
	return json(report, {
		status: report.status === 'ok' ? 200 : 503,
		headers: { 'cache-control': 'no-store' }
	});
};
