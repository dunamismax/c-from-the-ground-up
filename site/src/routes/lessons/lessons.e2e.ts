import { expect, test } from '@playwright/test';

test('the course overview lists all 35 lessons in five parts', async ({ page }) => {
	await page.goto('/lessons');
	await expect(page.getByRole('heading', { level: 2 })).toHaveCount(5);
	await expect(page.locator('main a[href^="/lessons/"]')).toHaveCount(35);
	await page.getByRole('link', { name: /Lesson 20\s+Linked Lists/ }).click();
	await expect(page).toHaveURL('/lessons/20-linked-lists');
});

test('a lesson page shows its prose, listing, run panel and navigation', async ({ page }) => {
	await page.goto('/lessons/30-multithreaded-file-analyzer');
	await expect(page).toHaveTitle('Lesson 30: Multithreaded File Analyzer | C From The Ground Up');
	await expect(page.getByRole('heading', { level: 1 })).toHaveText('Multithreaded File Analyzer');

	const breadcrumb = page.getByRole('navigation', { name: 'Breadcrumb' });
	await expect(breadcrumb.getByRole('link', { name: 'Part 4' })).toHaveAttribute(
		'href',
		'/parts/4'
	);
	await expect(page.getByRole('list', { name: 'Lesson requirements' })).toContainText('POSIX');

	// The full listing, with its file name and links.
	const listing = page.locator('figure.listing');
	await expect(listing).toHaveCount(1);
	await expect(listing).toContainText('30_multithreaded_file_analyzer.c');
	await expect(listing.getByRole('link', { name: 'Download' })).toHaveAttribute(
		'href',
		'/lessons/30-multithreaded-file-analyzer/files/30_multithreaded_file_analyzer.c'
	);
	await expect(listing.getByRole('link', { name: 'View on GitHub' })).toHaveAttribute(
		'href',
		/^https:\/\/github\.com\/dunamismax\/c-from-the-ground-up\/blob\/main\//
	);

	// The run panel shows the generated commands, with the link flag.
	const run = page.locator('#how-to-compile-and-run');
	await expect(run).toContainText(
		'cc -Wall -Wextra -std=c17 -o 30_multithreaded_file_analyzer 30_multithreaded_file_analyzer.c -pthread'
	);

	// The sidebar marks the current lesson.
	await expect(
		page.getByRole('navigation', { name: 'Course contents' }).locator('[aria-current="page"]')
	).toContainText('Multithreaded File Analyzer');

	// Previous and next.
	const lessonNav = page.getByRole('navigation', { name: 'Lesson navigation' });
	await lessonNav.getByRole('link', { name: /Next: Lesson 31/ }).click();
	await expect(page).toHaveURL('/lessons/31-makefiles');
	await expect(page.locator('figure.listing')).toHaveCount(4);
	await page
		.getByRole('navigation', { name: 'Lesson navigation' })
		.getByRole('link', { name: /Previous: Lesson 30/ })
		.click();
	await expect(page).toHaveURL('/lessons/30-multithreaded-file-analyzer');
});

test('the run panel tabs work with the keyboard', async ({ page }) => {
	await page.goto('/lessons/33-advanced-terminal-ui');
	const tabs = page.getByRole('tablist', { name: 'Operating system' });
	const linux = tabs.getByRole('tab', { name: 'Linux' });
	await linux.click();
	await expect(linux).toHaveAttribute('aria-selected', 'true');
	await expect(page.getByRole('tabpanel')).toContainText(
		'sudo apt install build-essential libncurses-dev'
	);
	await linux.press('ArrowRight');
	const windows = tabs.getByRole('tab', { name: 'Windows (WSL)' });
	await expect(windows).toBeFocused();
	await expect(windows).toHaveAttribute('aria-selected', 'true');
	await expect(page.getByRole('tabpanel')).toContainText('wsl --install');
});

test('the table of contents links to headings on the page', async ({ page }) => {
	await page.setViewportSize({ width: 1400, height: 900 });
	await page.goto('/lessons/31-makefiles');
	const toc = page.getByRole('navigation', { name: 'On this page' }).last();
	await toc.getByRole('link', { name: 'Key Takeaways' }).click();
	await expect(page).toHaveURL(/#key-takeaways$/);
	await expect(page.locator('#key-takeaways')).toBeInViewport();
});

test('lesson files download as plain text', async ({ request }) => {
	const response = await request.get('/lessons/01-hello-world/files/1_hello_world.c');
	expect(response.status()).toBe(200);
	expect(await response.text()).toContain('printf("Hello, World!\\n");');
});

test('the course outline folds away on small screens', async ({ page }) => {
	await page.setViewportSize({ width: 390, height: 844 });
	await page.goto('/lessons/05-conditional-statements');
	const toggle = page.getByRole('button', { name: 'Course contents' });
	const outline = page.locator('#course-outline');
	await expect(toggle).toHaveAttribute('aria-expanded', 'false');
	await expect(outline).toBeHidden();
	await toggle.click();
	await expect(toggle).toHaveAttribute('aria-expanded', 'true');
	await expect(outline.getByRole('link', { name: /Hello, World!/ })).toBeVisible();
});

test('a part page lists its lessons', async ({ page }) => {
	await page.goto('/parts/2');
	await expect(page.getByRole('heading', { level: 1 })).toHaveText(
		'The Intermediate Path — Deeper into C'
	);
	await expect(page.locator('main ol a[href^="/lessons/"]')).toHaveCount(6);
});

test('an unknown lesson is a 404', async ({ page }) => {
	const response = await page.goto('/lessons/99-no-such-lesson');
	expect(response?.status()).toBe(404);
});
