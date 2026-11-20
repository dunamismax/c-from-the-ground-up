import { readdirSync, readFileSync } from 'node:fs';
import path from 'node:path';
import { describe, expect, it } from 'vitest';
import type { Library } from '#lib/content/types.js';
import { compileCommand, declaredLibraries, lessonCommands, lessonMeta } from './metadata.js';
import { courseRoot } from './paths.js';

const root = courseRoot();

/** Parse the case statement in compile_lesson() into [pattern, libraries] pairs. */
function smokeCheckLinkFlags(): [RegExp, Library[]][] {
	const script = readFileSync(path.join(root, 'scripts/smoke_check.sh'), 'utf8');
	const body = /compile_lesson\(\) \{([\s\S]*?)\n\}/.exec(script)?.[1];
	const cases = body && /case "\$lesson_path" in([\s\S]*?)esac/.exec(body)?.[1];
	if (!cases) throw new Error('cannot find the case statement in compile_lesson()');

	const FLAG_TO_LIBRARY: Record<string, Library> = {
		'-pthread': 'pthread',
		'-lm': 'm',
		$NCURSES_FLAG: 'ncurses'
	};
	const arms: [RegExp, Library[]][] = [];
	let patterns: RegExp[] = [];
	for (const raw of cases.split('\n')) {
		const line = raw.trim();
		const arm = /^([^\s=]+)\)$/.exec(line);
		if (arm) {
			patterns = arm[1].split('|').map((glob) => {
				const literal = glob.replace(/^\*/, '').replace(/[.]/g, '\\.');
				return new RegExp(`${literal}$`);
			});
			continue;
		}
		const flags = /^extra_flags="?([^"]*)"?$/.exec(line);
		if (flags) {
			const libraries = flags[1].split(/\s+/).map((flag) => {
				const library = FLAG_TO_LIBRARY[flag];
				if (!library) throw new Error(`smoke_check.sh: unknown link flag ${flag}`);
				return library;
			});
			for (const pattern of patterns) arms.push([pattern, libraries]);
		}
	}
	return arms;
}

function lessonFiles(): string[] {
	return readdirSync(root)
		.filter((name) => name.startsWith('Part '))
		.flatMap((part) =>
			readdirSync(path.join(root, part)).filter((name) => /^\d+_.*\.c$/.test(name))
		);
}

describe('lesson metadata', () => {
	it('agrees with the link flags in scripts/smoke_check.sh', () => {
		const arms = smokeCheckLinkFlags();
		expect(arms.length).toBeGreaterThan(0);
		const files = lessonFiles();
		expect(files.length).toBeGreaterThanOrEqual(35);
		for (const file of files) {
			const expected = arms.find(([pattern]) => pattern.test(file))?.[1] ?? [];
			const lesson = Number(/^(\d+)_/.exec(file)?.[1]);
			expect(declaredLibraries(lesson), file).toEqual(expected);
		}
	});

	it('generates compile and run commands', () => {
		const hello = lessonMeta(1, ['1_hello_world.c']);
		expect(lessonCommands(hello)).toEqual({
			build: ['cc -Wall -Wextra -std=c11 -o 1_hello_world 1_hello_world.c'],
			run: ['./1_hello_world']
		});
		expect(hello).toMatchObject({ platform: 'portable', interactive: false, needsArgs: false });

		const threads = lessonMeta(30, ['30_multithreaded_file_analyzer.c']);
		expect(lessonCommands(threads).build).toEqual([
			'cc -Wall -Wextra -std=c11 -o 30_multithreaded_file_analyzer 30_multithreaded_file_analyzer.c -pthread'
		]);
		expect(threads).toMatchObject({ platform: 'posix', needsArgs: true });

		const sockets = lessonMeta(26, ['26_simple_socket_client.c', '26_simple_socket_server.c']);
		expect(lessonCommands(sockets).run).toEqual([
			'./26_simple_socket_server 8888',
			'./26_simple_socket_client 127.0.0.1 8888 "Hello from the C client!"'
		]);

		const make = lessonMeta(31, ['helper.h', 'helper.c', 'main.c', 'Makefile']);
		expect(lessonCommands(make)).toEqual({ build: ['make'], run: ['./31_project_main'] });
	});

	it('puts link flags after the source file', () => {
		expect(compileCommand({ source: 'a.c', output: 'a', args: '' }, ['m', 'ncurses'])).toBe(
			'cc -Wall -Wextra -std=c11 -o a a.c -lm -lncurses'
		);
	});

	it('rejects a lesson with several C files and no program list', () => {
		expect(() => lessonMeta(99, ['a.c', 'b.c'])).toThrow(/list its programs/);
	});
});
