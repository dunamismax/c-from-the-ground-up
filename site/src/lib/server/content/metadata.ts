// Per-lesson metadata that the chapter files do not state in a machine-readable
// way: link flags, platform, and how each program is run. The lesson page uses
// it for badges and for the generated compile and run commands.
//
// Keep `libraries` in step with compile_lesson() in scripts/smoke_check.sh;
// metadata.spec.ts fails when they disagree.

import type { Commands, LessonMeta, Library, Platform, Program } from '#lib/content/types.js';

/**
 * The C standard the compile commands teach: C17, settled by BUILD.md
 * decision D1. The lessons and chapters say the same; this is the one place
 * the site needs to follow.
 */
export const C_STANDARD = 'c17';

export const COMPILER = 'cc';
export const WARNING_FLAGS = ['-Wall', '-Wextra'];

/** The compiler and flags "Try it online" opens a lesson with in Compiler Explorer. */
export function onlineCompilerOptions(libraries: Library[]): string {
	return [
		`-std=${C_STANDARD}`,
		...WARNING_FLAGS,
		...libraries.map((library) => LINK_FLAGS[library])
	].join(' ');
}

export const LINK_FLAGS: Record<Library, string> = {
	pthread: '-pthread',
	m: '-lm',
	// Some systems name it ncursesw; the setup guide says so.
	ncurses: '-lncurses'
};

interface MetaEntry {
	platform?: Platform;
	libraries?: Library[];
	build?: 'single' | 'make';
	interactive?: boolean;
	/** Reads or writes files in its working folder. */
	usesFiles?: boolean;
	/** Example arguments for a single-program lesson. */
	args?: string;
	/** Explicit programs, in the order a learner should start them. */
	programs?: Program[];
}

/** Lessons not listed here are portable, interactive-free and argument-free. */
const TABLE: Record<number, MetaEntry> = {
	3: { interactive: true },
	5: { interactive: true },
	6: { interactive: true },
	13: { interactive: true },
	14: { usesFiles: true },
	15: { args: 'Bob 42' },
	16: { args: '5 + 3' },
	17: { interactive: true, usesFiles: true },
	25: { interactive: true, usesFiles: true, args: 'my_document.txt' },
	26: {
		platform: 'posix',
		programs: [
			{ source: '26_simple_socket_server.c', output: '26_simple_socket_server', args: '8888' },
			{
				source: '26_simple_socket_client.c',
				output: '26_simple_socket_client',
				args: '127.0.0.1 8888 "Hello from the C client!"'
			}
		]
	},
	27: { usesFiles: true, args: 'main 27_build_your_own_grep.c' },
	29: { platform: 'posix', interactive: true },
	30: {
		platform: 'posix',
		libraries: ['pthread'],
		usesFiles: true,
		args: '30_multithreaded_file_analyzer.c'
	},
	31: {
		build: 'make',
		programs: [{ source: 'Makefile', output: '31_project_main', args: '' }]
	},
	32: { libraries: ['m'] },
	33: { platform: 'ncurses', libraries: ['ncurses'], interactive: true },
	34: { usesFiles: true, args: 'users.dat' },
	35: {
		platform: 'ncurses',
		libraries: ['ncurses'],
		interactive: true,
		usesFiles: true,
		args: 'world.map'
	}
};

/** Every lesson's declared libraries, for the smoke-check agreement test. */
export function declaredLibraries(lesson: number): Library[] {
	return TABLE[lesson]?.libraries ?? [];
}

/**
 * Build the metadata for a lesson from the table and the names of its source
 * files (in chapter order).
 */
export function lessonMeta(lesson: number, sourceNames: string[]): LessonMeta {
	const entry = TABLE[lesson] ?? {};
	const build = entry.build ?? 'single';
	let programs = entry.programs;
	if (!programs) {
		const cFiles = sourceNames.filter((name) => name.endsWith('.c'));
		if (cFiles.length !== 1) {
			throw new Error(
				`lesson ${lesson}: ${cFiles.length} C files; list its programs in metadata.ts`
			);
		}
		programs = [
			{ source: cFiles[0], output: cFiles[0].replace(/\.c$/, ''), args: entry.args ?? '' }
		];
	}
	for (const program of programs) {
		if (!sourceNames.includes(program.source)) {
			throw new Error(`lesson ${lesson}: metadata names ${program.source}, not in the chapter`);
		}
	}
	const platform = entry.platform ?? 'portable';
	const usesFiles = entry.usesFiles ?? false;
	return {
		platform,
		libraries: entry.libraries ?? [],
		build,
		interactive: entry.interactive ?? false,
		needsArgs: programs.some((program) => program.args !== ''),
		usesFiles,
		online: platform === 'portable' && build === 'single' && programs.length === 1 && !usesFiles,
		programs
	};
}

export function compileCommand(program: Program, libraries: Library[]): string {
	return [
		COMPILER,
		...WARNING_FLAGS,
		`-std=${C_STANDARD}`,
		'-o',
		program.output,
		program.source,
		...libraries.map((library) => LINK_FLAGS[library])
	].join(' ');
}

export function runCommand(program: Program): string {
	return program.args ? `./${program.output} ${program.args}` : `./${program.output}`;
}

/** The commands to build and run a lesson, generated from its metadata. */
export function lessonCommands(meta: LessonMeta): Commands {
	const build =
		meta.build === 'make'
			? ['make']
			: meta.programs.map((program) => compileCommand(program, meta.libraries));
	return {
		build,
		run: meta.programs.map(runCommand),
		onlineOptions: meta.online ? onlineCompilerOptions(meta.libraries) : null
	};
}
