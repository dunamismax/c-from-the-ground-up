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

test('robots.txt and the sitemap list every public page', async ({ request }) => {
	const robots = await (await request.get('/robots.txt')).text();
	expect(robots).toContain('Sitemap: https://cfromthegroundup.com/sitemap.xml');
	expect(robots).toContain('Disallow: /api/');
	const sitemap = await request.get('/sitemap.xml');
	expect(sitemap.status()).toBe(200);
	const xml = await sitemap.text();
	expect(xml.match(/<loc>https:\/\/cfromthegroundup\.com\/lessons\/\d{2}-/g)).toHaveLength(35);
	expect(xml).toContain('<loc>https://cfromthegroundup.com/parts/5</loc>');
	expect(xml).toContain('<loc>https://cfromthegroundup.com/setup</loc>');
	expect(xml).toContain('<loc>https://cfromthegroundup.com/privacy</loc>');
});

test('pages carry structured data', async ({ page }) => {
	await page.goto('/');
	const course = JSON.parse(
		(await page.locator('script[type="application/ld+json"]').textContent()) ?? '{}'
	) as Record<string, unknown>;
	expect(course).toMatchObject({ '@type': 'Course', numberOfLessons: 35 });

	await page.goto('/lessons/10-pointers');
	const lesson = JSON.parse(
		(await page.locator('script[type="application/ld+json"]').textContent()) ?? '[]'
	) as Record<string, unknown>[];
	expect(lesson[0]).toMatchObject({ '@type': 'LearningResource', name: 'Lesson 10: Pointers' });
	await expect(page.locator('link[rel="canonical"]')).toHaveAttribute(
		'href',
		'https://cfromthegroundup.com/lessons/10-pointers'
	);
});

test('pages have Open Graph images, and lessons have their own', async ({ page, request }) => {
	const image = async (path: string) => {
		await page.goto(path);
		return page.locator('meta[property="og:image"]').getAttribute('content');
	};
	expect(await image('/')).toBe('https://cfromthegroundup.com/og/site.png');
	expect(await image('/about')).toBe('https://cfromthegroundup.com/og/site.png');
	expect(await image('/lessons/07-functions')).toBe(
		'https://cfromthegroundup.com/og/07-functions.png'
	);
	await expect(page.locator('meta[name="twitter:card"]')).toHaveAttribute(
		'content',
		'summary_large_image'
	);
	for (const name of ['site', '07-functions', '35-capstone-text-adventure']) {
		const response = await request.get(`/og/${name}.png`);
		expect(response.status()).toBe(200);
		expect(response.headers()['content-type']).toBe('image/png');
	}
	expect((await request.get('/og/99-no-such-lesson.png')).status()).toBe(404);
});
