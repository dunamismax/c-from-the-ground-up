<script lang="ts">
	import type { LessonRef } from '#lib/content/types.js';
	import { progress } from '#lib/progress/progress.svelte.js';

	// "Continue where you left off": shown only to a returning learner.
	let { lessons, class: className = '' }: { lessons: LessonRef[]; class?: string } = $props();

	const target = $derived.by(() => {
		if (!progress.ready) return null;
		const slug = progress.continueSlug(lessons.map((lesson) => lesson.slug));
		return lessons.find((lesson) => lesson.slug === slug) ?? null;
	});
	const resuming = $derived(target !== null && progress.lastViewed?.slug === target.slug);
</script>

{#if target}
	<a
		href="/lessons/{target.slug}"
		class="group flex items-center gap-4 rounded-xl border border-accent/40 bg-accent-soft p-4 hover:border-accent {className}"
	>
		<span
			class="flex size-10 shrink-0 items-center justify-center rounded-full bg-accent text-on-accent"
			aria-hidden="true"
		>
			<svg class="size-5" viewBox="0 0 24 24" fill="currentColor"><path d="M8 5v14l11-7z" /></svg>
		</span>
		<span>
			<span class="block text-sm text-muted">
				{resuming ? 'Continue where you left off' : 'Up next'}
			</span>
			<span class="block font-semibold group-hover:text-accent">
				Lesson {target.number}: {target.title}
			</span>
		</span>
	</a>
{/if}
