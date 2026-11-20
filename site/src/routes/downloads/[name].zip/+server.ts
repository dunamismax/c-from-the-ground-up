import { readdir, readFile } from 'node:fs/promises';
import path from 'node:path';
import { error } from '@sveltejs/kit';
import { contentAvailable, getCourse } from '#lib/server/content/index.js';
import { courseRoot } from '#lib/server/content/paths.js';
import { zipFiles } from '#lib/server/content/zip.js';
import type { EntryGenerator, RequestHandler } from './$types';

// Zip downloads, prerendered:
//   /downloads/c-from-the-ground-up.zip  every lesson file, in its Part folder
//   /downloads/<lesson-slug>.zip          one lesson's files
export const prerender = true;

const COURSE = 'c-from-the-ground-up';

export const entries: EntryGenerator = async () => {
	const course = await getCourse();
	return [{ name: COURSE }, ...course.lessons.map((lesson) => ({ name: lesson.slug }))];
};

async function courseFiles(): Promise<Record<string, string>> {
	const root = courseRoot();
	const files: Record<string, string> = {};
	for (const name of ['README.md', 'LICENSE']) {
		files[`${COURSE}/${name}`] = await readFile(path.join(root, name), 'utf8');
	}
	for (const part of (await readdir(root)).filter((name) => name.startsWith('Part '))) {
		for (const entry of await readdir(path.join(root, part), { recursive: true })) {
			const rel = `${part}/${entry.split(path.sep).join('/')}`;
			if (/\.(c|h)$/.test(rel) || path.posix.basename(rel) === 'Makefile') {
				files[`${COURSE}/${rel}`] = await readFile(path.join(root, rel), 'utf8');
			}
		}
	}
	return files;
}

export const GET: RequestHandler = async ({ params }) => {
	if (!contentAvailable()) error(404, 'Not found');
	let files: Record<string, string>;
	if (params.name === COURSE) {
		files = await courseFiles();
	} else {
		const lesson = (await getCourse()).lessons.find((candidate) => candidate.slug === params.name);
		if (!lesson) error(404, 'Not found');
		files = Object.fromEntries(
			lesson.sources.map((source) => [`${lesson.slug}/${source.name}`, source.contents])
		);
	}
	return new Response(new Uint8Array(zipFiles(files)), {
		headers: { 'content-type': 'application/zip' }
	});
};
