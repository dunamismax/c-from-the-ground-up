import { expect, test } from '@playwright/test';

test('the placeholder home page renders', async ({ page }) => {
	await page.goto('/');
	await expect(page.getByRole('heading', { level: 1 })).toHaveText('C From The Ground Up');
});

test('/healthz reports a healthy app and database', async ({ request }) => {
	const response = await request.get('/healthz');
	expect(response.status()).toBe(200);
	expect(await response.json()).toEqual({ status: 'ok', database: 'ok' });
});
