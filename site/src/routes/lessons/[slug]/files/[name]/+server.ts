import { error } from '@sveltejs/kit';
import { contentAvailable, getCourse, getLesson } from '#lib/server/content/index.js';
import type { EntryGenerator, RequestHandler } from './$types';

// Each lesson file as a plain download, prerendered next to its lesson:
// /lessons/01-hello-world/files/1_hello_world.c
export const prerender = true;

export const entries: EntryGenerator = async () => {
	const course = await getCourse();
	return course.lessons.flatMap((lesson) =>
		lesson.sources.map((source) => ({ slug: lesson.slug, name: source.name }))
	);
};

export const GET: RequestHandler = async ({ params }) => {
	if (!contentAvailable()) error(404, 'Not found');
	const lesson = await getLesson(params.slug);
	const source = lesson?.sources.find((file) => file.name === params.name);
	if (!source) error(404, 'Not found');
	return new Response(source.contents, {
		headers: { 'content-type': 'text/plain; charset=utf-8' }
	});
};
