import { error } from '@sveltejs/kit';
import { contentAvailable, getCourse } from '#lib/server/content/index.js';
import { excerpt } from '#lib/text.js';
import type { EntryGenerator, PageServerLoad } from './$types';

export const entries: EntryGenerator = async () => {
	const course = await getCourse();
	return course.parts.map((part) => ({ number: String(part.number) }));
};

export const load: PageServerLoad = async ({ params }) => {
	if (!contentAvailable()) error(404, 'Not found');
	const course = await getCourse();
	const part = course.parts.find((candidate) => String(candidate.number) === params.number);
	if (!part) error(404, 'Not found');
	const lessons = course.lessons.filter((lesson) => lesson.part.number === part.number);
	return {
		part: { number: part.number, title: part.title, summary: part.summary },
		lessons: lessons.map((lesson) => ({
			number: lesson.number,
			slug: lesson.slug,
			title: lesson.title,
			description: excerpt(lesson.description, 150),
			meta: lesson.meta
		})),
		prev: course.parts.find((candidate) => candidate.number === part.number - 1) ?? null,
		next: course.parts.find((candidate) => candidate.number === part.number + 1) ?? null
	};
};
