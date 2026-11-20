// Drizzle schema for learner state: accounts (Better Auth's tables) and lesson
// progress.
//
// Course content never lives in the database: it is read from the lesson and
// chapter files at build time.
//
// Everything is in schema `app`. In production site/deploy/provision.sh
// creates it, owned by cfromthegroundup_owner, and the web role may only use
// that schema; locally the first migration creates it.
//
// After changing this file, run `npm run db:generate` and commit the new
// migration under site/drizzle/.

import { boolean, index, pgSchema, primaryKey, text, timestamp } from 'drizzle-orm/pg-core';

export const app = pgSchema('app');

// --- Better Auth (https://www.better-auth.com/docs/concepts/database) --------
// Field names are the ones Better Auth expects; columns are snake_case.

export const user = app.table('user', {
	id: text('id').primaryKey(),
	name: text('name').notNull(),
	email: text('email').notNull().unique(),
	emailVerified: boolean('email_verified').default(false).notNull(),
	image: text('image'),
	createdAt: timestamp('created_at', { withTimezone: true }).defaultNow().notNull(),
	updatedAt: timestamp('updated_at', { withTimezone: true })
		.defaultNow()
		.$onUpdate(() => new Date())
		.notNull()
});

export const session = app.table(
	'session',
	{
		id: text('id').primaryKey(),
		expiresAt: timestamp('expires_at', { withTimezone: true }).notNull(),
		token: text('token').notNull().unique(),
		createdAt: timestamp('created_at', { withTimezone: true }).defaultNow().notNull(),
		updatedAt: timestamp('updated_at', { withTimezone: true })
			.$onUpdate(() => new Date())
			.notNull(),
		ipAddress: text('ip_address'),
		userAgent: text('user_agent'),
		userId: text('user_id')
			.notNull()
			.references(() => user.id, { onDelete: 'cascade' })
	},
	(table) => [index('session_user_id_idx').on(table.userId)]
);

export const account = app.table(
	'account',
	{
		id: text('id').primaryKey(),
		accountId: text('account_id').notNull(),
		providerId: text('provider_id').notNull(),
		userId: text('user_id')
			.notNull()
			.references(() => user.id, { onDelete: 'cascade' }),
		accessToken: text('access_token'),
		refreshToken: text('refresh_token'),
		idToken: text('id_token'),
		accessTokenExpiresAt: timestamp('access_token_expires_at', { withTimezone: true }),
		refreshTokenExpiresAt: timestamp('refresh_token_expires_at', { withTimezone: true }),
		scope: text('scope'),
		password: text('password'),
		createdAt: timestamp('created_at', { withTimezone: true }).defaultNow().notNull(),
		updatedAt: timestamp('updated_at', { withTimezone: true })
			.$onUpdate(() => new Date())
			.notNull()
	},
	(table) => [index('account_user_id_idx').on(table.userId)]
);

export const verification = app.table(
	'verification',
	{
		id: text('id').primaryKey(),
		identifier: text('identifier').notNull(),
		value: text('value').notNull(),
		expiresAt: timestamp('expires_at', { withTimezone: true }).notNull(),
		createdAt: timestamp('created_at', { withTimezone: true }).defaultNow().notNull(),
		updatedAt: timestamp('updated_at', { withTimezone: true })
			.defaultNow()
			.$onUpdate(() => new Date())
			.notNull()
	},
	(table) => [index('verification_identifier_idx').on(table.identifier)]
);

// --- Learner progress ---------------------------------------------------------

/**
 * One row per learner and lesson they have opened or finished. A lesson is
 * complete while `completed_at` is set; the most recent `last_viewed_at` is
 * "continue where you left off".
 */
export const lessonProgress = app.table(
	'lesson_progress',
	{
		userId: text('user_id')
			.notNull()
			.references(() => user.id, { onDelete: 'cascade' }),
		lessonSlug: text('lesson_slug').notNull(),
		completedAt: timestamp('completed_at', { withTimezone: true }),
		lastViewedAt: timestamp('last_viewed_at', { withTimezone: true })
	},
	(table) => [primaryKey({ columns: [table.userId, table.lessonSlug] })]
);
