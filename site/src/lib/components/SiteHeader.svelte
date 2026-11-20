<script lang="ts">
	import { page } from '$app/state';
	import { account } from '#lib/account/account.svelte.js';
	import { searchUi } from '#lib/search-ui.svelte.js';
	import { GITHUB_URL } from '#lib/site.js';
	import Logo from './Logo.svelte';
	import ThemeToggle from './ThemeToggle.svelte';

	const links = [
		{ href: '/lessons', label: 'Lessons' },
		{ href: '/setup', label: 'Setup' },
		{ href: '/about', label: 'About' }
	];

	function isCurrent(href: string): boolean {
		const path = page.url.pathname;
		return path === href || path.startsWith(`${href}/`);
	}
</script>

<header class="sticky top-0 z-30 border-b border-line bg-canvas/90 backdrop-blur">
	<div class="mx-auto flex h-14 max-w-7xl items-center gap-4 px-4 sm:px-6">
		<a href="/" class="flex items-center gap-2.5 font-semibold tracking-tight text-ink">
			<Logo class="size-7" />
			<span>C From The Ground Up</span>
		</a>
		<nav aria-label="Main" class="ml-auto flex items-center gap-1 text-sm">
			{#each links as link (link.href)}
				<a
					href={link.href}
					class="hidden rounded-lg px-3 py-2 font-medium text-muted hover:bg-sunken hover:text-ink aria-[current=page]:text-ink sm:inline-block"
					aria-current={isCurrent(link.href) ? 'page' : undefined}
				>
					{link.label}
				</a>
			{/each}
			<button
				type="button"
				class="inline-flex h-9 items-center gap-2 rounded-lg border border-line px-2.5 text-muted hover:border-line-strong hover:text-ink"
				aria-label="Search the course"
				aria-keyshortcuts="/"
				onclick={() => (searchUi.open = true)}
			>
				<svg
					class="size-4"
					viewBox="0 0 24 24"
					fill="none"
					stroke="currentColor"
					stroke-width="2"
					aria-hidden="true"
				>
					<circle cx="11" cy="11" r="7" /><path d="m20 20-3.5-3.5" stroke-linecap="round" />
				</svg>
				<span class="hidden md:inline">Search</span>
				<kbd
					class="hidden rounded border border-line px-1.5 font-mono text-xs md:inline"
					aria-hidden="true"
				>
					/
				</kbd>
			</button>
			{#if account.status === 'signed-in' || account.status === 'signed-out'}
				<a
					href="/account"
					class="rounded-lg px-3 py-2 font-medium text-muted hover:bg-sunken hover:text-ink aria-[current=page]:text-ink"
					aria-current={isCurrent('/account') ? 'page' : undefined}
				>
					{account.status === 'signed-in' ? 'Account' : 'Sign in'}
				</a>
			{/if}
			<a
				href={GITHUB_URL}
				class="inline-flex size-9 items-center justify-center rounded-lg text-muted hover:bg-sunken hover:text-ink"
				aria-label="Source on GitHub"
				title="Source on GitHub"
			>
				<svg class="size-5" viewBox="0 0 24 24" fill="currentColor" aria-hidden="true">
					<path
						d="M12 .5a11.5 11.5 0 0 0-3.64 22.41c.58.1.79-.25.79-.56v-2c-3.2.7-3.88-1.37-3.88-1.37-.52-1.33-1.28-1.69-1.28-1.69-1.05-.71.08-.7.08-.7 1.16.08 1.77 1.19 1.77 1.19 1.03 1.77 2.7 1.26 3.36.96.1-.75.4-1.26.73-1.55-2.55-.29-5.24-1.28-5.24-5.68 0-1.26.45-2.28 1.19-3.09-.12-.29-.52-1.46.11-3.05 0 0 .97-.31 3.17 1.18a11 11 0 0 1 5.77 0c2.2-1.49 3.17-1.18 3.17-1.18.63 1.59.23 2.76.11 3.05.74.81 1.19 1.83 1.19 3.09 0 4.41-2.69 5.38-5.26 5.67.41.36.78 1.06.78 2.14v3.17c0 .31.21.67.8.56A11.5 11.5 0 0 0 12 .5Z"
					/>
				</svg>
			</a>
			<ThemeToggle />
		</nav>
	</div>
	<nav
		aria-label="Main (small screens)"
		class="flex gap-1 border-t border-line px-2 text-sm sm:hidden"
	>
		{#each links as link (link.href)}
			<a
				href={link.href}
				class="flex-1 rounded-lg px-3 py-2 text-center font-medium text-muted hover:text-ink aria-[current=page]:text-ink"
				aria-current={isCurrent(link.href) ? 'page' : undefined}
			>
				{link.label}
			</a>
		{/each}
	</nav>
</header>
