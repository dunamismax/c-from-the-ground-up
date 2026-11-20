import { json } from '@sveltejs/kit';
import type { RequestHandler } from './$types';

export const prerender = false;

// Reports the address the app sees for the caller (`getClientAddress()`), so
// the deploy can check the proxy settings end to end: behind Caddy it must be
// the visitor's own address, not Caddy's and not a spoofed X-Forwarded-For.
// Rate limiting (milestone W6) keys on the same value. It only ever tells a
// caller their own address.
export const GET: RequestHandler = ({ getClientAddress }) => {
	return json({ address: getClientAddress() }, { headers: { 'cache-control': 'no-store' } });
};
