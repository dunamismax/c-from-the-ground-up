// Bundle scripts/migrate.ts and its dependencies (drizzle-orm, postgres) into
// build/migrate/migrate.js, so a release can migrate without node_modules,
// like the adapter-node server next to it. Runs after `vite build`.

import { rolldown } from 'rolldown';

const bundle = await rolldown({
	input: new URL('./migrate.ts', import.meta.url).pathname,
	platform: 'node',
	// The project tsconfig extends SvelteKit's generated one, which rolldown
	// cannot resolve; the migrator needs no TypeScript options.
	tsconfig: false
});
await bundle.write({
	file: new URL('../build/migrate/migrate.js', import.meta.url).pathname,
	format: 'esm',
	codeSplitting: false
});
await bundle.close();
console.log('Bundled the migrator into build/migrate/migrate.js');
