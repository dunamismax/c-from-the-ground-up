/** Shorten `text` to at most `max` characters, on a word boundary, with an ellipsis. */
export function excerpt(text: string, max: number): string {
	const clean = text.replace(/\s+/g, ' ').trim();
	if (clean.length <= max) return clean;
	// Leave room for the ellipsis; a space just past the cut still counts as a
	// word boundary.
	const space = clean.slice(0, max).lastIndexOf(' ');
	const cut = space > max / 2 ? clean.slice(0, space) : clean.slice(0, max - 1);
	return `${cut.replace(/[\s,.;:!?-]+$/, '')}…`;
}
