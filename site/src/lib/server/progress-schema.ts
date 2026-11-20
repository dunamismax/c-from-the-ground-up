// Request validation for the progress API (see #lib/progress/api.ts).

import * as v from 'valibot';
import type { ProgressOp as ProgressOpType } from '#lib/progress/api.js';
import type { ProgressSnapshot } from '#lib/progress/progress.js';

const slug = v.pipe(v.string(), v.regex(/^\d{2}-[a-z0-9-]+$/), v.maxLength(80));
const timestamp = v.pipe(v.string(), v.isoTimestamp());

const ProgressOp = v.variant('type', [
	v.object({ type: v.literal('complete'), slug }),
	v.object({ type: v.literal('incomplete'), slug }),
	v.object({ type: v.literal('view'), slug })
]);

// `satisfies` keeps the schema in step with the type the browser uses.
export const OpsBody = v.object({
	ops: v.pipe(v.array(ProgressOp), v.minLength(1), v.maxLength(50))
}) satisfies v.GenericSchema<unknown, { ops: ProgressOpType[] }>;

export const SnapshotBody = v.object({
	version: v.literal(1),
	completed: v.pipe(
		v.record(slug, timestamp),
		v.check((completed) => Object.keys(completed).length <= 200, 'too many lessons')
	),
	lastViewed: v.nullable(v.object({ slug, at: timestamp }))
}) satisfies v.GenericSchema<unknown, ProgressSnapshot>;
