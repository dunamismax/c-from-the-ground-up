import MiniSearch from 'minisearch';
import { cComments, makefileComments } from '#lib/server/content/comments.js';
import { getCourse } from '#lib/server/content/index.js';
import { INDEX_OPTIONS, type SearchDocument } from '#lib/search.js';
import { excerpt } from '#lib/text.js';
import type { RequestHandler } from './$types';

// The search index, built once at build time and served as a static file.
export const prerender = true;

export const GET: RequestHandler = async () => {
	const course = await getCourse();
	const documents: SearchDocument[] = [];
	for (const lesson of course.lessons) {
		const base = { slug: lesson.slug, number: lesson.number, lessonTitle: lesson.title };
		for (const section of lesson.searchSections) {
			// The introduction carries the lesson title; empty sections such as
			// "Full Source" would only repeat it.
			if (section.id && !section.text) continue;
			documents.push({
				...base,
				id: section.id ? `${lesson.slug}#${section.id}` : lesson.slug,
				title: section.id ? '' : lesson.title,
				heading: section.heading,
				text: section.text,
				excerpt: excerpt(section.text, 140)
			});
		}
		for (const source of lesson.sources) {
			const text =
				source.language === 'makefile'
					? makefileComments(source.contents)
					: cComments(source.contents);
			documents.push({
				...base,
				id: `${lesson.slug}#${source.id}`,
				title: '',
				heading: source.name,
				text,
				excerpt: excerpt(text, 140)
			});
		}
	}
	const index = new MiniSearch<SearchDocument>(INDEX_OPTIONS);
	index.addAll(documents);
	return new Response(JSON.stringify(index), {
		headers: { 'content-type': 'application/json' }
	});
};
