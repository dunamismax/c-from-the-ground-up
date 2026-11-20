import { describe, expect, it } from 'vitest';
import { cComments, makefileComments } from './comments.js';

describe('cComments', () => {
	it('collects block and line comments and drops the markers', () => {
		const source = [
			'/**',
			' * @brief Part 2, Lesson 13: Dynamic Memory Allocation',
			' * A lesson body.',
			' */',
			'int x = 1; // a line comment',
			'/* ==========',
			' * |   - LESSON START -   |',
			' * ========== */'
		].join('\n');
		expect(cComments(source)).toBe('A lesson body. a line comment');
	});

	it('ignores comment markers inside string and character literals', () => {
		expect(cComments('printf("// not a comment /* nor this */"); char c = \'/\'; // yes')).toBe(
			'yes'
		);
	});
});

describe('makefileComments', () => {
	it('collects # comments', () => {
		expect(makefileComments('# Build it\nCC = cc\n\t# recipe note\nall: main')).toBe(
			'Build it recipe note'
		);
	});
});
