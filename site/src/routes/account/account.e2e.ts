import { readFileSync } from 'node:fs';
import AxeBuilder from '@axe-core/playwright';
import { expect, test, type Page } from '@playwright/test';
import { createLearner, E2E_ACCOUNTS_ENV, signIn } from '../../../e2e/accounts.js';

// Accounts and synced progress (milestone W6). The test server has accounts on
// with a made-up GitHub app; tests sign in by creating sessions directly.

async function open(page: Page, path: string) {
	await page.goto(path);
	await page.locator('html[data-hydrated]').waitFor({ state: 'attached' });
}

/** Open a lesson and wait until the account (if any) has synced progress. */
async function openLesson(page: Page, slug: string) {
	const synced = page.waitForResponse(
		(response) => response.url().endsWith('/api/account') && response.ok()
	);
	await open(page, `/lessons/${slug}`);
	await synced;
}

/** Click a progress button and wait for the account to store the change. */
async function clickAndSave(page: Page, name: string) {
	const saved = page.waitForResponse(
		(response) =>
			response.url().endsWith('/api/progress') &&
			response.request().method() === 'POST' &&
			response.ok()
	);
	await page.getByRole('button', { name, exact: true }).click();
	await saved;
}

async function completedOnServer(page: Page): Promise<string[]> {
	const response = await page.request.get('/api/progress');
	expect(response.status()).toBe(200);
	const snapshot = (await response.json()) as { completed: Record<string, string> };
	return Object.keys(snapshot.completed).sort();
}

test('signed out, the account page offers GitHub sign-in', async ({ page }) => {
	// Never leave for the real GitHub.
	await page.route('https://github.com/**', (route) =>
		route.fulfill({ status: 200, contentType: 'text/html', body: '<title>GitHub stub</title>' })
	);
	await open(page, '/');
	await page
		.getByRole('navigation', { name: 'Main', exact: true })
		.getByRole('link', { name: 'Sign in' })
		.click();
	await expect(page).toHaveURL('/account');
	await expect(page.getByRole('heading', { level: 1 })).toHaveText('Your account');

	const leaving = page.waitForRequest((request) => request.url().startsWith('https://github.com/'));
	await page.getByRole('button', { name: 'Sign in with GitHub' }).click();
	const authorize = new URL((await leaving).url());
	expect(authorize.pathname).toBe('/login/oauth/authorize');
	expect(authorize.searchParams.get('client_id')).toBe(E2E_ACCOUNTS_ENV.GITHUB_CLIENT_ID);
	expect(authorize.searchParams.get('redirect_uri')).toBe(
		`${E2E_ACCOUNTS_ENV.BETTER_AUTH_URL}/api/auth/callback/github`
	);
	expect(authorize.searchParams.get('state')).toBeTruthy();
});

test('a failed GitHub sign-in comes back to a message', async ({ page }) => {
	await open(page, '/account?error=access_denied');
	await expect(page.getByRole('alert')).toHaveText('Signing in did not finish. You can try again.');
});

test('progress made before signing in is merged into the account', async ({ page, context }) => {
	await openLesson(page, '01-hello-world');
	await page.getByRole('button', { name: 'Mark complete' }).click();

	const learner = await createLearner('Merge Learner');
	await signIn(context, learner);
	const merged = page.waitForResponse(
		(response) => response.url().endsWith('/api/progress') && response.request().method() === 'PUT'
	);
	await open(page, '/account');
	expect((await merged).ok()).toBe(true);
	await expect(page.getByText(`Signed in as ${learner.name}`)).toBeVisible();
	await expect(page.getByRole('progressbar', { name: 'Your progress' })).toHaveAttribute(
		'aria-valuenow',
		'1'
	);
	expect(await completedOnServer(page)).toEqual(['01-hello-world']);
	await expect(
		page
			.getByRole('navigation', { name: 'Main', exact: true })
			.getByRole('link', { name: 'Account' })
	).toBeVisible();
});

test('progress follows the learner to another browser, both ways', async ({ browser }) => {
	const learner = await createLearner('Two Devices');
	const laptop = await browser.newContext();
	const phone = await browser.newContext();
	await signIn(laptop, learner);
	await signIn(phone, learner);
	const a = await laptop.newPage();
	const b = await phone.newPage();

	await openLesson(a, '02-variables-and-data-types');
	await clickAndSave(a, 'Mark complete');

	// The other browser sees it...
	await openLesson(b, '02-variables-and-data-types');
	await expect(b.getByRole('button', { name: 'Mark as not complete' })).toHaveAttribute(
		'aria-pressed',
		'true'
	);
	// ...and undoing it there wins over the first browser's cached copy.
	await clickAndSave(b, 'Mark as not complete');
	await openLesson(a, '02-variables-and-data-types');
	await expect(a.getByRole('button', { name: 'Mark complete' })).toBeVisible();
	expect(await completedOnServer(a)).toEqual([]);

	// "Continue where you left off" comes from the account too.
	await openLesson(b, '07-functions');
	await open(a, '/');
	await expect(
		a.getByRole('link', { name: /Continue where you left off\s*Lesson 7: Functions/ })
	).toBeVisible();

	await laptop.close();
	await phone.close();
});

test('signing out forgets the progress in this browser', async ({ page, context }) => {
	const learner = await createLearner('Sign Out');
	await signIn(context, learner);
	await openLesson(page, '03-user-input');
	await clickAndSave(page, 'Mark complete');

	await open(page, '/account');
	await page.getByRole('button', { name: 'Sign out' }).click();
	await expect(page.getByRole('button', { name: 'Sign in with GitHub' })).toBeVisible();
	expect((await page.request.get('/api/progress')).status()).toBe(401);

	await openLesson(page, '03-user-input');
	await expect(page.getByRole('button', { name: 'Mark complete' })).toBeVisible();
});

test('a learner can download their data and delete their account', async ({ page, context }) => {
	const learner = await createLearner('Delete Me');
	await signIn(context, learner);
	await openLesson(page, '04-basic-operators');
	await clickAndSave(page, 'Mark complete');

	await open(page, '/account');
	const downloading = page.waitForEvent('download');
	await page.getByRole('link', { name: 'Download your data' }).click();
	const download = await downloading;
	expect(download.suggestedFilename()).toBe('cfromthegroundup-account.json');
	const data = JSON.parse(readFileSync(await download.path(), 'utf8')) as {
		user: { email: string };
		progress: { lesson: string; completedAt: string | null }[];
		sessions: unknown[];
	};
	expect(data.user.email).toBe(learner.email);
	expect(data.progress.map((row) => row.lesson)).toEqual(['04-basic-operators']);
	expect(data.sessions.length).toBeGreaterThan(0);
	expect(JSON.stringify(data)).not.toContain('token');

	const sessionCookies = await context.cookies();
	await page.getByRole('button', { name: 'Delete account...' }).click();
	await page.getByRole('button', { name: 'Yes, delete my account' }).click();
	await expect(page.getByRole('button', { name: 'Sign in with GitHub' })).toBeVisible();
	const state = await page.request.get('/api/account');
	expect(await state.json()).toEqual({ enabled: true, user: null });

	// The old session is gone for good, even if its cookie were replayed.
	await context.addCookies(sessionCookies);
	expect((await page.request.get('/api/progress')).status()).toBe(401);
});

test('the progress API refuses bad requests', async ({ page, context }) => {
	expect((await page.request.get('/api/progress')).status()).toBe(401);
	await signIn(context, await createLearner('Bad Requests'));
	const post = (data: unknown) =>
		page.request.post('/api/progress', { data, headers: { 'content-type': 'application/json' } });
	expect((await post({ ops: [{ type: 'complete', slug: '../../etc' }] })).status()).toBe(400);
	expect((await post({ ops: [] })).status()).toBe(400);
	expect((await post({ ops: [{ type: 'complete', slug: '01-hello-world' }] })).status()).toBe(200);
	const text = await page.request.post('/api/progress', {
		data: 'ops',
		headers: { 'content-type': 'text/plain' }
	});
	expect([403, 415]).toContain(text.status());
	expect((await page.request.get('/api/progress')).headers()['cache-control']).toContain(
		'no-store'
	);
});

test('the signed-in account page has no detectable WCAG 2.2 AA violations', async ({
	page,
	context
}) => {
	await signIn(context, await createLearner('Axe Learner'));
	for (const theme of ['light', 'dark'] as const) {
		await page.emulateMedia({ colorScheme: theme });
		await open(page, '/account');
		await page.getByRole('button', { name: 'Delete account...' }).click();
		await expect(page.getByRole('button', { name: 'Yes, delete my account' })).toBeVisible();
		const results = await new AxeBuilder({ page })
			.withTags(['wcag2a', 'wcag2aa', 'wcag21a', 'wcag21aa', 'wcag22aa'])
			.analyze();
		expect(results.violations.map((violation) => violation.id)).toEqual([]);
	}
});
