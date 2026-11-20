<script lang="ts">
	import type { LessonMeta } from '#lib/content/types.js';
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
	<span class="font-mono text-xs font-semibold text-ember">Lesson {lesson.number}</span>
	<span class="mt-1 text-lg font-semibold tracking-tight group-hover:text-accent">
		{lesson.title}
	</span>
	<span class="mt-2 flex-1 text-sm leading-relaxed text-muted">{lesson.description}</span>
	<span class="mt-4"><LessonBadges meta={lesson.meta} compact /></span>
</a>
