import { defineConfig } from '@playwright/test';
import { E2E_ACCOUNTS_ENV, E2E_ORIGIN, E2E_PORT } from './e2e/accounts.js';

// End-to-end tests run against the real production server (adapter-node's
// `node build`), not the Vite dev server, with the committed migrations
// applied by the bundled migrator first. Start the database with
// `npm run db:start`. Accounts are on, with test-only settings (e2e/accounts.ts).
export default defineConfig({
	testMatch: '**/*.e2e.{ts,js}',
	use: { baseURL: E2E_ORIGIN },
	webServer: {
		command:
			'npm run build && node --env-file-if-exists=.env build/migrate/migrate.js drizzle && npm run start',
		url: E2E_ORIGIN,
		// ORIGIN as in production: without it adapter-node assumes https, and
		// Better Auth would not recognise its own URLs.
		env: { HOST: '127.0.0.1', PORT: String(E2E_PORT), ORIGIN: E2E_ORIGIN, ...E2E_ACCOUNTS_ENV },
		reuseExistingServer: !process.env.CI
	}
});
