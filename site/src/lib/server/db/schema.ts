// Drizzle schema for learner state (users, sessions, lesson progress, ...).
//
// Course content never lives in the database: it is read from the lesson and
// chapter files at build time. Tables arrive with the features that need them
// (accounts and `lesson_progress` in milestone W6).
//
// After changing this file, run `npm run db:generate` and commit the new
// migration under site/drizzle/.

export {};
