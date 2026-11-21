import { error } from '@sveltejs/kit';
import { contentAvailable, getCourse } from '#lib/server/content/index.js';
import { renderOgImage, type OgCard } from '#lib/server/og.js';
import { SITE_DESCRIPTION } from '#lib/site.js';
import { excerpt } from '#lib/text.js';
import type { EntryGenerator, RequestHandler } from './$types';

// Open Graph images, prerendered:
//   /og/site.png           the landing page and every page without its own
//   /og/<lesson-slug>.png  one per lesson
export const prerender = true;

export const entries: EntryGenerator = async () => {
	const course = await getCourse();
	return [{ name: 'site' }, ...course.lessons.map((lesson) => ({ name: lesson.slug }))];
};

async function cardFor(name: string): Promise<OgCard | null> {
	if (name === 'site') {
		const { lessons } = await getCourse();
		return {
			eyebrow: `A free course in ${lessons.length} lessons`,
			title: 'Learn C from the ground up.',
			description: excerpt(SITE_DESCRIPTION, 140)
		};
	}
	const lesson = (await getCourse()).lessons.find((candidate) => candidate.slug === name);
	if (!lesson) return null;
	return {
		eyebrow: `Part ${lesson.part.number} · Lesson ${lesson.number}`,
		title: lesson.title,
		description: excerpt(lesson.description, 140)
	};
}

export const GET: RequestHandler = async ({ params }) => {
	if (!contentAvailable()) error(404, 'Not found');
	const card = await cardFor(params.name);
	if (!card) error(404, 'Not found');
	return new Response(new Uint8Array(await renderOgImage(card)), {
		headers: { 'content-type': 'image/png' }
	});
};
