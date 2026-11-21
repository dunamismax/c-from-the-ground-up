// Apply the committed Drizzle migrations (site/drizzle/) to the database.
//
//   node scripts/migrate.ts drizzle                 local development (npm run db:migrate)
//   node build/migrate/migrate.js drizzle           production, bundled by scripts/build-migrator.ts
//   node build/migrate/migrate.js --check drizzle   read the migrations only; no database
//
// Environment:
//   DATABASE_URL   the role that runs migrations (production: cfromthegroundup_migration)
//   DB_OWNER_ROLE  optional; the migration runs `SET ROLE` to it first, so every
//                  table belongs to the owner role and the web role gets its
//                  privileges through the owner's default privileges
//
// The bookkeeping table lives next to the app's tables, in schema `app`
// (see schema.ts), so the migration role never needs another schema.
// Migrations are forward-only. A release must keep working on the schema the
// next release migrates to: migrations run while the old release still serves.

import { readMigrationFiles } from 'drizzle-orm/migrator';
import { drizzle } from 'drizzle-orm/postgres-js';
import { migrate } from 'drizzle-orm/postgres-js/migrator';
import postgres from 'postgres';

const MIGRATIONS_SCHEMA = 'app';
const MIGRATIONS_TABLE = '__drizzle_migrations';

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

// One connection, so SET ROLE applies to every statement the migrator runs.
const sql = postgres(url, { max: 1, connect_timeout: 10, onnotice: () => {} });
try {
	if (owner) await sql`set role ${sql(owner)}`;
	const applied = async () => {
		const [row] = await sql<{ count: number }[]>`
			select count(*)::int as count from ${sql(MIGRATIONS_SCHEMA)}.${sql(MIGRATIONS_TABLE)}`;
		return row.count;
	};
	// The bookkeeping table does not exist before the first migration.
	const before = await applied().catch(() => 0);
	await migrate(drizzle(sql), config);
	const after = await applied();
	console.log(
		`migrate: ${after - before} applied, ${after} of ${migrations.length} in the database`
	);
} catch (error) {
	fail(error instanceof Error ? error.message : String(error));
} finally {
	await sql.end({ timeout: 5 });
}
