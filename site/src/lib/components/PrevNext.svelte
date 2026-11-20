<script lang="ts">
	import type { LessonRef } from '#lib/content/types.js';

	let { prev, next }: { prev: LessonRef | null; next: LessonRef | null } = $props();
</script>

<nav
	aria-label="Lesson navigation"
	class="mt-16 grid gap-4 border-t border-line pt-8 sm:grid-cols-2"
>
	{#if prev}
		<a
			href="/lessons/{prev.slug}"
			rel="prev"
			class="group rounded-xl border border-line bg-raised p-4 hover:border-accent"
		>
			<span class="text-sm text-muted">&larr; Previous: Lesson {prev.number}</span>
			<span class="mt-1 block font-semibold group-hover:text-accent">{prev.title}</span>
		</a>
	{:else}
		<span></span>
	{/if}
	{#if next}
		<a
			href="/lessons/{next.slug}"
			rel="next"
			class="group rounded-xl border border-line bg-raised p-4 text-right hover:border-accent"
		>
			<span class="text-sm text-muted">Next: Lesson {next.number} &rarr;</span>
			<span class="mt-1 block font-semibold group-hover:text-accent">{next.title}</span>
		</a>
	{:else}
		<a
			href="/lessons"
			class="group rounded-xl border border-line bg-raised p-4 text-right hover:border-accent"
		>
			<span class="text-sm text-muted">You finished the course</span>
			<span class="mt-1 block font-semibold group-hover:text-accent">Back to all lessons</span>
		</a>
	{/if}
</nav>
