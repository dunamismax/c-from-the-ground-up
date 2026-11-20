import { defineConfig } from '@playwright/test';

// End-to-end tests run against the real production server (adapter-node's
// `node build`), not the Vite dev server. /healthz needs the database, so start
// it first with `npm run db:start`.
export default defineConfig({
	testMatch: '**/*.e2e.{ts,js}',
	use: { baseURL: 'http://127.0.0.1:4173' },
	webServer: {
		command: 'npm run build && npm run start',
		url: 'http://127.0.0.1:4173',
		env: { HOST: '127.0.0.1', PORT: '4173' },
		reuseExistingServer: !process.env.CI
	}
});
