import { describe, expect, it } from 'vitest';
import { RateLimiter } from './rate-limit.js';

describe('RateLimiter', () => {
	it('allows up to max requests per window, per key', () => {
		const limiter = new RateLimiter({ max: 2, windowMs: 1000 });
		expect(limiter.take('a', 0).allowed).toBe(true);
		expect(limiter.take('a', 10).allowed).toBe(true);
		expect(limiter.take('a', 20)).toEqual({ allowed: false, retryAfterSeconds: 1 });
		expect(limiter.take('b', 20).allowed).toBe(true);
	});

	it('starts a new window once the old one has passed', () => {
		const limiter = new RateLimiter({ max: 1, windowMs: 60_000 });
		expect(limiter.take('a', 0).allowed).toBe(true);
		expect(limiter.take('a', 30_000)).toEqual({ allowed: false, retryAfterSeconds: 30 });
		expect(limiter.take('a', 60_000).allowed).toBe(true);
	});

	it('keeps memory bounded', () => {
		const limiter = new RateLimiter({ max: 1, windowMs: 1000, maxKeys: 3 });
		for (const key of ['a', 'b', 'c', 'd', 'e']) limiter.take(key, 0);
		// 'a' was dropped to make room, so it starts afresh.
		expect(limiter.take('a', 0).allowed).toBe(true);
		// 'e' is still counted.
		expect(limiter.take('e', 0).allowed).toBe(false);
	});
});
