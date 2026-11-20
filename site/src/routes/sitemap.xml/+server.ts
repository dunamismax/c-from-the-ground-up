import { getCourse } from '#lib/server/content/index.js';
import { SITE_URL } from '#lib/site.js';
import type { RequestHandler } from './$types';

// Every public page, for search engines. Prerendered with the course.
export const prerender = true;

export const GET: RequestHandler = async () => {
	const course = await getCourse();
	const paths = [
		'/',
		'/lessons',
		...course.parts.map((part) => `/parts/${part.number}`),
		...course.lessons.map((lesson) => `/lessons/${lesson.slug}`),
		'/setup',
		'/about'
	];
	const urls = paths.map((path) => `\t<url><loc>${SITE_URL}${path}</loc></url>`).join('\n');
	const xml = `<?xml version="1.0" encoding="UTF-8"?>
<urlset xmlns="http://www.sitemaps.org/schemas/sitemap/0.9">
${urls}
</urlset>
`;
	return new Response(xml, { headers: { 'content-type': 'application/xml' } });
};
