import { describe, expect, it } from 'vitest';
import { displayTitle, parsePartSummaries, parseSummary } from './summary.js';

describe('parseSummary', () => {
	it('skips prefix chapters and everything after a --- separator', () => {
		const parts = parseSummary(
			[
				'- [Introduction](index.md)',
				'- [Setting Up](setup.md)',
				'',
				'# Part 1: One',
				'- [Hello, World!](chapters/01-hello-world.md)',
				'',
				'---',
				'',
				'- [Glossary](glossary.md)'
			].join('\n')
		);
		expect(parts.map((part) => part.chapters.map((chapter) => chapter.slug))).toEqual([
			['01-hello-world']
		]);
	});

	it('parses parts and chapters and skips the introduction', () => {
		const parts = parseSummary(
			[
				'# Summary',
				'',
				'- [Introduction](index.md)',
				'',
				'# Part 1: The Beginner Path -- Core Concepts',
				'',
				'- [Hello, World!](chapters/01-hello-world.md)',
				'- [Loops](chapters/06-loops.md)',
				'',
				'# Part 2: Next',
				'- [Pointers](chapters/10-pointers.md)'
			].join('\n')
		);
		expect(parts).toEqual([
			{
				number: 1,
				title: 'The Beginner Path — Core Concepts',
				chapters: [
					{
						title: 'Hello, World!',
						file: 'chapters/01-hello-world.md',
						slug: '01-hello-world',
						number: 1
					},
					{ title: 'Loops', file: 'chapters/06-loops.md', slug: '06-loops', number: 6 }
				]
			},
			{
				number: 2,
				title: 'Next',
				chapters: [
					{ title: 'Pointers', file: 'chapters/10-pointers.md', slug: '10-pointers', number: 10 }
				]
			}
		]);
	});

	it('rejects a chapter link it cannot parse', () => {
		expect(() => parseSummary('# Part 1: A\n- [Bad](chapters/bad.md)')).toThrow(/cannot parse/);
	});

	it('turns the ASCII double dash into an em dash', () => {
		expect(displayTitle('A -- B')).toBe('A — B');
	});
});

describe('parsePartSummaries', () => {
	it('reads the part descriptions from the Structure list', () => {
		const summaries = parsePartSummaries(
			[
				'# Intro',
				'',
				'## Structure',
				'',
				'- **Part 1: The Beginner Path** -- Core language fundamentals from Hello World',
				'  through strings',
				'- **Part 2: Next** -- Pointers',
				'',
				'## Quick Start',
				'- **Part 3: Ignored** -- not in the section'
			].join('\n')
		);
		expect([...summaries]).toEqual([
			[1, 'Core language fundamentals from Hello World through strings'],
			[2, 'Pointers']
		]);
	});
});
