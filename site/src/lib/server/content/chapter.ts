// Turn one chapter file (src/chapters/NN-slug.md) into rendered segments.
//
// A chapter is: `# Title`, prose, one or more source listings (a
// `<!-- source: path -->` marker followed by a fenced block that
// scripts/sync_chapters.py keeps identical to the file), a
// `## How to Compile and Run` section, and sometimes more prose after it.
//
// The body is rendered to HTML once, so heading ids stay unique, and then split
// at each listing and at the run section. The lesson page renders those two with
// components (code block, compile panel) and the rest as plain HTML.

import { unified } from 'unified';
import remarkParse from 'remark-parse';
import remarkGfm from 'remark-gfm';
import remarkRehype from 'remark-rehype';
import rehypeSlug from 'rehype-slug';
import rehypeAutolinkHeadings from 'rehype-autolink-headings';
import { toHtml } from 'hast-util-to-html';
import { toString as hastToString } from 'hast-util-to-string';
import type { Highlighter } from 'shiki';
import type { Element, Root as HastRoot, RootContent as HastContent } from 'hast';
import type { Root as MdRoot, RootContent as MdContent, Paragraph } from 'mdast';
import type { SearchSection, Segment, TocEntry } from '#lib/content/types.js';
import { rehypeShiki } from './highlight.js';
import { rewriteLinks } from './page.js';

const MARKER_RE = /^<!-- source: (.+?) -->$/;
export const RUN_HEADING = 'How to Compile and Run';
export const RUN_ID = 'how-to-compile-and-run';

export interface Listing {
	/** Repository-relative path from the marker. */
	path: string;
	/** Fence language, for example `c` or `makefile`. */
	lang: string;
	/** The listing as it appears in the chapter (synced by `make sync`). */
	embedded: string;
}

export interface ParsedChapter {
	title: string;
	listings: Listing[];
	body: MdRoot;
	/** The run section without its heading, or null if the chapter has none. */
	run: MdRoot | null;
}

export interface RenderedChapter {
	segments: Segment[];
	toc: TocEntry[];
	/** The prose split at h2/h3 headings, as plain text, for the search index. */
	sections: SearchSection[];
	description: string;
	runNotesHtml: string;
}

function mdText(node: MdContent | MdRoot): string {
	if ('value' in node) return node.value;
	if ('children' in node) return node.children.map((child) => mdText(child)).join('');
	return '';
}

/** A node that remark-rehype turns into `<tag data-...>`; used as a split point. */
function placeholder(tag: string, properties: Record<string, string>): Paragraph {
	return { type: 'paragraph', children: [], data: { hName: tag, hProperties: properties } };
}

export function parseChapter(markdown: string, chapterPath: string): ParsedChapter {
	const tree = unified().use(remarkParse).use(remarkGfm).parse(markdown);
	const nodes = tree.children;
	const title = nodes[0]?.type === 'heading' && nodes[0].depth === 1 ? mdText(nodes[0]) : null;
	if (title === null) throw new Error(`${chapterPath}: must start with a # Title heading`);

	const listings: Listing[] = [];
	const body: MdContent[] = [];
	let run: MdRoot | null = null;

	for (let i = 1; i < nodes.length; i++) {
		const node = nodes[i];
		const marker = node.type === 'html' ? MARKER_RE.exec(node.value.trim()) : null;
		if (marker) {
			const code = nodes[i + 1];
			if (code?.type !== 'code') {
				throw new Error(`${chapterPath}: marker for ${marker[1]} is not followed by a code block`);
			}
			body.push(placeholder('source-listing', { dataIndex: String(listings.length) }));
			listings.push({ path: marker[1], lang: code.lang ?? '', embedded: code.value });
			i++;
			continue;
		}
		if (node.type === 'html') {
			throw new Error(`${chapterPath}: unexpected raw HTML: ${node.value}`);
		}
		if (node.type === 'heading' && node.depth === 2 && mdText(node) === RUN_HEADING) {
			if (run) throw new Error(`${chapterPath}: more than one "${RUN_HEADING}" section`);
			let end = i + 1;
			while (end < nodes.length) {
				const next = nodes[end];
				if (next.type === 'heading' && next.depth <= 2) break;
				end++;
			}
			run = { type: 'root', children: nodes.slice(i + 1, end) };
			body.push(placeholder('run-section', {}));
			i = end - 1;
			continue;
		}
		body.push(node);
	}

	return { title, listings, body: { type: 'root', children: body }, run };
}

function processor(highlighter: Highlighter) {
	return unified()
		.use(remarkRehype)
		.use(rehypeSlug)
		.use(rehypeAutolinkHeadings, {
			behavior: 'wrap',
			test: ['h2', 'h3', 'h4'],
			properties: { className: ['heading-anchor'] }
		})
		.use(rehypeShiki, { highlighter });
}

/**
 * The run section without its code blocks and the lead-in lines that
 * introduce them ("Build both programs:"). The page shows generated commands
 * instead; what is left is any extra advice, such as lesson 31's "Try it".
 */
function runNotes(run: MdRoot): MdRoot {
	const kept = run.children.filter((node, index) => {
		if (node.type === 'code') return false;
		const next = run.children[index + 1];
		if (node.type === 'paragraph' && next?.type === 'code' && mdText(node).trim().endsWith(':')) {
			return false;
		}
		return true;
	});
	return { type: 'root', children: kept };
}

export async function renderChapter(
	chapter: ParsedChapter,
	highlighter: Highlighter
): Promise<RenderedChapter> {
	const pipeline = processor(highlighter);
	const hast: HastRoot = await pipeline.run(chapter.body);
	// Links to the site's own address become paths (see siteHref).
	rewriteLinks(hast);

	const segments: Segment[] = [];
	const toc: TocEntry[] = [];
	let buffer: HastContent[] = [];
	let description = '';
	const sections: SearchSection[] = [];
	let section: SearchSection = { id: '', heading: '', text: '' };
	const startSection = (next: SearchSection) => {
		section.text = section.text.replace(/\s+/g, ' ').trim();
		if (section.text || section.heading) sections.push(section);
		section = next;
	};

	const flush = () => {
		const html = toHtml({ type: 'root', children: buffer }).trim();
		if (html) segments.push({ kind: 'html', html });
		buffer = [];
	};

	for (const node of hast.children) {
		const element: Element | null = node.type === 'element' ? node : null;
		if (element?.tagName === 'source-listing') {
			flush();
			segments.push({ kind: 'source', index: Number(element.properties.dataIndex) });
			continue;
		}
		if (element?.tagName === 'run-section') {
			flush();
			segments.push({ kind: 'run' });
			toc.push({ depth: 2, id: RUN_ID, text: RUN_HEADING });
			startSection({ id: RUN_ID, heading: RUN_HEADING, text: '' });
			continue;
		}
		if (element && (element.tagName === 'h2' || element.tagName === 'h3')) {
			const entry: TocEntry = {
				depth: element.tagName === 'h2' ? 2 : 3,
				id: String(element.properties.id),
				text: hastToString(element)
			};
			toc.push(entry);
			startSection({ id: entry.id, heading: entry.text, text: '' });
		} else if (element && element.tagName !== 'pre') {
			// Prose only: code examples would flood the index with C keywords.
			section.text += ` ${hastToString(element)}`;
		}
		if (!description && element?.tagName === 'p') {
			description = hastToString(element).replace(/\s+/g, ' ').trim();
		}
		buffer.push(node);
	}
	flush();
	startSection({ id: '', heading: '', text: '' });

	let runNotesHtml = '';
	if (chapter.run) {
		const notes: HastRoot = await pipeline.run(runNotes(chapter.run));
		rewriteLinks(notes);
		runNotesHtml = toHtml(notes).trim();
	}

	return { segments, toc, sections, description, runNotesHtml };
}
