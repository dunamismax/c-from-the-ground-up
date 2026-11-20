import AxeBuilder from '@axe-core/playwright';
import { expect, test } from '@playwright/test';

// Automated WCAG 2.2 AA checks (axe-core) on each kind of page, in both
// themes. They catch contrast, labelling and structure problems; keyboard and
// screen-reader behaviour still needs the other e2e tests and a human.
const pages = [
	'/',
	'/lessons',
	'/parts/3',
	'/lessons/01-hello-world',
	'/lessons/33-advanced-terminal-ui',
	'/setup',
	'/about',
	'/no-such-page'
];

for (const theme of ['light', 'dark'] as const) {
	for (const path of pages) {
		test(`${path} has no detectable WCAG 2.2 AA violations (${theme})`, async ({ page }) => {
			await page.emulateMedia({ colorScheme: theme });
			await page.goto(path);
			await expect(page.locator('html')).toHaveAttribute('data-theme', theme);
			const results = await new AxeBuilder({ page })
				.withTags(['wcag2a', 'wcag2aa', 'wcag21a', 'wcag21aa', 'wcag22aa'])
				.analyze();
			expect(
				results.violations.map((violation) => ({
					id: violation.id,
					help: violation.help,
					targets: violation.nodes.slice(0, 5).map((node) => node.target.join(' '))
				}))
			).toEqual([]);
		});
	}
}
