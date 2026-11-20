// Learner progress in the database (table lesson_progress, see db/schema.ts).

import { and, eq, sql } from 'drizzle-orm';
import type { ProgressOp } from '#lib/progress/api.js';
import type { ProgressSnapshot } from '#lib/progress/progress.js';
import type { Database } from './db/index.js';
import { lessonProgress } from './db/schema.js';

export interface ProgressRow {
	lessonSlug: string;
	completedAt: Date | null;
	lastViewedAt: Date | null;
}

/** Rows to the snapshot the browser uses. */
export function snapshotFromRows(rows: ProgressRow[]): ProgressSnapshot {
	const completed: Record<string, string> = {};
	let lastViewed: ProgressSnapshot['lastViewed'] = null;
	for (const row of rows) {
		if (row.completedAt) completed[row.lessonSlug] = row.completedAt.toISOString();
		if (row.lastViewedAt && (!lastViewed || row.lastViewedAt.toISOString() > lastViewed.at)) {
			lastViewed = { slug: row.lessonSlug, at: row.lastViewedAt.toISOString() };
		}
	}
	return { version: 1, completed, lastViewed };
}

/**
 * A timestamp the browser sent, kept only if it is plausible: never in the
 * future and not before the site existed. Otherwise `now`.
 */
export function clampTimestamp(value: string, now: Date): Date {
	const time = Date.parse(value);
	if (Number.isNaN(time) || time > now.getTime() || time < Date.UTC(2025, 0, 1)) return now;
	return new Date(time);
}

export async function readProgress(db: Database, userId: string): Promise<ProgressSnapshot> {
	const rows = await db
		.select({
			lessonSlug: lessonProgress.lessonSlug,
			completedAt: lessonProgress.completedAt,
			lastViewedAt: lessonProgress.lastViewedAt
		})
		.from(lessonProgress)
		.where(eq(lessonProgress.userId, userId));
	return snapshotFromRows(rows);
}

export async function applyOps(
	db: Database,
	userId: string,
	ops: ProgressOp[],
	now = new Date()
): Promise<ProgressSnapshot> {
	await db.transaction(async (tx) => {
		for (const op of ops) {
			if (op.type === 'incomplete') {
				await tx
					.update(lessonProgress)
					.set({ completedAt: null })
					.where(and(eq(lessonProgress.userId, userId), eq(lessonProgress.lessonSlug, op.slug)));
			} else if (op.type === 'complete') {
				await tx
					.insert(lessonProgress)
					.values({ userId, lessonSlug: op.slug, completedAt: now })
					.onConflictDoUpdate({
						target: [lessonProgress.userId, lessonProgress.lessonSlug],
						// Completing twice keeps the first date.
						set: {
							completedAt: sql`coalesce(${lessonProgress.completedAt}, excluded.completed_at)`
						}
					});
			} else {
				await tx
					.insert(lessonProgress)
					.values({ userId, lessonSlug: op.slug, lastViewedAt: now })
					.onConflictDoUpdate({
						target: [lessonProgress.userId, lessonProgress.lessonSlug],
						set: { lastViewedAt: sql`excluded.last_viewed_at` }
					});
			}
		}
	});
	return readProgress(db, userId);
}

/**
 * Merge a snapshot from the browser into the account: a lesson is complete if
 * either side says so (keeping the earlier date), and the later "last viewed"
 * wins. Nothing the account already has is removed.
 */
export async function mergeProgress(
	db: Database,
	userId: string,
	snapshot: ProgressSnapshot,
	now = new Date()
): Promise<ProgressSnapshot> {
	const rows = new Map<string, { completedAt: Date | null; lastViewedAt: Date | null }>();
	for (const [slug, at] of Object.entries(snapshot.completed)) {
		rows.set(slug, { completedAt: clampTimestamp(at, now), lastViewedAt: null });
	}
	if (snapshot.lastViewed) {
		const { slug, at } = snapshot.lastViewed;
		const row = rows.get(slug) ?? { completedAt: null, lastViewedAt: null };
		rows.set(slug, { ...row, lastViewedAt: clampTimestamp(at, now) });
	}
	if (rows.size > 0) {
		await db
			.insert(lessonProgress)
			.values([...rows].map(([lessonSlug, row]) => ({ userId, lessonSlug, ...row })))
			.onConflictDoUpdate({
				target: [lessonProgress.userId, lessonProgress.lessonSlug],
				// least() and greatest() ignore NULLs in PostgreSQL.
				set: {
					completedAt: sql`least(${lessonProgress.completedAt}, excluded.completed_at)`,
					lastViewedAt: sql`greatest(${lessonProgress.lastViewedAt}, excluded.last_viewed_at)`
				}
			});
	}
	return readProgress(db, userId);
}
