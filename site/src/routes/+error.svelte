<script lang="ts">
	import { page } from '$app/state';
	import PageMeta from '#lib/components/PageMeta.svelte';

	const notFound = $derived(page.status === 404);
</script>

<PageMeta title={notFound ? 'Page not found' : 'Something went wrong'} />

<div class="mx-auto max-w-prose px-4 py-24 sm:px-6">
	<p class="font-mono text-sm font-semibold text-ember">Error {page.status}</p>
	<h1 class="mt-2 text-title font-bold">
		{notFound ? 'Segmentation fault (page not found)' : 'Something went wrong'}
	</h1>
	<p class="mt-4 text-lg text-muted">
		{#if notFound}
			That address does not point at anything here. Lessons live at
			<code class="font-mono text-base">/lessons/</code> followed by their number and name.
		{:else}
			The server could not finish this request. Try again in a moment.
		{/if}
	</p>
	<div class="mt-8 flex flex-wrap gap-3">
		<a
			href="/lessons"
			class="rounded-lg bg-accent px-5 py-3 font-semibold text-on-accent hover:bg-accent-hover"
		>
			All lessons
		</a>
		<a
			href="/"
			class="rounded-lg border border-line-strong bg-raised px-5 py-3 font-semibold hover:border-accent hover:text-accent"
		>
			Home
		</a>
	</div>
</div>
