// The course content loader. It reads src/SUMMARY.md, every chapter and every
// lesson source file from the repository at build time and returns typed,
// serializable objects. Lesson pages are prerendered, so none of this runs in
// production; the site never keeps its own copy of the content.

import { readdir, readFile } from 'node:fs/promises';
import path from 'node:path';
import type { Course, Lesson, LessonRef, Part, SourceFile } from '#lib/content/types.js';
import type { Highlighter } from 'shiki';
import { parseChapter, renderChapter } from './chapter.js';
import { getHighlighter, highlightToHtml } from './highlight.js';
import { lessonCommands, lessonMeta } from './metadata.js';
import { courseRoot, findCourseRoot, githubBlobUrl, githubRawUrl } from './paths.js';
import { parsePartSummaries, parseSummary } from './summary.js';

/** Anchor of the solutions section on a lesson page; the chapters link to it. */
export const SOLUTIONS_ID = 'solutions';

function sourceId(name: string): string {
	return `source-${name.toLowerCase().replace(/[^a-z0-9]+/g, '-')}`;
}

/** Read one repository file and highlight it. */
async function loadSource(
	root: string,
	repoPath: string,
	highlighter: Highlighter
): Promise<SourceFile> {
	const contents = await readFile(path.join(root, repoPath), 'utf8');
	const name = path.posix.basename(repoPath);
	const language = name === 'Makefile' ? 'makefile' : 'c';
	return {
		path: repoPath,
		name,
		id: sourceId(name),
		language,
		contents,
		lineCount: contents.replace(/\n$/, '').split('\n').length,
		html: highlightToHtml(highlighter, contents, language),
		githubUrl: githubBlobUrl(repoPath),
		rawUrl: githubRawUrl(repoPath)
	};
}

/**
 * The worked solutions in `exercises/<slug>/`, in exercise order (their names
 * start with the exercise number). scripts/sync_chapters.py checks that the
 * directory matches the chapter's exercises.
 */
async function loadSolutions(
	root: string,
	slug: string,
	highlighter: Highlighter
): Promise<SourceFile[]> {
	const directory = `exercises/${slug}`;
	let names: string[];
	try {
		names = await readdir(path.join(root, directory));
	} catch (error) {
		if ((error as NodeJS.ErrnoException).code === 'ENOENT') return [];
		throw error;
	}
	const solutions = names
		.filter((name) => /^\d+_.+\.c$/.test(name))
		.sort((a, b) => parseInt(a, 10) - parseInt(b, 10));
	return Promise.all(
		solutions.map((name) => loadSource(root, `${directory}/${name}`, highlighter))
	);
}

export async function loadCourse(root: string = courseRoot()): Promise<Course> {
	const highlighter = await getHighlighter();
	const summary = parseSummary(await readFile(path.join(root, 'src/SUMMARY.md'), 'utf8'));
	const partSummaries = parsePartSummaries(await readFile(path.join(root, 'src/index.md'), 'utf8'));
	const lessons: Lesson[] = [];
	const parts: Part[] = [];

	for (const summaryPart of summary) {
		const summaryText = partSummaries.get(summaryPart.number);
		if (!summaryText) {
			throw new Error(`src/index.md: no "## Structure" entry for Part ${summaryPart.number}`);
		}
		const part: Part = {
			number: summaryPart.number,
			title: summaryPart.title,
			summary: summaryText,
			lessons: []
		};
		parts.push(part);

		for (const entry of summaryPart.chapters) {
			const chapterPath = `src/${entry.file}`;
			const parsed = parseChapter(
				await readFile(path.join(root, chapterPath), 'utf8'),
				chapterPath
			);
			const rendered = await renderChapter(parsed, highlighter);

			const sources: SourceFile[] = [];
			for (const listing of parsed.listings) {
				// Read the file itself, the single source of truth, rather than the
				// copy embedded in the chapter.
				sources.push(await loadSource(root, listing.path, highlighter));
			}
			const solutions = await loadSolutions(root, entry.slug, highlighter);

			const meta = lessonMeta(
				entry.number,
				sources.map((source) => source.name)
			);
			lessons.push({
				number: entry.number,
				slug: entry.slug,
				title: entry.title,
				part: { number: part.number, title: part.title },
				chapterPath,
				description: rendered.description,
				segments: rendered.segments,
				// The solutions section is added by the lesson page, after the chapter.
				toc:
					solutions.length > 0
						? [...rendered.toc, { depth: 2, id: SOLUTIONS_ID, text: 'Solutions' }]
						: rendered.toc,
				searchSections: rendered.sections,
				sources,
				solutions,
				runNotesHtml: rendered.runNotesHtml,
				meta,
				commands: lessonCommands(meta),
				prev: null,
				next: null
			});
			part.lessons.push({ number: entry.number, slug: entry.slug, title: entry.title });
		}
	}

	const ref = (lesson: Lesson): LessonRef => ({
		number: lesson.number,
		slug: lesson.slug,
		title: lesson.title
	});
	lessons.forEach((lesson, index) => {
		lesson.prev = index > 0 ? ref(lessons[index - 1]) : null;
		lesson.next = index < lessons.length - 1 ? ref(lessons[index + 1]) : null;
	});

	return { parts, lessons };
}

/**
 * Whether the repository's content is on disk. It is at build time and in
 * development; the production server only has the build. Routes that load
 * content are prerendered, but SvelteKit can still reach them at runtime for
 * an unknown path such as /lessons/nope, which should be a 404, not a 500.
 */
export function contentAvailable(): boolean {
	return findCourseRoot() !== null;
}

let cached: Promise<Course> | undefined;

/**
 * The course, loaded once per build. In development it is reloaded on every
 * call, so edits to chapters and lessons show up on refresh.
 */
export function getCourse(): Promise<Course> {
	if (import.meta.env.DEV) return loadCourse();
	cached ??= loadCourse();
	return cached;
}

export async function getLesson(slug: string): Promise<Lesson | undefined> {
	const course = await getCourse();
	return course.lessons.find((lesson) => lesson.slug === slug);
}
