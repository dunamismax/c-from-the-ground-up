import { eq } from 'drizzle-orm';
import { noStoreJson, requireUser } from '#lib/server/api.js';
import { getDb } from '#lib/server/db/index.js';
import { account, lessonProgress, session, user } from '#lib/server/db/schema.js';
import type { RequestHandler } from './$types';

// Everything stored about the signed-in learner, as a JSON download. Secrets
// (session tokens and the encrypted GitHub token) are left out.
export const prerender = false;

export const GET: RequestHandler = async ({ locals }) => {
	const { id } = requireUser(locals);
	const db = getDb();
	const [profile] = await db
		.select({
			id: user.id,
			name: user.name,
			email: user.email,
			createdAt: user.createdAt,
			updatedAt: user.updatedAt
		})
		.from(user)
		.where(eq(user.id, id));
	const linked = await db
		.select({
			provider: account.providerId,
			accountId: account.accountId,
			scope: account.scope,
			createdAt: account.createdAt
		})
		.from(account)
		.where(eq(account.userId, id));
	const sessions = await db
		.select({
			createdAt: session.createdAt,
			expiresAt: session.expiresAt,
			ipAddress: session.ipAddress,
			userAgent: session.userAgent
		})
		.from(session)
		.where(eq(session.userId, id));
	const progress = await db
		.select({
			lesson: lessonProgress.lessonSlug,
			completedAt: lessonProgress.completedAt,
			lastViewedAt: lessonProgress.lastViewedAt
		})
		.from(lessonProgress)
		.where(eq(lessonProgress.userId, id))
		.orderBy(lessonProgress.lessonSlug);
	return noStoreJson(
		{ exportedAt: new Date().toISOString(), user: profile, accounts: linked, sessions, progress },
		{ headers: { 'content-disposition': 'attachment; filename="cfromthegroundup-account.json"' } }
	);
};
