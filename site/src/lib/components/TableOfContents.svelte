<script lang="ts">
	import { onMount } from 'svelte';
	import type { TocEntry } from '#lib/content/types.js';

	// "On this page": the lesson's headings, with the one being read marked.
	let { entries }: { entries: TocEntry[] } = $props();
	let active = $state('');

	onMount(() => {
		const headings = entries
			.map((entry) => document.getElementById(entry.id))
			.filter((element): element is HTMLElement => element !== null);
		const observer = new IntersectionObserver(
			(observed) => {
				for (const item of observed) {
					if (item.isIntersecting) active = item.target.id;
				}
			},
			{ rootMargin: '-80px 0px -70% 0px' }
		);
		headings.forEach((heading) => observer.observe(heading));
		return () => observer.disconnect();
	});
</script>

<nav aria-labelledby="toc-heading" class="text-sm">
	<p id="toc-heading" class="mb-3 font-semibold text-ink">On this page</p>
	<ul class="space-y-1.5 border-l border-line">
		{#each entries as entry (entry.id)}
			<li>
				<a
					href="#{entry.id}"
					class="-ml-px block border-l-2 py-0.5 text-muted hover:text-ink {entry.depth === 3
						? 'pl-6'
						: 'pl-3'} {active === entry.id
						? 'border-accent font-medium text-ink'
						: 'border-transparent'}"
					aria-current={active === entry.id ? 'location' : undefined}
				>
					{entry.text}
				</a>
			</li>
		{/each}
	</ul>
</nav>
