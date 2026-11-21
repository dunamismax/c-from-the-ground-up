// `pnpm run db:migrate`: apply site/drizzle/ with scripts/migrate.ts, as the
// migration role when MIGRATION_DATABASE_URL is set (the host's role model,
// as on cfromthegroundup_test), otherwise as DATABASE_URL.
import { spawnSync } from 'node:child_process';

const url = process.env.MIGRATION_DATABASE_URL ?? process.env.DATABASE_URL;
const result = spawnSync(
	process.execPath,
	[new URL('./migrate.ts', import.meta.url).pathname, 'drizzle'],
	{ stdio: 'inherit', env: url ? { ...process.env, DATABASE_URL: url } : process.env }
);
process.exit(result.status ?? 1);
