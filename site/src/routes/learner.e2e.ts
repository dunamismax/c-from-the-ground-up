import { strFromU8, unzipSync } from 'fflate';
import { expect, test, type Page } from '@playwright/test';

/** Go to a page and wait until it is hydrated (keyboard shortcuts work). */
async function open(page: Page, path: string) {
	await page.goto(path);
	await page.locator('html[data-hydrated]').waitFor({ state: 'attached' });
}

test('progress: mark a lesson complete and continue where you left off', async ({ page }) => {
	await page.goto('/lessons/01-hello-world');
	const complete = page.getByRole('button', { name: 'Mark complete' });
	await expect(complete).toBeEnabled();
	await complete.click();
	await expect(page.getByRole('button', { name: 'Mark as not complete' })).toHaveAttribute(
		'aria-pressed',
		'true'
	);
	await expect(
		page
			.getByRole('navigation', { name: 'Course contents' })
			.getByRole('link', { name: /Hello, World!.*\(complete\)/ })
	).toBeVisible();

	// The overview shows overall progress and suggests the next lesson.
	await page.goto('/lessons');
	await expect(page.getByRole('progressbar', { name: 'Your progress' })).toHaveAttribute(
		'aria-valuenow',
		'1'
	);
	await expect(
		page.getByRole('link', { name: /Up next\s*Lesson 2: Variables and Data Types/ })
	).toBeVisible();

	// Progress survives a reload and shows on the landing page.
	await open(page, '/lessons/07-functions');
	await open(page, '/');
	const resume = page.getByRole('link', {
		name: /Continue where you left off\s*Lesson 7: Functions/
	});
	await expect(resume).toBeVisible();
	await resume.click();
	await expect(page).toHaveURL('/lessons/07-functions');

	// Undo.
	await page.goto('/lessons/01-hello-world');
	await page.getByRole('button', { name: 'Mark as not complete' }).click();
	await expect(page.getByRole('button', { name: 'Mark complete' })).toBeVisible();
});

test('search finds a topic and opens the section', async ({ page }) => {
	await open(page, '/');
	await page.keyboard.press('/');
	const box = page.getByRole('combobox', { name: 'Search lessons' });
	await expect(box).toBeFocused();
	await box.fill('malloc');
	const results = page.getByRole('listbox', { name: 'Results' });
	await expect(results.getByRole('option').first()).toContainText('Dynamic Memory Allocation');
	await box.press('Enter');
	await expect(page).toHaveURL(/\/lessons\/13-dynamic-memory-allocation/);
	await expect(page.getByRole('dialog')).toBeHidden();
});

test('search opens from the header and closes with Escape', async ({ page }) => {
	await open(page, '/lessons');
	await page.getByRole('button', { name: 'Search the course' }).click();
	const box = page.getByRole('combobox', { name: 'Search lessons' });
	await expect(box).toBeFocused();
	await box.fill('zzzzqqq');
	await expect(page.getByText('No lessons mention')).toBeVisible();
	await box.press('Escape');
	await expect(page.getByRole('dialog')).toBeHidden();
});

test('j and k move between lessons', async ({ page }) => {
	await open(page, '/lessons/05-conditional-statements');
	await page.locator('h1').click();
	await page.keyboard.press('j');
	await expect(page).toHaveURL('/lessons/06-loops');
	await page.keyboard.press('k');
	await expect(page).toHaveURL('/lessons/05-conditional-statements');
	await page.keyboard.press('ArrowRight');
	await expect(page).toHaveURL('/lessons/06-loops');
});

test('try it online opens the lesson in Compiler Explorer', async ({ page, context }) => {
	let posted: unknown = null;
	await context.route('https://godbolt.org/api/shortener', async (route) => {
		posted = route.request().postDataJSON();
		await route.fulfill({ json: { url: 'https://godbolt.org/z/e2etest' } });
	});
	await context.route('https://godbolt.org/z/**', (route) =>
		route.fulfill({ contentType: 'text/html', body: '<title>Compiler Explorer</title>' })
	);

	await page.goto('/lessons/16-simple-calculator');
	const [popup] = await Promise.all([
		page.waitForEvent('popup'),
		page.getByRole('button', { name: 'Try it online' }).click()
	]);
	await expect(popup).toHaveURL('https://godbolt.org/z/e2etest');
	const session = (posted as { sessions: { source: string; executors: { arguments: string }[] }[] })
		.sessions[0];
	expect(session.source).toContain('Simple Calculator');
	expect(session.executors[0].arguments).toBe('5 + 3');
});

test('lessons that need a real system say so instead', async ({ page }) => {
	await page.goto('/lessons/29-tiny-shell');
	await expect(page.getByRole('button', { name: 'Try it online' })).toHaveCount(0);
	await expect(
		page.getByText('It uses POSIX system calls, so run this lesson on your own machine')
	).toBeVisible();
});

test('zip downloads hold the lesson files', async ({ request }) => {
	const lesson = await request.get('/downloads/31-makefiles.zip');
	expect(lesson.status()).toBe(200);
	const files = unzipSync(new Uint8Array(await lesson.body()));
	expect(Object.keys(files).sort()).toEqual([
		'31-makefiles/Makefile',
		'31-makefiles/helper.c',
		'31-makefiles/helper.h',
		'31-makefiles/main.c'
	]);

	const course = await request.get('/downloads/c-from-the-ground-up.zip');
	expect(course.status()).toBe(200);
	const all = unzipSync(new Uint8Array(await course.body()));
	const names = Object.keys(all);
	expect(names.filter((name) => name.endsWith('.c'))).toHaveLength(37);
	expect(names).toContain('c-from-the-ground-up/LICENSE');
	expect(
		strFromU8(all['c-from-the-ground-up/Part 1 - The Beginner Path_ Core Concepts/1_hello_world.c'])
	).toContain('Hello, World!');
});
