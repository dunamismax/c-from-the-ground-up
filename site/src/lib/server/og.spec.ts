import { describe, expect, it } from 'vitest';
import { OG_HEIGHT, OG_WIDTH, renderOgImage } from './og.js';

describe('renderOgImage', () => {
	it('renders a 1200 x 630 PNG', async () => {
		const png = await renderOgImage({
			eyebrow: 'Part 1 · Lesson 7',
			title: 'Functions',
			description: 'Split a program into named, reusable pieces.'
		});
		const view = new DataView(png.buffer, png.byteOffset, png.byteLength);
		// PNG signature, then the IHDR chunk's width and height.
		expect([...png.slice(1, 4)].map((byte) => String.fromCharCode(byte)).join('')).toBe('PNG');
		expect(view.getUint32(16)).toBe(OG_WIDTH);
		expect(view.getUint32(20)).toBe(OG_HEIGHT);
	}, 20_000);
});
