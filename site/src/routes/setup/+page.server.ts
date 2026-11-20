import { getCourse } from '#lib/server/content/index.js';
import { C_STANDARD } from '#lib/server/content/metadata.js';
import type { PageServerLoad } from './$types';

export const load: PageServerLoad = async () => {
	const course = await getCourse();
	const first = course.lessons[0];
	return {
		standard: C_STANDARD,
		first: {
			slug: first.slug,
			title: first.title,
			file: first.sources[0].name,
			folder: first.sources[0].path.split('/')[0],
			commands: first.commands
		}
	};
};
