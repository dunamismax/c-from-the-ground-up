// Self-hosted Umami analytics (BUILD.md W9, decision D9).
//
// The tracker is on only when UMAMI_WEBSITE_ID is set. The ID is not a secret
// (it appears in every page's HTML), and pages are prerendered, so it lives in
// the code rather than in the server's environment: set it, commit and deploy.
// Umami runs on the same host; Caddy passes /stats/script.js and
// /stats/api/send to it, so the browser talks only to this site's own origin
// and the CSP needs no new source. See site/deploy/README.md, "Analytics".

/** The Umami website ID for cfromthegroundup.com, or '' to leave analytics off. */
export const UMAMI_WEBSITE_ID = '';

/** Where Caddy serves the tracker. It posts to /stats/api/send, next to it. */
export const UMAMI_SCRIPT_PATH = '/stats/script.js';

/** Only count visits to the real site, never local builds or test runs. */
export const UMAMI_DOMAINS = 'cfromthegroundup.com';

/** The tracker <script> tag's attributes, name to value. */
export type TrackerAttributes = Readonly<
	Record<'src' | 'data-website-id' | 'data-domains' | 'data-do-not-track', string>
>;

/** The attributes of the tracker's <script> tag, or null when analytics is off. */
export function trackerAttributes(websiteId: string = UMAMI_WEBSITE_ID): TrackerAttributes | null {
	const id = websiteId.trim();
	if (id === '') return null;
	if (!/^[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}$/i.test(id)) {
		throw new Error(`UMAMI_WEBSITE_ID must be a UUID, not "${id}"`);
	}
	return {
		src: UMAMI_SCRIPT_PATH,
		'data-website-id': id,
		'data-domains': UMAMI_DOMAINS,
		'data-do-not-track': 'true'
	};
}
