import { describe, expect, it } from 'vitest';
import { excerpt } from './text.js';

describe('excerpt', () => {
	it('keeps short text as it is, with whitespace collapsed', () => {
		expect(excerpt('  Hello,\n  world.  ', 50)).toBe('Hello, world.');
	});

	it('cuts long text on a word boundary and adds an ellipsis', () => {
		expect(excerpt('The quick brown fox jumps over the lazy dog', 20)).toBe('The quick brown fox…');
	});

	it('never returns more than max characters', () => {
		const text = 'word '.repeat(100);
		for (let max = 5; max < 60; max++) expect(excerpt(text, max).length).toBeLessThanOrEqual(max);
	});

	it('drops trailing punctuation before the ellipsis', () => {
		expect(excerpt('One, two, three, four, five', 17)).toBe('One, two, three…');
	});
});
