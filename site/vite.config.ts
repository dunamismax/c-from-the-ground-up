import { createHash } from 'node:crypto';
import { readFileSync } from 'node:fs';
import tailwindcss from '@tailwindcss/vite';
import { defineConfig } from 'vitest/config';
import { playwright } from '@vitest/browser-playwright';
import adapter from '@sveltejs/adapter-node';
import { sveltekit } from '@sveltejs/kit/vite';

// The CSP hash of each inline <script> in src/app.html (the theme script that
// runs before first paint). Computed here so editing the script cannot
// silently break the policy.
function appHtmlScriptHashes(): `sha256-${string}`[] {
	const html = readFileSync(new URL('./src/app.html', import.meta.url), 'utf8');
	return [...html.matchAll(/<script>([\s\S]*?)<\/script>/g)].map(
		(match) => `sha256-${createHash('sha256').update(match[1]).digest('base64')}` as const
	);
}

export default defineConfig({
	plugins: [
		tailwindcss(),
		sveltekit({
			compilerOptions: {
				// Force runes mode for the project, except for libraries. Can be removed in svelte 6.
				runes: ({ filename }) =>
					filename.split(/[/\\]/).includes('node_modules') ? undefined : true
			},
			adapter: adapter(),
			// Content Security Policy. Pages are prerendered, so SvelteKit uses
			// hashes for the scripts it generates (and a <meta> tag on static
			// pages). Styles allow inline attributes: Shiki colors code tokens
			// with style="", and components set widths with style:.
			csp: {
				mode: 'hash',
				directives: {
					'default-src': ['self'],
					'script-src': ['self', ...appHtmlScriptHashes()],
					'style-src': ['self', 'unsafe-inline'],
					'img-src': ['self', 'data:'],
					'font-src': ['self'],
					// "Try it online" posts to Compiler Explorer's shortener.
					'connect-src': ['self', 'https://godbolt.org'],
					'object-src': ['none'],
					'base-uri': ['self'],
					'form-action': ['self']
				}
			}
		})
	],
	test: {
		expect: { requireAssertions: true },
		projects: [
			{
				extends: './vite.config.ts',
				test: {
					name: 'client',
					browser: {
						enabled: true,
						provider: playwright(),
						instances: [{ browser: 'chromium', headless: true }]
					},
					include: ['src/**/*.svelte.{test,spec}.{js,ts}'],
					exclude: ['src/lib/server/**']
				}
			},

			{
				extends: './vite.config.ts',
				test: {
					name: 'server',
					environment: 'node',
					include: ['src/**/*.{test,spec}.{js,ts}'],
					exclude: ['src/**/*.svelte.{test,spec}.{js,ts}']
				}
			}
		]
	}
});
