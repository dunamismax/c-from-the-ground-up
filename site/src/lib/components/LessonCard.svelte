<script lang="ts">
	import type { LessonMeta } from '#lib/content/types.js';
	import { progress } from '#lib/progress/progress.svelte.js';
	import LessonBadges from './LessonBadges.svelte';

	let {
		lesson
	}: {
		lesson: { number: number; slug: string; title: string; description: string; meta: LessonMeta };
	} = $props();
</script>

<a
	href="/lessons/{lesson.slug}"
	class="group flex h-full flex-col rounded-xl border border-line bg-raised p-5 shadow-xs transition-colors hover:border-accent motion-reduce:transition-none"
>
	<span class="flex items-center justify-between gap-2">
		<span class="font-mono text-xs font-semibold text-ember">Lesson {lesson.number}</span>
		{#if progress.isComplete(lesson.slug)}
			<span class="rounded-full bg-success-soft px-2 py-0.5 text-xs font-medium text-success">
				Complete
			</span>
		{/if}
	</span>
	<span class="mt-1 text-lg font-semibold tracking-tight group-hover:text-accent">
		{lesson.title}
	</span>
	<span class="mt-2 flex-1 text-sm leading-relaxed text-muted">{lesson.description}</span>
	<span class="mt-4"><LessonBadges meta={lesson.meta} compact /></span>
</a>
