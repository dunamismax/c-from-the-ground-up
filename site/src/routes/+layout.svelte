<script lang="ts">
	import '../app.css';
	import { onMount } from 'svelte';
	import favicon from '#lib/assets/favicon.svg';
	import SearchDialog from '#lib/components/SearchDialog.svelte';
	import SiteFooter from '#lib/components/SiteFooter.svelte';
	import SiteHeader from '#lib/components/SiteHeader.svelte';
	import { account } from '#lib/account/account.svelte.js';
	import { browserBackend } from '#lib/progress/account-backend.js';
	import { progress } from '#lib/progress/progress.svelte.js';
	import { searchUi } from '#lib/search-ui.svelte.js';
	import { isTypingTarget } from '#lib/shortcuts.js';
	import type { LayoutProps } from './$types';

	let { children }: LayoutProps = $props();

	// Learner progress lives in this browser (milestone W4). Pages render
	// without it and fill in once it is read. For a signed-in learner the
	// account then takes over (milestone W6).
	onMount(() => {
		const disconnect = progress.connect(browserBackend(window.localStorage, window));
		void account.load();
		// Lets end-to-end tests wait until the page is interactive.
		document.documentElement.dataset.hydrated = '';
		return disconnect;
	});

	function onKeydown(event: KeyboardEvent) {
		if (event.defaultPrevented || isTypingTarget(event.target)) return;
		const slash = event.key === '/' && !event.ctrlKey && !event.metaKey && !event.altKey;
		const commandK = event.key.toLowerCase() === 'k' && (event.metaKey || event.ctrlKey);
		if (slash || commandK) {
			event.preventDefault();
			searchUi.open = true;
		}
	}
</script>

<svelte:head>
	<link rel="icon" href={favicon} type="image/svg+xml" />
</svelte:head>

<svelte:window onkeydown={onKeydown} />

<a
	href="#main"
	class="sr-only z-50 rounded-lg bg-accent px-4 py-2 font-medium text-on-accent focus:not-sr-only focus:fixed focus:top-3 focus:left-3"
>
	Skip to content
</a>

<div class="flex min-h-screen flex-col">
	<SiteHeader />
	<main id="main" tabindex="-1" class="flex-1 focus:outline-none">
		{@render children()}
	</main>
	<SiteFooter />
</div>

<SearchDialog />
