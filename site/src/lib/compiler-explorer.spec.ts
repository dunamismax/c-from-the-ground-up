import { describe, expect, it, vi } from 'vitest';
import { clientState, COMPILER_ID, shortLink } from './compiler-explorer.js';

const lesson = {
	source: 'int main(void) { return 0; }\n',
	options: '-std=c11 -Wall',
	args: '5 + 3'
};

describe('Compiler Explorer', () => {
	it('builds a C session with a compiler and an executor', () => {
		const session = clientState(lesson).sessions[0];
		expect(session).toMatchObject({ language: 'c', source: lesson.source });
		expect(session.compilers).toEqual([{ id: COMPILER_ID, options: '-std=c11 -Wall' }]);
		expect(session.executors[0]).toMatchObject({
			compiler: { id: COMPILER_ID, options: '-std=c11 -Wall' },
			arguments: '5 + 3'
		});
	});

	it('posts the state to the shortener and returns the link', async () => {
		const fetcher = vi.fn<typeof fetch>(() =>
			Promise.resolve(Response.json({ url: 'https://godbolt.org/z/abc123' }))
		);
		await expect(shortLink(lesson, fetcher)).resolves.toBe('https://godbolt.org/z/abc123');
		const [url, init] = fetcher.mock.calls[0];
		expect(url).toBe('https://godbolt.org/api/shortener');
		expect(JSON.parse(init?.body as string)).toEqual(clientState(lesson));
	});

	it('rejects a failed or unexpected answer', async () => {
		await expect(
			shortLink(lesson, () => Promise.resolve(new Response('busy', { status: 503 })))
		).rejects.toThrow(/503/);
		await expect(
			shortLink(lesson, () => Promise.resolve(Response.json({ url: 'https://evil.example/' })))
		).rejects.toThrow(/did not return a link/);
	});
});
