// Parse src/SUMMARY.md (the mdBook table of contents) into parts and chapters.
//
// The format is:
//
//   # Part 1: The Beginner Path -- Core Concepts
//
//   - [Hello, World!](chapters/01-hello-world.md)
//
// Links that appear before the first part heading (the introduction) are not
// lessons and are skipped.

export interface SummaryChapter {
	title: string;
	/** Path relative to src/, for example `chapters/01-hello-world.md`. */
	file: string;
	/** The chapter file name without `.md`, for example `01-hello-world`. */
	slug: string;
	number: number;
}

export interface SummaryPart {
	number: number;
	title: string;
	chapters: SummaryChapter[];
}

const PART_RE = /^# Part (\d+): (.+)$/;
const LINK_RE = /^- \[(.+)\]\((chapters\/((\d+)-[a-z0-9-]+)\.md)\)$/;

/** The book writes ASCII `--`; the site shows a real dash. */
export function displayTitle(title: string): string {
	return title.replace(/ -- /g, ' — ');
}

export function parseSummary(text: string): SummaryPart[] {
	const parts: SummaryPart[] = [];
	for (const raw of text.split('\n')) {
		const line = raw.trim();
		const part = PART_RE.exec(line);
		if (part) {
			parts.push({ number: Number(part[1]), title: displayTitle(part[2]), chapters: [] });
			continue;
		}
		const link = LINK_RE.exec(line);
		if (link) {
			const current = parts.at(-1);
			if (!current) continue;
			current.chapters.push({
				title: link[1],
				file: link[2],
				slug: link[3],
				number: Number(link[4])
			});
			continue;
		}
		if (line.startsWith('- [') && parts.length > 0) {
			throw new Error(`SUMMARY.md: cannot parse chapter link: ${line}`);
		}
	}
	return parts;
}

const PART_BLURB_RE = /^- \*\*Part (\d+): [^*]+\*\* -- (.+)$/;

/**
 * One-line descriptions of each part, from the "## Structure" list in
 * src/index.md (the book's introduction), for example:
 *
 *   - **Part 1: The Beginner Path** -- Core language fundamentals from Hello World
 *     through strings
 */
export function parsePartSummaries(indexMarkdown: string): Map<number, string> {
	const section = /^## Structure\n([\s\S]*?)(?=^## )/m.exec(indexMarkdown)?.[1] ?? '';
	// Join each bullet's continuation lines onto it.
	const bullets = section.replace(/\n {2,}/g, ' ').split('\n');
	const summaries = new Map<number, string>();
	for (const bullet of bullets) {
		const match = PART_BLURB_RE.exec(bullet.trim());
		if (match) summaries.set(Number(match[1]), match[2].trim());
	}
	return summaries;
}
