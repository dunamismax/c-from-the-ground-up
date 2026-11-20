// Keyboard shortcut helpers. Shortcuts never fire while the learner is typing
// or working inside a widget that uses the keys itself.

/** True when a key press belongs to a text field or an editable element. */
export function isTypingTarget(target: EventTarget | null): boolean {
	if (!(target instanceof HTMLElement)) return false;
	if (target.isContentEditable) return true;
	return (
		['INPUT', 'TEXTAREA', 'SELECT'].includes(target.tagName) || target.closest('dialog') !== null
	);
}

/** True when arrow keys should scroll or move inside the focused element instead. */
export function ownsArrowKeys(target: EventTarget | null): boolean {
	if (!(target instanceof HTMLElement)) return false;
	return target.closest('pre, [role="tablist"], [role="listbox"]') !== null;
}

export type LessonShortcut = 'next' | 'prev' | null;

/** j / ArrowRight: next lesson; k / ArrowLeft: previous lesson. */
export function lessonShortcut(event: KeyboardEvent): LessonShortcut {
	if (event.defaultPrevented || event.ctrlKey || event.metaKey || event.altKey || event.shiftKey) {
		return null;
	}
	if (isTypingTarget(event.target)) return null;
	const arrow = event.key === 'ArrowRight' || event.key === 'ArrowLeft';
	if (arrow && ownsArrowKeys(event.target)) return null;
	if (event.key === 'j' || event.key === 'ArrowRight') return 'next';
	if (event.key === 'k' || event.key === 'ArrowLeft') return 'prev';
	return null;
}
