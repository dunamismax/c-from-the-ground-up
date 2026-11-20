<script lang="ts">
	import { progress } from '#lib/progress/progress.svelte.js';

	// "Mark complete" at the end of a lesson. Saved in this browser.
	let { slug }: { slug: string } = $props();
	const complete = $derived(progress.isComplete(slug));
</script>

<div
	class="mt-12 flex flex-wrap items-center gap-4 rounded-xl border p-5 {complete
		? 'border-success/40 bg-success-soft'
		: 'border-line bg-raised'}"
>
	<div class="flex-1">
		<p class="font-semibold">{complete ? 'Lesson complete' : 'Finished this lesson?'}</p>
		<p class="text-sm text-muted">
			{complete
				? 'Nice work. Your progress is saved in this browser.'
				: 'Mark it complete to track your progress through the course.'}
		</p>
	</div>
	<button
		type="button"
		class="rounded-lg px-4 py-2 text-sm font-semibold {complete
			? 'border border-line-strong bg-raised hover:border-ink'
			: 'bg-success text-canvas hover:opacity-90'}"
		aria-pressed={complete}
		disabled={!progress.ready}
		onclick={() => progress.setComplete(slug, !complete)}
	>
		{complete ? 'Mark as not complete' : 'Mark complete'}
	</button>
</div>
