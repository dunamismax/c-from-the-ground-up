// "Try it online": open a lesson in Compiler Explorer (godbolt.org) with its
// source, flags and example arguments loaded and execution turned on.
//
// The browser fetches the lesson file from this site, then asks Compiler
// Explorer's public shortener for a link to that state. Nothing is sent until
// the learner clicks the button.

/** x86-64 GCC 16.2 on godbolt.org. Pinned so a lesson always opens the same way. */
export const COMPILER_ID = 'cg162';
export const COMPILER_NAME = 'GCC 16.2';
const SHORTENER = 'https://godbolt.org/api/shortener';

export interface OnlineLesson {
	source: string;
	options: string;
	args: string;
}

/** Compiler Explorer's "client state": one C editor with a compiler and an executor. */
export function clientState({ source, options, args }: OnlineLesson) {
	return {
		sessions: [
			{
				id: 1,
				language: 'c',
				source,
				compilers: [{ id: COMPILER_ID, options }],
				executors: [
					{
						compiler: { id: COMPILER_ID, options, libs: [] },
						arguments: args,
						stdin: '',
						compilerVisible: true,
						compilerOutputVisible: true,
						argumentsVisible: true,
						stdinVisible: true,
						wrap: false
					}
				]
			}
		]
	};
}

/** Create a Compiler Explorer short link for the lesson. */
export async function shortLink(
	lesson: OnlineLesson,
	fetcher: typeof fetch = fetch
): Promise<string> {
	const response = await fetcher(SHORTENER, {
		method: 'POST',
		headers: { 'content-type': 'application/json', accept: 'application/json' },
		body: JSON.stringify(clientState(lesson))
	});
	if (!response.ok) throw new Error(`Compiler Explorer answered ${response.status}`);
	const body: unknown = await response.json();
	const url = typeof body === 'object' && body !== null && 'url' in body ? body.url : undefined;
	if (typeof url !== 'string' || !url.startsWith('https://godbolt.org/')) {
		throw new Error('Compiler Explorer did not return a link');
	}
	return url;
}
