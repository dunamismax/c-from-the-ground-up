import { loadMarkdownPage } from '#lib/server/content/page.js';
import type { PageServerLoad } from './$types';

// src/glossary.md, the same file the book uses, rendered at build time.
export const load: PageServerLoad = async () => {
	const page = await loadMarkdownPage('glossary.md');
	return { page };
};
