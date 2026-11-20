import { getCourse } from '#lib/server/content/index.js';
import type { PageServerLoad } from './$types';

export const load: PageServerLoad = async () => {
	const course = await getCourse();
	return { slugs: course.lessons.map((lesson) => lesson.slug) };
};
