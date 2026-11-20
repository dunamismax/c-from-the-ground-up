/**
 * A fixed-window rate limiter kept in memory (the site is one Node process).
 * `take(key)` counts one request against `key` and says whether it is allowed.
 */
export class RateLimiter {
	readonly #max: number;
	readonly #windowMs: number;
	readonly #maxKeys: number;
	readonly #windows = new Map<string, { count: number; resetAt: number }>();

	constructor(options: { max: number; windowMs: number; maxKeys?: number }) {
		this.#max = options.max;
		this.#windowMs = options.windowMs;
		this.#maxKeys = options.maxKeys ?? 10_000;
	}

	take(key: string, now = Date.now()): { allowed: boolean; retryAfterSeconds: number } {
		let window = this.#windows.get(key);
		if (!window || window.resetAt <= now) {
			if (this.#windows.size >= this.#maxKeys) this.#prune(now);
			window = { count: 0, resetAt: now + this.#windowMs };
			this.#windows.set(key, window);
		}
		window.count++;
		return {
			allowed: window.count <= this.#max,
			retryAfterSeconds: Math.max(1, Math.ceil((window.resetAt - now) / 1000))
		};
	}

	/** Drop expired windows; if that is not enough, the oldest ones. */
	#prune(now: number) {
		for (const [key, window] of this.#windows) {
			if (window.resetAt <= now) this.#windows.delete(key);
		}
		for (const key of this.#windows.keys()) {
			if (this.#windows.size < this.#maxKeys) break;
			this.#windows.delete(key);
		}
	}
}
