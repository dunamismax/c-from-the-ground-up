import { expect, test } from '@playwright/test';

test('the landing page pitches the course and starts lesson 1', async ({ page }) => {
	await page.goto('/');
	await expect(page).toHaveTitle('C From The Ground Up');
	await expect(page.getByRole('heading', { level: 1 })).toHaveText('Learn C from the ground up.');

	// The curriculum overview lists every part.
	const path = page.getByRole('region', { name: 'The path' });
	for (let part = 1; part <= 5; part++) {
		await expect(path.getByText(`Part ${part}`, { exact: true })).toBeVisible();
	}

	await page.getByRole('link', { name: 'Start lesson 1' }).first().click();
	await expect(page).toHaveURL('/lessons/01-hello-world');
	await expect(page.getByRole('heading', { level: 1 })).toHaveText('Hello, World!');
});

test('the header links to the main pages', async ({ page }) => {
	await page.goto('/');
	const nav = page.getByRole('navigation', { name: 'Main', exact: true });
	await nav.getByRole('link', { name: 'Setup' }).click();
	await expect(page.getByRole('heading', { level: 1 })).toHaveText('Setup guide');
	await nav.getByRole('link', { name: 'About' }).click();
	await expect(page.getByRole('heading', { level: 1 })).toHaveText('About the course');
	await nav.getByRole('link', { name: 'Lessons' }).click();
	await expect(page.getByRole('heading', { level: 1 })).toHaveText('The course');
	await expect(nav.getByRole('link', { name: 'Lessons' })).toHaveAttribute('aria-current', 'page');
});

test('the theme toggle switches and remembers the theme', async ({ page }) => {
	await page.emulateMedia({ colorScheme: 'light' });
	await page.goto('/');
	const html = page.locator('html');
	await expect(html).toHaveAttribute('data-theme', 'light');
	await page.getByRole('button', { name: 'Switch to dark theme' }).click();
	await expect(html).toHaveAttribute('data-theme', 'dark');
	await page.reload();
	await expect(html).toHaveAttribute('data-theme', 'dark');
	await expect(page.getByRole('button', { name: 'Switch to light theme' })).toBeVisible();
});

test('a skip link jumps to the main content', async ({ page }) => {
	await page.goto('/');
	await page.keyboard.press('Tab');
	const skip = page.getByRole('link', { name: 'Skip to content' });
	await expect(skip).toBeFocused();
	await skip.press('Enter');
	await expect(page).toHaveURL(/#main$/);
});

test('an unknown page is a 404 with a way back', async ({ page }) => {
	const response = await page.goto('/no-such-page');
	expect(response?.status()).toBe(404);
	await expect(page.getByRole('heading', { level: 1 })).toContainText('page not found');
	await expect(page.getByRole('link', { name: 'All lessons' }).first()).toBeVisible();
});

test('/healthz reports a healthy app and database', async ({ request }) => {
	const response = await request.get('/healthz');
	expect(response.status()).toBe(200);
	expect(await response.json()).toEqual({ status: 'ok', database: 'ok' });
});
