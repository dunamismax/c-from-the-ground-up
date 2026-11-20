import { describe, expect, it } from 'vitest';
import { parseChapter, renderChapter } from './chapter.js';
import { getHighlighter } from './highlight.js';

const CHAPTER = [
	'# Example',
	'',
	'First paragraph with `code`.',
	'',
	'## Topic',
	'',
	'```sh',
	'echo hi',
	'```',
	'',
	'## Full Source',
	'',
	'<!-- source: Part 1/1_example.c -->',
	'```c',
	'int main(void) { return 0; }',
	'```',
	'',
	'## How to Compile and Run',
	'',
	'Build it:',
	'',
	'```sh',
	'cc example.c',
	'```',
	'',
	'Try changing it.',
	'',
	'## Key Takeaways',
	'',
	'- One'
].join('\n');

describe('parseChapter', () => {
	it('extracts the title, listings and run section', () => {
		const parsed = parseChapter(CHAPTER, 'example.md');
		expect(parsed.title).toBe('Example');
		expect(parsed.listings).toEqual([
			{ path: 'Part 1/1_example.c', lang: 'c', embedded: 'int main(void) { return 0; }' }
		]);
		expect(parsed.run?.children.map((node) => node.type)).toEqual([
			'paragraph',
			'code',
			'paragraph'
		]);
	});

	it('requires a title and a code block after each marker', () => {
		expect(() => parseChapter('Text', 'a.md')).toThrow(/# Title/);
		expect(() => parseChapter('# A\n\n<!-- source: x.c -->\n\nText', 'a.md')).toThrow(
			/not followed by a code block/
		);
	});
});

describe('renderChapter', () => {
	it('splits the body at listings and the run section, in order', async () => {
		const rendered = await renderChapter(
			parseChapter(CHAPTER, 'example.md'),
			await getHighlighter()
		);
		expect(rendered.segments.map((segment) => segment.kind)).toEqual([
			'html',
			'source',
			'run',
			'html'
		]);
		expect(rendered.toc).toEqual([
			{ depth: 2, id: 'topic', text: 'Topic' },
			{ depth: 2, id: 'full-source', text: 'Full Source' },
			{ depth: 2, id: 'how-to-compile-and-run', text: 'How to Compile and Run' },
			{ depth: 2, id: 'key-takeaways', text: 'Key Takeaways' }
		]);
		expect(rendered.description).toBe('First paragraph with code.');
		const first = rendered.segments[0];
		expect(first.kind === 'html' && first.html).toContain(
			'<h2 id="topic"><a class="heading-anchor" href="#topic">'
		);
		expect(first.kind === 'html' && first.html).toContain('class="shiki');
		expect(rendered.runNotesHtml).toBe('<p>Try changing it.</p>');
		expect(rendered.sections).toEqual([
			{ id: '', heading: '', text: 'First paragraph with code.' },
			{ id: 'topic', heading: 'Topic', text: '' },
			{ id: 'full-source', heading: 'Full Source', text: '' },
			{ id: 'how-to-compile-and-run', heading: 'How to Compile and Run', text: '' },
			{ id: 'key-takeaways', heading: 'Key Takeaways', text: 'One' }
		]);
	});
});
