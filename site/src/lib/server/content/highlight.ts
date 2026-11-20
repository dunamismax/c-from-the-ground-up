// Build-time syntax highlighting with Shiki. Pages get finished HTML with the
// token colors inline; no highlighting code reaches the browser. Code is shown
// dark in both site themes, so one theme is enough. The <pre> background comes
// from the site's own --c-code token (app.css), not from the theme.

import { createHighlighter, type Highlighter, type ShikiTransformer } from 'shiki';
import type { Element, Root } from 'hast';

// github-dark-default, not github-dark: its comments (#8b949e) meet WCAG AA
// contrast on the code background, and in this course the comments are the
// lesson.
export const THEME = 'github-dark-default';

const dropPreStyle: ShikiTransformer = {
	pre(node) {
		delete node.properties.style;
	}
};

const LANGUAGES = ['c', 'makefile', 'shellscript'] as const;

/** Fence languages used in the chapters, mapped to Shiki language ids. */
const ALIASES: Record<string, string> = {
	c: 'c',
	h: 'c',
	makefile: 'makefile',
	make: 'makefile',
	sh: 'shellscript',
	shell: 'shellscript',
	bash: 'shellscript',
	text: 'text',
	'': 'text'
};

let highlighter: Promise<Highlighter> | undefined;

export function getHighlighter(): Promise<Highlighter> {
	highlighter ??= createHighlighter({ themes: [THEME], langs: [...LANGUAGES] });
	return highlighter;
}

function shikiLanguage(lang: string): string {
	const resolved = ALIASES[lang.toLowerCase()];
	if (resolved === undefined) {
		throw new Error(`no highlighting for code fence language ${JSON.stringify(lang)}`);
	}
	return resolved;
}

/** Shiki renders a trailing newline as an empty last line; drop it. */
function trimFinalNewline(code: string): string {
	return code.endsWith('\n') ? code.slice(0, -1) : code;
}

/** Highlight `code` and return the `<pre class="shiki">` element. */
export function highlightToHast(hl: Highlighter, code: string, lang: string): Element {
	const language = shikiLanguage(lang);
	const root = hl.codeToHast(trimFinalNewline(code), {
		lang: language,
		theme: THEME,
		transformers: [dropPreStyle]
	});
	const pre = root.children.find(
		(node): node is Element => node.type === 'element' && node.tagName === 'pre'
	);
	if (!pre) throw new Error('Shiki returned no <pre> element');
	pre.properties.dataLanguage = lang || 'text';
	return pre;
}

export function highlightToHtml(hl: Highlighter, code: string, lang: string): string {
	return hl.codeToHtml(trimFinalNewline(code), {
		lang: shikiLanguage(lang),
		theme: THEME,
		transformers: [dropPreStyle]
	});
}

/** A rehype plugin that replaces every `<pre><code class="language-x">` with Shiki output. */
export function rehypeShiki(options: { highlighter: Highlighter }) {
	const visit = (node: Root | Element) => {
		node.children.forEach((child, index) => {
			if (child.type !== 'element') return;
			const code = child.tagName === 'pre' ? child.children[0] : undefined;
			if (code?.type === 'element' && code.tagName === 'code') {
				const classes = Array.isArray(code.properties.className) ? code.properties.className : [];
				const langClass = classes.map(String).find((name) => name.startsWith('language-'));
				const lang = langClass ? langClass.slice('language-'.length) : '';
				const text = code.children.map((part) => (part.type === 'text' ? part.value : '')).join('');
				node.children[index] = highlightToHast(options.highlighter, text, lang);
				return;
			}
			visit(child);
		});
	};
	return (tree: Root) => visit(tree);
}
