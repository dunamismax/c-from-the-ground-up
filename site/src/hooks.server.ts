import type { Handle } from '@sveltejs/kit/hooks';
import { svelteKitHandler } from 'better-auth/svelte-kit';
import { building } from '$app/env';
import { getAuth, getSessionUser } from '#lib/server/auth.js';
import { RateLimiter } from '#lib/server/rate-limit.js';

// Prerendered pages never reach this hook in production (adapter-node serves
// them as files). It handles the dynamic part of the site: /api/ (accounts and
// synced progress, milestone W6) and /healthz.

// Every write to /api/, per client address, on top of Better Auth's own limits.
const writes = new RateLimiter({ max: 120, windowMs: 60_000 });

function noStore(response: Response): Response {
	if (response.headers.has('cache-control')) return response;
	try {
		response.headers.set('cache-control', 'private, no-store');
		return response;
	} catch {
		// Immutable headers: copy the response.
		const copy = new Response(response.body, response);
		copy.headers.set('cache-control', 'private, no-store');
		return copy;
	}
}

export const handle: Handle = async ({ event, resolve }) => {
	event.locals.user = null;
	const { pathname } = event.url;
	if (building || !pathname.startsWith('/api/')) return resolve(event);

	if (event.request.method !== 'GET' && event.request.method !== 'HEAD') {
		const limit = writes.take(event.getClientAddress());
		if (!limit.allowed) {
			return new Response(JSON.stringify({ message: 'Too many requests' }), {
				status: 429,
				headers: {
					'content-type': 'application/json',
					'cache-control': 'no-store',
					'retry-after': String(limit.retryAfterSeconds)
				}
			});
		}
	}

	const auth = getAuth();
	if (auth && pathname.startsWith('/api/auth/')) {
		return noStore(await svelteKitHandler({ event, resolve, auth, building }));
	}
	event.locals.user = await getSessionUser(event.request.headers);
	return noStore(await resolve(event));
};
