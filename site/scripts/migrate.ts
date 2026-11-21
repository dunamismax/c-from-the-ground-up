// Apply the committed Drizzle migrations (site/drizzle/) to the database.
//
//   node scripts/migrate.ts drizzle                 local development (pnpm run db:migrate)
//   node build/migrate/migrate.js drizzle           production, bundled by scripts/build-migrator.ts
//   node build/migrate/migrate.js --check drizzle   read the migrations only; no database
//
// Environment:
//   DATABASE_URL   the role that runs migrations (production: cfromthegroundup_migration)
//   DB_OWNER_ROLE  optional; the migration runs `SET ROLE` to it first, so every
//                  table belongs to the owner role and the web role gets its
//                  privileges through the owner's default privileges
//
// The bookkeeping table is drizzle.__drizzle_migrations, as on every site on
// the host (ubuntu-vm-backup docs/web-platform.md). Until November 2026 this
// site kept it in schema `app`; the first run of this version copies those
// rows across (see adoptLegacyBookkeeping) and leaves the old table alone.
// Migrations are forward-only. A release must keep working on the schema the
// next release migrates to: migrations run while the old release still serves.

import { readMigrationFiles } from 'drizzle-orm/migrator';
import { drizzle } from 'drizzle-orm/node-postgres';
import { migrate } from 'drizzle-orm/node-postgres/migrator';
import pg from 'pg';

const MIGRATIONS_SCHEMA = 'drizzle';
const MIGRATIONS_TABLE = '__drizzle_migrations';
const LEGACY_SCHEMA = 'app';

function fail(message: string): never {
	console.error(`migrate: ${message}`);
	process.exit(1);
}

const args = process.argv.slice(2);
const checkOnly = args[0] === '--check';
const migrationsFolder = checkOnly ? args[1] : args[0];
if (!migrationsFolder || args.length > (checkOnly ? 2 : 1)) {
	fail('usage: migrate [--check] MIGRATIONS_FOLDER');
}

const config = {
	migrationsFolder,
	migrationsSchema: MIGRATIONS_SCHEMA,
	migrationsTable: MIGRATIONS_TABLE
};
// Fails on a missing journal or migration file, which is what --check is for.
let migrations: ReturnType<typeof readMigrationFiles>;
try {
	migrations = readMigrationFiles(config);
} catch (error) {
	fail(`${migrationsFolder}: ${error instanceof Error ? error.message : String(error)}`);
}
if (checkOnly) {
	console.log(`migrate: ${migrations.length} migration(s) in ${migrationsFolder}`);
	process.exit(0);
}

const url = process.env.DATABASE_URL;
if (!url) fail('DATABASE_URL is not set');
const owner = process.env.DB_OWNER_ROLE;
if (owner !== undefined && !/^[a-z_][a-z0-9_]*$/.test(owner)) fail('invalid DB_OWNER_ROLE');

const table = (schema: string) => `"${schema}"."${MIGRATIONS_TABLE}"`;

/**
 * Copy the rows of app.__drizzle_migrations into drizzle.__drizzle_migrations
 * when only the old table exists, in one transaction, so Drizzle's migrator
 * sees the migrations that are already applied. Same table definition as the
 * one Drizzle's node-postgres migrator creates.
 */
async function adoptLegacyBookkeeping(client: pg.Client) {
	const { rows } = await client.query<{ legacy: boolean; current: boolean }>(
		`select to_regclass($1) is not null as legacy, to_regclass($2) is not null as current`,
		[table(LEGACY_SCHEMA), table(MIGRATIONS_SCHEMA)]
	);
	if (!rows[0].legacy || rows[0].current) return;
	await client.query('begin');
	try {
		await client.query(`create schema if not exists "${MIGRATIONS_SCHEMA}"`);
		await client.query(
			`create table ${table(MIGRATIONS_SCHEMA)} (id serial primary key, hash text not null, created_at bigint)`
		);
		const copied = await client.query(
			`insert into ${table(MIGRATIONS_SCHEMA)} (id, hash, created_at)
			 select id, hash, created_at from ${table(LEGACY_SCHEMA)} order by id`
		);
		await client.query(
			`select setval(pg_get_serial_sequence($1, 'id'), coalesce(max(id), 1), max(id) is not null)
			 from ${table(MIGRATIONS_SCHEMA)}`,
			[`${MIGRATIONS_SCHEMA}.${MIGRATIONS_TABLE}`]
		);
		await client.query('commit');
		console.log(
			`migrate: moved ${copied.rowCount} bookkeeping row(s) from ${LEGACY_SCHEMA} to ${MIGRATIONS_SCHEMA}`
		);
	} catch (error) {
		await client.query('ROLLBACK');
		throw error;
	}
}

// One connection, so SET ROLE applies to every statement the migrator runs.
const client = new pg.Client({ connectionString: url, connectionTimeoutMillis: 10_000 });
try {
	await client.connect();
	if (owner) await client.query(`set role ${client.escapeIdentifier(owner)}`);
	await adoptLegacyBookkeeping(client);
	const applied = async () => {
		const { rows } = await client.query<{ count: number }>(
			`select count(*)::int as count from ${table(MIGRATIONS_SCHEMA)}`
		);
		return rows[0].count;
	};
	// The bookkeeping table does not exist before the first migration.
	const before = await applied().catch(() => 0);
	await migrate(drizzle({ client }), config);
	const after = await applied();
	console.log(
		`migrate: ${after - before} applied, ${after} of ${migrations.length} in the database`
	);
} catch (error) {
	fail(error instanceof Error ? error.message : String(error));
} finally {
	await client.end().catch(() => {});
}
