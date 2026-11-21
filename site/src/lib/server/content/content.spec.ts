import { readdirSync, readFileSync } from 'node:fs';
import path from 'node:path';
import { describe, expect, it } from 'vitest';
import { parseChapter } from './chapter.js';
import { loadCourse } from './index.js';
import { courseRoot } from './paths.js';
import { parseSummary } from './summary.js';

// These tests read the real course: src/SUMMARY.md, every chapter and every
// lesson file in the repository.
const root = courseRoot();
const course = await loadCourse(root);
const summary = parseSummary(readFileSync(path.join(root, 'src/SUMMARY.md'), 'utf8'));
const chapterFiles = readdirSync(path.join(root, 'src/chapters')).filter((name) =>
	name.endsWith('.md')
);

// Parse every chapter once, while the file loads, like `course` above. Doing it
// inside each test made three tests re-highlight all 35 chapters, and in a full
// `npm test` run (sharing the CPU with the browser tests) that could pass
// Vitest's 5-second per-test timeout.
const parsedChapters = new Map(
	chapterFiles.map((file) => {
		const chapterPath = `src/chapters/${file}`;
		return [
			chapterPath,
			parseChapter(readFileSync(path.join(root, chapterPath), 'utf8'), chapterPath)
		] as const;
	})
);

function parsedChapter(chapterPath: string) {
	const parsed = parsedChapters.get(chapterPath);
	if (!parsed) throw new Error(`${chapterPath} is not in src/chapters`);
	return parsed;
}

function lessonSourceFiles(): string[] {
	const found: string[] = [];
	for (const part of readdirSync(root).filter((name) => name.startsWith('Part '))) {
		for (const entry of readdirSync(path.join(root, part), { recursive: true })) {
			const rel = `${part}/${String(entry)}`;
			if (/\.(c|h)$/.test(rel) || path.posix.basename(rel) === 'Makefile') found.push(rel);
		}
	}
	return found.sort();
}

describe('course content', () => {
	it('yields one lesson per SUMMARY.md chapter, numbered 1 to 38', () => {
		const summaryChapters = summary.flatMap((part) => part.chapters);
		expect(course.lessons.map((lesson) => lesson.slug)).toEqual(
			summaryChapters.map((chapter) => chapter.slug)
		);
		expect(course.lessons.map((lesson) => lesson.number)).toEqual(
			Array.from({ length: 38 }, (_, i) => i + 1)
		);
		expect(course.parts.map((part) => part.number)).toEqual([1, 2, 3, 4, 5, 6]);
	});

	it('has unique slugs that match the chapter file names', () => {
		const slugs = course.lessons.map((lesson) => lesson.slug);
		expect(new Set(slugs).size).toBe(slugs.length);
		expect(slugs.map((slug) => `${slug}.md`).sort()).toEqual([...chapterFiles].sort());
		for (const lesson of course.lessons) {
			expect(lesson.chapterPath).toBe(`src/chapters/${lesson.slug}.md`);
			expect(lesson.slug.startsWith(String(lesson.number).padStart(2, '0') + '-')).toBe(true);
		}
	});

	it('uses the same title in SUMMARY.md and the chapter heading', () => {
		for (const lesson of course.lessons) {
			const parsed = parsedChapter(lesson.chapterPath);
			expect(parsed.title, lesson.chapterPath).toBe(lesson.title);
		}
	});

	it('embeds every lesson source file exactly once, identical to the file on disk', () => {
		const embedded: string[] = [];
		for (const [chapterPath, parsed] of parsedChapters) {
			for (const listing of parsed.listings) {
				const onDisk = readFileSync(path.join(root, listing.path), 'utf8');
				expect(listing.embedded, `${chapterPath}: run make sync`).toBe(onDisk.replace(/\n$/, ''));
				embedded.push(listing.path);
			}
		}
		expect(embedded.sort()).toEqual(lessonSourceFiles());
	});

	it('serves the files on disk as lesson sources', () => {
		for (const lesson of course.lessons) {
			expect(lesson.sources.length, lesson.slug).toBeGreaterThan(0);
			for (const source of lesson.sources) {
				expect(source.contents).toBe(readFileSync(path.join(root, source.path), 'utf8'));
				expect(source.html).toContain('class="shiki');
				expect(source.githubUrl).toMatch(
					/^https:\/\/github\.com\/dunamismax\/c-from-the-ground-up\/blob\/main\//
				);
			}
		}
		const bySlug = Object.fromEntries(course.lessons.map((lesson) => [lesson.slug, lesson]));
		expect(bySlug['26-simple-sockets'].sources.map((source) => source.name)).toEqual([
			'26_simple_socket_client.c',
			'26_simple_socket_server.c'
		]);
		expect(bySlug['31-makefiles'].sources.map((source) => source.name)).toEqual([
			'helper.h',
			'helper.c',
			'main.c',
			'Makefile'
		]);
	});

	it('gives every lesson prose, a description and a compile and run section', () => {
		for (const lesson of course.lessons) {
			const html = lesson.segments.filter((segment) => segment.kind === 'html');
			expect(html.length, lesson.slug).toBeGreaterThan(0);
			expect(lesson.description.length, lesson.slug).toBeGreaterThan(20);
			expect(
				lesson.segments.filter((segment) => segment.kind === 'run'),
				lesson.slug
			).toHaveLength(1);
			const sourceSegments = lesson.segments.filter((segment) => segment.kind === 'source');
			expect(sourceSegments).toHaveLength(lesson.sources.length);
		}
	});

	it('shows the same run commands in the chapter as the metadata generates', () => {
		for (const lesson of course.lessons) {
			const parsed = parsedChapter(lesson.chapterPath);
			const chapterLines = (parsed.run?.children ?? [])
				.flatMap((node) => (node.type === 'code' ? node.value.split('\n') : []))
				.map((line) => line.trim());
			for (const command of lesson.commands.run) {
				expect(chapterLines, `${lesson.chapterPath}: "${command}"`).toContain(command);
			}
		}
	});

	it('builds a table of contents whose ids exist in the page', () => {
		for (const lesson of course.lessons) {
			const html = lesson.segments.map((s) => (s.kind === 'html' ? s.html : '')).join('');
			const ids = lesson.toc.map((entry) => entry.id);
			expect(new Set(ids).size, lesson.slug).toBe(ids.length);
			for (const entry of lesson.toc) {
				// The run panel and the solutions section are rendered by the page.
				if (entry.id === 'how-to-compile-and-run' || entry.id === 'solutions') continue;
				expect(html, `${lesson.slug}#${entry.id}`).toContain(`id="${entry.id}"`);
			}
		}
	});

	it('links lessons in order with previous and next', () => {
		const lessons = course.lessons;
		expect(lessons[0].prev).toBeNull();
		expect(lessons.at(-1)?.next).toBeNull();
		for (let i = 1; i < lessons.length; i++) {
			expect(lessons[i].prev?.slug).toBe(lessons[i - 1].slug);
			expect(lessons[i - 1].next?.slug).toBe(lessons[i].slug);
		}
	});

	it('highlights code at build time and ships no raw markdown fences', () => {
		for (const lesson of course.lessons) {
			for (const segment of lesson.segments) {
				if (segment.kind !== 'html') continue;
				expect(segment.html).not.toContain('```');
				expect(segment.html).not.toContain('<code class="language-');
			}
		}
	});
});

describe('exercise solutions', () => {
	const exerciseDirs = readdirSync(path.join(root, 'exercises')).sort();

	it('loads every directory in exercises/ into the lesson of the same slug', () => {
		const withSolutions = course.lessons.filter((lesson) => lesson.solutions.length > 0);
		expect(withSolutions.map((lesson) => lesson.slug)).toEqual(exerciseDirs);
		for (const lesson of withSolutions) {
			const onDisk = readdirSync(path.join(root, 'exercises', lesson.slug)).sort();
			expect(lesson.solutions.map((solution) => solution.name)).toEqual(onDisk);
		}
	});

	it('orders the solutions by exercise number and links them to GitHub', () => {
		const lesson = course.lessons.find((candidate) => candidate.slug === '06-loops');
		expect(lesson?.solutions.map((solution) => solution.name)).toEqual([
			'1_multiplication_table.c',
			'2_running_total.c',
			'3_guessing_game.c'
		]);
		expect(lesson?.solutions[2].githubUrl).toBe(
			'https://github.com/dunamismax/c-from-the-ground-up/blob/main/exercises/06-loops/3_guessing_game.c'
		);
		expect(lesson?.solutions[2].contents).toBe(
			readFileSync(path.join(root, 'exercises/06-loops/3_guessing_game.c'), 'utf8')
		);
	});

	it('adds "Solutions" to the outline only where there are solutions', () => {
		for (const lesson of course.lessons) {
			const last = lesson.toc.at(-1);
			expect(last?.id === 'solutions', lesson.slug).toBe(lesson.solutions.length > 0);
		}
	});

	it('gives every listing on a lesson page its own anchor', () => {
		for (const lesson of course.lessons) {
			const ids = [...lesson.sources, ...lesson.solutions].map((source) => source.id);
			expect(new Set(ids).size, lesson.slug).toBe(ids.length);
		}
	});
});
