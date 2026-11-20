// Learner progress: which lessons are complete and which was viewed last.
//
// The data is a small immutable snapshot. A ProgressBackend stores it: today
// the browser's localStorage (localStorageBackend below); in milestone W6 an
// account in the database can implement the same interface.

export interface ProgressSnapshot {
	version: 1;
	/** Lesson slug -> when it was marked complete (ISO 8601). */
	completed: Record<string, string>;
	lastViewed: { slug: string; at: string } | null;
}

export interface ProgressBackend {
	read(): ProgressSnapshot;
	write(snapshot: ProgressSnapshot): void;
	/** Call `listener` when the stored progress changes elsewhere (another tab). */
	subscribe?(listener: (snapshot: ProgressSnapshot) => void): () => void;
}

export const EMPTY_PROGRESS: ProgressSnapshot = Object.freeze({
	version: 1,
	completed: Object.freeze({}) as Record<string, string>,
	lastViewed: null
});

const SLUG_RE = /^\d{2}-[a-z0-9-]+$/;

function isTimestamp(value: unknown): value is string {
	return typeof value === 'string' && !Number.isNaN(Date.parse(value));
}

/** Read a stored snapshot, dropping anything malformed instead of failing. */
export function parseSnapshot(raw: string | null): ProgressSnapshot {
	if (!raw) return EMPTY_PROGRESS;
	let data: unknown;
	try {
		data = JSON.parse(raw);
	} catch {
		return EMPTY_PROGRESS;
	}
	if (typeof data !== 'object' || data === null || !('version' in data) || data.version !== 1) {
		return EMPTY_PROGRESS;
	}
	const completed: Record<string, string> = {};
	if ('completed' in data && typeof data.completed === 'object' && data.completed !== null) {
		for (const [slug, at] of Object.entries(data.completed)) {
			if (SLUG_RE.test(slug) && isTimestamp(at)) completed[slug] = at;
		}
	}
	let lastViewed: ProgressSnapshot['lastViewed'] = null;
	if ('lastViewed' in data && typeof data.lastViewed === 'object' && data.lastViewed !== null) {
		const { slug, at } = data.lastViewed as Record<string, unknown>;
		if (typeof slug === 'string' && SLUG_RE.test(slug) && isTimestamp(at))
			lastViewed = { slug, at };
	}
	return { version: 1, completed, lastViewed };
}

export function markComplete(
	snapshot: ProgressSnapshot,
	slug: string,
	now: Date
): ProgressSnapshot {
	if (snapshot.completed[slug]) return snapshot;
	return { ...snapshot, completed: { ...snapshot.completed, [slug]: now.toISOString() } };
}

export function markIncomplete(snapshot: ProgressSnapshot, slug: string): ProgressSnapshot {
	if (!snapshot.completed[slug]) return snapshot;
	const completed = { ...snapshot.completed };
	delete completed[slug];
	return { ...snapshot, completed };
}

export function recordView(snapshot: ProgressSnapshot, slug: string, now: Date): ProgressSnapshot {
	return { ...snapshot, lastViewed: { slug, at: now.toISOString() } };
}

export interface PartProgress {
	done: number;
	total: number;
	/** Whole percent, 0 to 100. */
	percent: number;
}

export function progressFor(snapshot: ProgressSnapshot, slugs: string[]): PartProgress {
	const done = slugs.filter((slug) => snapshot.completed[slug]).length;
	const total = slugs.length;
	return { done, total, percent: total === 0 ? 0 : Math.round((done / total) * 100) };
}

/**
 * Where to send a returning learner: the lesson they viewed last, unless they
 * finished it, in which case the first lesson after it they have not finished.
 */
export function continueSlug(snapshot: ProgressSnapshot, order: string[]): string | null {
	const last = snapshot.lastViewed?.slug;
	if (!last) return null;
	const start = order.indexOf(last);
	if (start === -1) return null;
	if (!snapshot.completed[last]) return last;
	return order.slice(start + 1).find((slug) => !snapshot.completed[slug]) ?? null;
}

export const STORAGE_KEY = 'cftgu-progress';

/** Progress in the browser's localStorage, synced across tabs. */
export function localStorageBackend(
	storage: Storage,
	target: Window | null = null
): ProgressBackend {
	return {
		read() {
			try {
				return parseSnapshot(storage.getItem(STORAGE_KEY));
			} catch {
				return EMPTY_PROGRESS;
			}
		},
		write(snapshot) {
			try {
				storage.setItem(STORAGE_KEY, JSON.stringify(snapshot));
			} catch {
				// Full or blocked storage: progress lasts until the tab closes.
			}
		},
		subscribe(listener) {
			if (!target) return () => {};
			const onStorage = (event: StorageEvent) => {
				if (event.key === STORAGE_KEY) listener(parseSnapshot(event.newValue));
			};
			target.addEventListener('storage', onStorage);
			return () => target.removeEventListener('storage', onStorage);
		}
	};
}
