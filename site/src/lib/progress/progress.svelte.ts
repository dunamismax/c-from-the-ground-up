// The reactive progress store the UI uses. It starts empty (the server and
// prerendered pages know nothing about the learner) and connects to a
// backend in the browser, from the root layout.

import {
	continueSlug,
	EMPTY_PROGRESS,
	markComplete,
	markIncomplete,
	progressFor,
	recordView,
	type ProgressBackend,
	type ProgressSnapshot
} from './progress.js';

class ProgressStore {
	#snapshot = $state<ProgressSnapshot>(EMPTY_PROGRESS);
	#backend: ProgressBackend | null = null;
	#unsubscribe: (() => void) | undefined;
	/** False until the browser's stored progress has been read. */
	ready = $state(false);

	connect(backend: ProgressBackend): () => void {
		this.#unsubscribe?.();
		this.#backend = backend;
		this.#snapshot = backend.read();
		this.#unsubscribe = backend.subscribe?.((snapshot) => (this.#snapshot = snapshot));
		this.ready = true;
		return () => this.#unsubscribe?.();
	}

	#save(next: ProgressSnapshot) {
		if (next === this.#snapshot) return;
		this.#snapshot = next;
		this.#backend?.write(next);
	}

	isComplete(slug: string): boolean {
		return Boolean(this.#snapshot.completed[slug]);
	}

	setComplete(slug: string, complete: boolean) {
		this.#save(
			complete
				? markComplete(this.#snapshot, slug, new Date())
				: markIncomplete(this.#snapshot, slug)
		);
	}

	view(slug: string) {
		this.#save(recordView(this.#snapshot, slug, new Date()));
	}

	progressFor(slugs: string[]) {
		return progressFor(this.#snapshot, slugs);
	}

	continueSlug(order: string[]) {
		return continueSlug(this.#snapshot, order);
	}

	get lastViewed() {
		return this.#snapshot.lastViewed;
	}
}

export const progress = new ProgressStore();
