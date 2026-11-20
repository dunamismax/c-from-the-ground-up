import { getCourse } from '#lib/server/content/index.js';
import { excerpt } from '#lib/text.js';
import type { PageServerLoad } from './$types';

export const load: PageServerLoad = async () => {
	const course = await getCourse();
	return {
		parts: course.parts.map((part) => ({
			number: part.number,
			title: part.title,
			summary: part.summary,
			lessons: course.lessons
				.filter((lesson) => lesson.part.number === part.number)
				.map((lesson) => ({
					number: lesson.number,
					slug: lesson.slug,
					title: lesson.title,
					description: excerpt(lesson.description, 150),
					meta: lesson.meta
				}))
		}))
	};
};
