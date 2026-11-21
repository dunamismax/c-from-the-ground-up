import { expect, test } from '@playwright/test';
import { trackerAttributes, UMAMI_WEBSITE_ID } from '../lib/analytics.js';

// The Umami tracker (BUILD.md W9) is added only when UMAMI_WEBSITE_ID is set,
// once per visit, from this site's own origin.
test('the analytics tracker follows UMAMI_WEBSITE_ID', async ({ page }) => {
	await page.goto('/');
	await page.locator('html[data-hydrated]').waitFor();
	const scripts = page.locator('script[src="/stats/script.js"]');
	const expected = trackerAttributes(UMAMI_WEBSITE_ID);
	if (!expected) {
		await expect(scripts).toHaveCount(0);
		return;
	}
	await expect(scripts).toHaveCount(1);
	for (const [name, value] of Object.entries(expected)) {
		await expect(scripts).toHaveAttribute(name, value);
	}
	// A client-side navigation must not add a second copy.
	await page.getByRole('link', { name: 'Lessons', exact: true }).first().click();
	await expect(page).toHaveURL(/\/lessons$/);
	await expect(scripts).toHaveCount(1);
});
