import { defineEnvVars } from '@sveltejs/kit/env';
import { building } from '$app/env';

// Environment variables, validated when the app builds and when it starts.
// Copy .env.example to .env for local development.
export const variables = defineEnvVars({
	DATABASE_URL: {
		description: 'PostgreSQL connection string for learner state.',
		// Optional while building (the build never touches the database, so CI
		// can build without one), required and checked when the server starts.
		schema: (value) => {
			if (building && value === undefined) return value;
			if (!value || !/^postgres(ql)?:\/\//.test(value)) {
				throw new Error('expected a postgres:// connection string');
			}
			return value;
		}
	}
});
