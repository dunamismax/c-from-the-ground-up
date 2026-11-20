import { strFromU8, unzipSync } from 'fflate';
import { describe, expect, it } from 'vitest';
import { zipFiles } from './zip.js';

describe('zipFiles', () => {
	it('round-trips files and is reproducible', () => {
		const files = { 'course/b.c': 'int b;\n', 'course/a.h': '#pragma once\n' };
		const zip = zipFiles(files);
		const unpacked = unzipSync(zip);
		expect(Object.keys(unpacked).sort()).toEqual(['course/a.h', 'course/b.c']);
		expect(strFromU8(unpacked['course/b.c'])).toBe('int b;\n');
		expect(zipFiles(files)).toEqual(zip);
	});
});
