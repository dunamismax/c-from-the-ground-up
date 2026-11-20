// Pull the comments out of a lesson source file, for the search index. In this
// course the comments are the lesson, so they are what learners search for.

/** The text of every comment in C source, without comment markers or banners. */
export function cComments(source: string): string {
	const parts: string[] = [];
	// Skip string and character literals so "//" or "/*" inside them is not a comment.
	const pattern = /"(?:\\.|[^"\\\n])*"|'(?:\\.|[^'\\\n])*'|\/\*([\s\S]*?)\*\/|\/\/([^\n]*)/g;
	for (const match of source.matchAll(pattern)) {
		const block = match[1] ?? match[2];
		if (block !== undefined) parts.push(block);
	}
	return clean(parts.join('\n').replace(/^[ \t]*\*+/gm, ''));
}

/** The text of every `#` comment in a Makefile. */
export function makefileComments(source: string): string {
	const lines = source.split('\n').filter((line) => line.trimStart().startsWith('#'));
	return clean(lines.map((line) => line.trimStart().replace(/^#+/, '')).join('\n'));
}

/**
 * Drop banner rules such as "=====" and "| - LESSON START - |" and the
 * header's @file / @brief / @author / @date tags, and collapse whitespace.
 */
function clean(text: string): string {
	return text
		.split('\n')
		.map((line) => line.replace(/[=|]{3,}|[|]/g, ' ').trim())
		.filter((line) => line && !line.startsWith('@') && !/^-\s*LESSON (START|END)\s*-$/.test(line))
		.join(' ')
		.replace(/\s+/g, ' ')
		.trim();
}
