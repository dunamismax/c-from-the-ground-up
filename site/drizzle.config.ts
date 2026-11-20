import { defineConfig } from 'drizzle-kit';

// drizzle-kit runs outside Vite, so load site/.env ourselves when it exists.
if (!process.env.DATABASE_URL) {
	try {
		process.loadEnvFile();
	} catch {
		// No .env file: fall through to the check below.
	}
}

if (!process.env.DATABASE_URL) throw new Error('DATABASE_URL is not set');

export default defineConfig({
	schema: './src/lib/server/db/schema.ts',
	out: './drizzle',
	dialect: 'postgresql',
	dbCredentials: { url: process.env.DATABASE_URL },
	verbose: true,
	strict: true
});
