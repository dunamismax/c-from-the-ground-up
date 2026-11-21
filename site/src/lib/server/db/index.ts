import { drizzle, type NodePgDatabase } from 'drizzle-orm/node-postgres';
import pg from 'pg';
import * as schema from './schema.js';
import { DATABASE_URL } from '$app/env/private';

export type Database = NodePgDatabase<typeof schema>;

let instance: Database | undefined;

/**
 * The shared Drizzle client over a `pg` connection pool, created on first use.
 *
 * Creating it lazily keeps `pnpm run build` (which imports server modules to
 * analyse and prerender routes) from needing a database or `DATABASE_URL`.
 */
export function getDb(): Database {
	if (!instance) {
		if (!DATABASE_URL) throw new Error('DATABASE_URL is not set');
		const pool = new pg.Pool({ connectionString: DATABASE_URL, connectionTimeoutMillis: 5000 });
		// An idle connection that breaks (PostgreSQL restarted, for example) is
		// reported here; without a listener Node would exit. The pool drops it
		// and opens a new one for the next query.
		pool.on('error', (error) => console.error('database pool:', error.message));
		// adapter-node emits this once the HTTP server has closed on SIGTERM, so
		// idle connections do not keep the process alive until systemd kills it.
		process.once('sveltekit:shutdown', () => void pool.end());
		instance = drizzle({ client: pool, schema });
	}
	return instance;
}
