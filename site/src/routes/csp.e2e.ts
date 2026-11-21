import { expect, test } from '@playwright/test';

// The Content Security Policy (vite.config.ts) must not block anything the
// site itself does. Record every violation the browser reports.
test.beforeEach(async ({ page }) => {
	await page.addInitScript(() => {
		const seen: string[] = [];
		Object.assign(window, { cspViolations: seen });
		document.addEventListener('securitypolicyviolation', (event) => {
			seen.push(`${event.violatedDirective} ${event.blockedURI} ${event.sample}`);
		});
	});
});

async function violations(page: import('@playwright/test').Page): Promise<string[]> {
	return page.evaluate(() => (window as unknown as { cspViolations: string[] }).cspViolations);
}

for (const path of [
	'/',
	'/lessons',
	'/lessons/13-dynamic-memory-allocation',
	'/setup',
	'/glossary',
	'/no-such-page'
]) {
	test(`${path} runs without CSP violations`, async ({ page }) => {
		const response = await page.goto(path);
		await page.locator('html[data-hydrated]').waitFor({ state: 'attached' });
		const policy =
			(await response?.headerValue('content-security-policy')) ??
			(await page.locator('meta[http-equiv="content-security-policy"]').getAttribute('content'));
		expect(policy).toContain("script-src 'self'");
		expect(await violations(page)).toEqual([]);
	});
}

test('search and the theme toggle run without CSP violations', async ({ page }) => {
	await page.goto('/lessons/01-hello-world');
	await page.locator('html[data-hydrated]').waitFor({ state: 'attached' });
	await page.getByRole('button', { name: /Switch to (dark|light) theme/ }).click();
	await page.getByRole('button', { name: 'Search the course' }).click();
	await page.getByRole('combobox', { name: 'Search lessons' }).fill('pointer');
	await expect(page.getByRole('option').first()).toBeVisible();
	await page.keyboard.press('Escape');
	await page.getByRole('button', { name: 'Mark complete' }).click();
	expect(await violations(page)).toEqual([]);
});
