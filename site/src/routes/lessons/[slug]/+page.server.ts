import { error } from '@sveltejs/kit';
import { contentAvailable, getCourse, getLesson } from '#lib/server/content/index.js';
import { excerpt } from '#lib/text.js';
import type { EntryGenerator, PageServerLoad } from './$types';

export const entries: EntryGenerator = async () => {
	const course = await getCourse();
	return course.lessons.map((lesson) => ({ slug: lesson.slug }));
};

export const load: PageServerLoad = async ({ params }) => {
	// Lesson pages are prerendered. At runtime the server has no repository to
	// read, so an unknown slug that reaches it is simply not found.
	if (!contentAvailable()) error(404, 'Not found');
	const lesson = await getLesson(params.slug);
	if (!lesson) error(404, 'Not found');
	const course = await getCourse();

	return {
		lesson: {
			...lesson,
			// The search index carries the prose text; the page does not need it.
			searchSections: [],
			// The code is already in the highlighted HTML; do not ship it twice.
			sources: lesson.sources.map(({ contents, ...source }) => source),
			solutions: lesson.solutions.map(({ contents, ...source }) => source)
		},
		summary: excerpt(lesson.description, 160),
		outline: course.parts.map(({ number, title, lessons }) => ({ number, title, lessons }))
	};
};
