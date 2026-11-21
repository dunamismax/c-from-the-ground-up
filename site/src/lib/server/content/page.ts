// Render a standalone Markdown page of the book (src/glossary.md) for the site,
// with the same pipeline as the chapters: GitHub-flavoured Markdown, heading
// ids and anchor links. Links to chapters (`chapters/NN-slug.md`) become links
// to the lesson pages (`/lessons/NN-slug`), so one file serves both editions.
// Like the rest of the content loader, it runs only at build time.

import { readFile } from 'node:fs/promises';
import path from 'node:path';
import { unified } from 'unified';
import remarkParse from 'remark-parse';
import remarkGfm from 'remark-gfm';
import remarkRehype from 'remark-rehype';
import rehypeSlug from 'rehype-slug';
import rehypeAutolinkHeadings from 'rehype-autolink-headings';
import { toHtml } from 'hast-util-to-html';
import { toString as hastToString } from 'hast-util-to-string';
import type { Element, Root as HastRoot, RootContent as HastContent } from 'hast';
import type { TocEntry } from '#lib/content/types.js';
import { SITE_URL } from '#lib/site.js';
import { courseRoot } from './paths.js';

export interface MarkdownPage {
	title: string;
	/** The page body without its `# Title`, which the page renders itself. */
	html: string;
	/** The `##` headings, in order. */
	toc: TocEntry[];
	/** The first paragraph, as plain text. */
	description: string;
}

const CHAPTER_LINK_RE = /^chapters\/(\d+-[a-z0-9-]+)\.md(#.*)?$/;

/**
 * `chapters/01-hello-world.md#x` -> `/lessons/01-hello-world#x`, and a link
 * to the site's own address -> its path, so a chapter can point the book's
 * readers at the website without sending the website's readers away from the
 * page they are on. Other links are unchanged.
 */
export function siteHref(href: string): string {
	const match = CHAPTER_LINK_RE.exec(href);
	if (match) return `/lessons/${match[1]}${match[2] ?? ''}`;
	if (href === SITE_URL) return '/';
	if (href.startsWith(`${SITE_URL}/`)) return href.slice(SITE_URL.length);
	return href;
}

export function rewriteLinks(node: HastRoot | HastContent): void {
	if (node.type === 'element' && node.tagName === 'a' && typeof node.properties.href === 'string') {
		node.properties.href = siteHref(node.properties.href);
	}
	if ('children' in node) {
		for (const child of node.children) rewriteLinks(child);
	}
}

export async function renderMarkdownPage(markdown: string, file: string): Promise<MarkdownPage> {
	const pipeline = unified()
		.use(remarkParse)
		.use(remarkGfm)
		.use(remarkRehype)
		.use(rehypeSlug)
		.use(rehypeAutolinkHeadings, {
			behavior: 'wrap',
			test: ['h2', 'h3', 'h4'],
			properties: { className: ['heading-anchor'] }
		});
	const hast: HastRoot = await pipeline.run(pipeline.parse(markdown));
	rewriteLinks(hast);

	const elements = hast.children.filter((node): node is Element => node.type === 'element');
	const heading = elements[0];
	if (heading?.tagName !== 'h1') throw new Error(`${file}: must start with a # Title heading`);

	const toc: TocEntry[] = elements
		.filter((element) => element.tagName === 'h2')
		.map((element) => ({
			depth: 2,
			id: String(element.properties.id),
			text: hastToString(element)
		}));
	const paragraph = elements.find((element) => element.tagName === 'p');
	const body = hast.children.filter((node) => node !== heading);

	return {
		title: hastToString(heading),
		html: toHtml({ type: 'root', children: body }).trim(),
		toc,
		description: paragraph ? hastToString(paragraph).replace(/\s+/g, ' ').trim() : ''
	};
}

/** Load and render `src/<name>` from the course repository. */
export async function loadMarkdownPage(
	name: string,
	root: string = courseRoot()
): Promise<MarkdownPage> {
	const file = `src/${name}`;
	return renderMarkdownPage(await readFile(path.join(root, file), 'utf8'), file);
}
