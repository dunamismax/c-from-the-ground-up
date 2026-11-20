// The progress API between the browser and the server (milestone W6):
//
//   GET  /api/progress            the signed-in learner's snapshot
//   POST /api/progress  { ops }   apply changes, in order
//   PUT  /api/progress  snapshot  merge a snapshot in (progress made in this
//                                 browser before signing in); returns the result
//
// Every call answers with the learner's resulting ProgressSnapshot.

import type { ProgressSnapshot } from './progress.js';

// Validation lives in #lib/server/progress-schema.ts, so the browser bundle
// carries no validator.
export type ProgressOp =
	| { type: 'complete'; slug: string }
	| { type: 'incomplete'; slug: string }
	| { type: 'view'; slug: string };

/** The changes that turn `before` into `after`, as API operations. */
export function diffOps(before: ProgressSnapshot, after: ProgressSnapshot): ProgressOp[] {
	const ops: ProgressOp[] = [];
	for (const slug of Object.keys(after.completed)) {
		if (!before.completed[slug]) ops.push({ type: 'complete', slug });
	}
	for (const slug of Object.keys(before.completed)) {
		if (!after.completed[slug]) ops.push({ type: 'incomplete', slug });
	}
	const viewed = after.lastViewed;
	if (viewed && (viewed.slug !== before.lastViewed?.slug || viewed.at !== before.lastViewed.at)) {
		ops.push({ type: 'view', slug: viewed.slug });
	}
	return ops;
}
