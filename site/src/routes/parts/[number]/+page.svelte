<script lang="ts">
	import LessonCard from '#lib/components/LessonCard.svelte';
	import PageMeta from '#lib/components/PageMeta.svelte';
	import ProgressBar from '#lib/components/ProgressBar.svelte';
	import { progress } from '#lib/progress/progress.svelte.js';
	import type { PageProps } from './$types';

	let { data }: PageProps = $props();
	const part = $derived(data.part);
</script>

<PageMeta title="Part {part.number}: {part.title}" description="{part.summary}." />

<div class="mx-auto max-w-7xl px-4 py-12 sm:px-6">
	<nav aria-label="Breadcrumb" class="text-sm text-muted">
		<ol class="flex items-center gap-1.5">
			<li><a class="hover:text-ink" href="/lessons">Lessons</a></li>
			<li aria-hidden="true">/</li>
			<li><a aria-current="page" href="/parts/{part.number}">Part {part.number}</a></li>
		</ol>
	</nav>
	<header class="mt-6 max-w-prose">
		<p class="font-mono text-sm font-semibold text-ember">Part {part.number}</p>
		<h1 class="mt-1 text-title font-bold">{part.title}</h1>
		<p class="mt-4 text-lg text-muted">{part.summary}.</p>
		{#if progress.ready}
			{@const partProgress = progress.progressFor(data.lessons.map((lesson) => lesson.slug))}
			{#if partProgress.done > 0}
				<ProgressBar
					class="mt-6 max-w-sm"
					done={partProgress.done}
					total={partProgress.total}
					label="Part {part.number} progress"
				/>
			{/if}
		{/if}
	</header>
	<ol class="mt-10 grid gap-4 sm:grid-cols-2 lg:grid-cols-3">
		{#each data.lessons as lesson (lesson.slug)}
			<li><LessonCard {lesson} /></li>
		{/each}
	</ol>
	<nav aria-label="Parts" class="mt-12 flex flex-wrap justify-between gap-4 text-sm">
		{#if data.prev}
			<a class="font-medium text-accent hover:underline" href="/parts/{data.prev.number}">
				&larr; Part {data.prev.number}: {data.prev.title}
			</a>
		{:else}
			<span></span>
		{/if}
		{#if data.next}
			<a class="font-medium text-accent hover:underline" href="/parts/{data.next.number}">
				Part {data.next.number}: {data.next.title} &rarr;
			</a>
		{/if}
	</nav>
</div>
