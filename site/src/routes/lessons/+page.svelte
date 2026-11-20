<script lang="ts">
	import ContinueCard from '#lib/components/ContinueCard.svelte';
	import LessonCard from '#lib/components/LessonCard.svelte';
	import PageMeta from '#lib/components/PageMeta.svelte';
	import ProgressBar from '#lib/components/ProgressBar.svelte';
	import { progress } from '#lib/progress/progress.svelte.js';
	import type { PageProps } from './$types';

	let { data }: PageProps = $props();
	const allLessons = $derived(data.parts.flatMap((part) => part.lessons));
	const overall = $derived(progress.progressFor(allLessons.map((lesson) => lesson.slug)));
</script>

<PageMeta
	title="All lessons"
	description="The whole course: 35 lessons in five parts, from Hello World to sockets, threads, ncurses and a capstone text adventure."
/>

<div class="mx-auto max-w-7xl px-4 py-12 sm:px-6">
	<header class="max-w-prose">
		<h1 class="text-title font-bold">The course</h1>
		<p class="mt-4 text-lg text-muted">
			Five parts, taken in order. Each lesson is one small C program you read, compile and run; the
			comments in the code are the lesson.
		</p>
		<p class="mt-3 text-sm text-muted">
			Want every file at once? <a
				class="font-medium text-accent hover:underline"
				href="/downloads/c-from-the-ground-up.zip"
			>
				Download the whole course (.zip)
			</a>.
		</p>
		{#if progress.ready && overall.done > 0}
			<ProgressBar class="mt-8" done={overall.done} total={overall.total} label="Your progress" />
		{/if}
		<ContinueCard class="mt-6" lessons={allLessons} />
	</header>

	{#each data.parts as part (part.number)}
		<section
			id="part-{part.number}"
			aria-labelledby="part-{part.number}-title"
			class="mt-16 scroll-mt-20"
		>
			<div class="max-w-prose">
				<p class="font-mono text-sm font-semibold text-ember">Part {part.number}</p>
				<h2 id="part-{part.number}-title" class="mt-1 text-2xl font-bold tracking-tight">
					<a class="hover:text-accent" href="/parts/{part.number}">{part.title}</a>
				</h2>
				<p class="mt-2 text-muted">{part.summary}.</p>
				{#if progress.ready}
					{@const partProgress = progress.progressFor(part.lessons.map((lesson) => lesson.slug))}
					{#if partProgress.done > 0}
						<ProgressBar
							class="mt-4 max-w-sm"
							done={partProgress.done}
							total={partProgress.total}
							label="Part {part.number} progress"
						/>
					{/if}
				{/if}
			</div>
			<ol class="mt-6 grid gap-4 sm:grid-cols-2 lg:grid-cols-3">
				{#each part.lessons as lesson (lesson.slug)}
					<li><LessonCard {lesson} /></li>
				{/each}
			</ol>
		</section>
	{/each}
</div>
