import { getCourse } from '#lib/server/content/index.js';
import type { PageServerLoad } from './$types';

export const load: PageServerLoad = async () => {
	const course = await getCourse();
	const first = course.lessons[0];
	return {
		parts: course.parts,
		lessonCount: course.lessons.length,
		first: { slug: first.slug, title: first.title, commands: first.commands }
	};
};
