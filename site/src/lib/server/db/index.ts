import { drizzle } from 'drizzle-orm/postgres-js';
import postgres from 'postgres';
import * as schema from './schema.js';
import { DATABASE_URL } from '$app/env/private';

export type Database = ReturnType<typeof drizzle<typeof schema>>;

let instance: Database | undefined;

/**
 * The shared Drizzle client, created on first use.
 *
 * Creating it lazily keeps `npm run build` (which imports server modules to
 * analyse and prerender routes) from needing a database or `DATABASE_URL`.
 */
export function getDb(): Database {
	if (!instance) {
		if (!DATABASE_URL) throw new Error('DATABASE_URL is not set');
		const client = postgres(DATABASE_URL, { connect_timeout: 5 });
		instance = drizzle(client, { schema });
	}
	return instance;
}
