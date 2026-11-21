<script lang="ts">
	import PageMeta from '#lib/components/PageMeta.svelte';
	import type { PageProps } from './$types';

	let { data }: PageProps = $props();
</script>

<PageMeta
	title="Glossary"
	description="The key C terms the course introduces, from address to variable, each linked to the lesson that teaches it."
/>

<div class="mx-auto max-w-prose px-4 py-12 sm:px-6">
	<h1 class="text-title font-bold">{data.page.title}</h1>
	<nav aria-label="Glossary letters" class="mt-6">
		<ul class="flex flex-wrap gap-x-3 gap-y-1 font-mono text-sm">
			{#each data.page.toc as entry (entry.id)}
				<li>
					<a
						class="inline-flex min-h-6 min-w-6 items-center justify-center text-accent hover:underline"
						href="#{entry.id}"
					>
						{entry.text}
					</a>
				</li>
			{/each}
		</ul>
	</nav>
	<div class="prose mt-8">
		<!-- eslint-disable-next-line svelte/no-at-html-tags -- rendered from src/glossary.md at build time -->
		{@html data.page.html}
	</div>
</div>
