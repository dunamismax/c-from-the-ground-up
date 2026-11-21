import { defineConfig } from '@playwright/test';
import { E2E_ACCOUNTS_ENV, E2E_ORIGIN, E2E_PORT, e2eDatabase } from './e2e/accounts.js';

// End-to-end tests run against the real production server (adapter-node's
// `node build`), not the Vite dev server, with the committed migrations
// applied by the bundled migrator first. They need a PostgreSQL 18 database:
// on the server, cfromthegroundup_test on the host cluster (see .env.example).
// Accounts are on, with test-only settings (e2e/accounts.ts).
const db = e2eDatabase();

export default defineConfig({
	testMatch: '**/*.e2e.{ts,js}',
	use: { baseURL: E2E_ORIGIN },
	webServer: {
		// The migrator runs as the migration role when one is given (as in
		// production), the server as the web role.
		command:
			'pnpm run build && DATABASE_URL="$E2E_MIGRATION_URL" node build/migrate/migrate.js drizzle && node build',
		url: E2E_ORIGIN,
		// ORIGIN as in production: without it adapter-node assumes https, and
		// Better Auth would not recognise its own URLs.
		env: {
			HOST: '127.0.0.1',
			PORT: String(E2E_PORT),
			ORIGIN: E2E_ORIGIN,
			DATABASE_URL: db.url,
			E2E_MIGRATION_URL: db.migrationUrl,
			...(db.ownerRole ? { DB_OWNER_ROLE: db.ownerRole } : {}),
			...E2E_ACCOUNTS_ENV
		},
		reuseExistingServer: !process.env.CI
	}
});
