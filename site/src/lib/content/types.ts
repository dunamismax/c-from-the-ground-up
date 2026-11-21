// Types for course content. The loader in #lib/server/content/ builds these at
// build time from src/SUMMARY.md, src/chapters/*.md and the lesson sources;
// pages and components only ever see the plain, serializable objects below.

/** Portable ISO C, POSIX APIs (Unix-like systems or WSL), or ncurses. */
export type Platform = 'portable' | 'posix' | 'ncurses';

/** Libraries a lesson links against, beyond the C standard library. */
export type Library = 'pthread' | 'm' | 'ncurses';

/** One program a lesson builds and runs. Lesson 26 has two. */
export interface Program {
	/** Source file name, for example `1_hello_world.c`. */
	source: string;
	/** Executable name, for example `1_hello_world`. */
	output: string;
	/** Example command-line arguments, already shell-quoted, or '' for none. */
	args: string;
}

export interface LessonMeta {
	platform: Platform;
	libraries: Library[];
	/** `single`: one `cc` command per program; `make`: the lesson ships a Makefile. */
	build: 'single' | 'make';
	/** The program reads from the keyboard (stdin) while it runs. */
	interactive: boolean;
	/** The program needs command-line arguments. */
	needsArgs: boolean;
	/** The program reads or writes files in its working folder. */
	usesFiles: boolean;
	/**
	 * The lesson can be compiled and run in Compiler Explorer: portable C,
	 * one file, and no files to read or write.
	 */
	online: boolean;
	programs: Program[];
}

/** Shell commands generated from the metadata (never copied from prose). */
export interface Commands {
	build: string[];
	run: string[];
	/** Compiler flags for Compiler Explorer, or null when the lesson cannot run there. */
	onlineOptions: string | null;
}

export interface SourceFile {
	/** Repository-relative path, for example `Part 1 - .../1_hello_world.c`. */
	path: string;
	/** File name, for example `1_hello_world.c`. */
	name: string;
	/** Anchor id of the rendered listing on the lesson page. */
	id: string;
	language: 'c' | 'makefile';
	contents: string;
	lineCount: number;
	/** Shiki-highlighted HTML (github-dark-default). */
	html: string;
	/** The file on GitHub. */
	githubUrl: string;
	/** The raw file, for downloading. */
	rawUrl: string;
}

/** One part of a chapter's prose, between headings, for the search index. */
export interface SearchSection {
	/** Heading anchor id, or '' for the text before the first heading. */
	id: string;
	heading: string;
	text: string;
}

export interface TocEntry {
	depth: 2 | 3;
	id: string;
	text: string;
}

/**
 * The chapter body in order, split so that source listings and the compile
 * and run section can be rendered by components.
 */
export type Segment =
	{ kind: 'html'; html: string } | { kind: 'source'; index: number } | { kind: 'run' };

export interface LessonRef {
	number: number;
	slug: string;
	title: string;
}

export interface PartRef {
	number: number;
	title: string;
}

export interface Lesson extends LessonRef {
	part: PartRef;
	/** Repository-relative path of the chapter, for example `src/chapters/01-hello-world.md`. */
	chapterPath: string;
	/** Plain text of the first prose paragraph, for cards and meta descriptions. */
	description: string;
	segments: Segment[];
	toc: TocEntry[];
	/** Plain-text prose by section, for the search index; not sent to pages. */
	searchSections: SearchSection[];
	sources: SourceFile[];
	/**
	 * Worked solutions to the chapter's "Try It Yourself" exercises, in exercise
	 * order, from `exercises/<slug>/` in the repository. Empty for a lesson
	 * that has none yet.
	 */
	solutions: SourceFile[];
	/**
	 * Extra advice from the chapter's "How to Compile and Run" section, rendered,
	 * with its commands removed (the page shows `commands` instead). Often ''.
	 */
	runNotesHtml: string;
	meta: LessonMeta;
	commands: Commands;
	prev: LessonRef | null;
	next: LessonRef | null;
}

export interface Part extends PartRef {
	/** One line on what the part covers, from the book's introduction. */
	summary: string;
	lessons: LessonRef[];
}

export interface Course {
	parts: Part[];
	lessons: Lesson[];
}
