<script lang="ts">
	import LessonCard from '#lib/components/LessonCard.svelte';
	import PageMeta from '#lib/components/PageMeta.svelte';
	import type { PageProps } from './$types';

	let { data }: PageProps = $props();
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
			</div>
			<ol class="mt-6 grid gap-4 sm:grid-cols-2 lg:grid-cols-3">
				{#each part.lessons as lesson (lesson.slug)}
					<li><LessonCard {lesson} /></li>
				{/each}
			</ol>
		</section>
	{/each}
</div>
