<script lang="ts">
	import type { Part } from '#lib/content/types.js';
	import { progress } from '#lib/progress/progress.svelte.js';

	// The course outline: every part and lesson, with the current one marked.
	// On small screens it folds behind a button above the lesson.
	let { parts, current }: { parts: Pick<Part, 'number' | 'title' | 'lessons'>[]; current: string } =
		$props();
	let open = $state(false);
	let outline: HTMLElement | undefined = $state();

	// Bring the current lesson into view in the sidebar's own scroll area
	// (never the page's) whenever the lesson changes.
	$effect(() => {
		const link = outline?.querySelector<HTMLElement>(`a[href="/lessons/${current}"]`);
		const scroller = link?.closest<HTMLElement>('aside');
		if (!link || !scroller || scroller.scrollHeight <= scroller.clientHeight) return;
		const top = link.offsetTop - scroller.clientHeight / 2;
		scroller.scrollTop = Math.max(0, top);
	});
</script>

<div class="lg:hidden">
	<button
		type="button"
		class="flex w-full items-center justify-between rounded-lg border border-line bg-raised px-4 py-2.5 text-sm font-medium"
		aria-expanded={open}
		aria-controls="course-outline"
		onclick={() => (open = !open)}
	>
		Course contents
		<svg
			class="size-4 transition-transform motion-reduce:transition-none {open ? 'rotate-180' : ''}"
			viewBox="0 0 24 24"
			fill="none"
			stroke="currentColor"
			stroke-width="2"
			aria-hidden="true"
		>
			<path d="m6 9 6 6 6-6" stroke-linecap="round" stroke-linejoin="round" />
		</svg>
	</button>
</div>

<nav
	bind:this={outline}
	id="course-outline"
	aria-label="Course contents"
	class="{open ? 'block' : 'hidden'} mt-3 text-sm lg:mt-0 lg:block"
>
	{#each parts as part (part.number)}
		<div class="mb-5">
			<p class="mb-1.5 px-2 text-xs font-semibold tracking-wide text-muted uppercase">
				<a href="/parts/{part.number}" class="hover:text-ink">Part {part.number}</a>
			</p>
			<p class="mb-2 px-2 text-xs text-muted">{part.title}</p>
			{#if progress.ready}
				{@const partProgress = progress.progressFor(part.lessons.map((lesson) => lesson.slug))}
				{#if partProgress.done > 0}
					<div class="mx-2 mb-2 h-1 overflow-hidden rounded-full bg-sunken" aria-hidden="true">
						<div class="h-full bg-success" style:width="{partProgress.percent}%"></div>
					</div>
				{/if}
			{/if}
			<ol>
				{#each part.lessons as lesson (lesson.slug)}
					<li>
						<a
							href="/lessons/{lesson.slug}"
							class="flex gap-2 rounded-md px-2 py-1 text-muted hover:bg-sunken hover:text-ink aria-[current=page]:bg-accent-soft aria-[current=page]:font-medium aria-[current=page]:text-accent"
							aria-current={lesson.slug === current ? 'page' : undefined}
						>
							<span class="w-5 shrink-0 text-right tabular-nums">{lesson.number}</span>
							<span class="flex-1">{lesson.title}</span>
							{#if progress.isComplete(lesson.slug)}
								<svg
									class="mt-0.5 size-4 shrink-0 text-success"
									viewBox="0 0 24 24"
									fill="none"
									stroke="currentColor"
									stroke-width="2.5"
									aria-hidden="true"
								>
									<path d="M20 6 9 17l-5-5" stroke-linecap="round" stroke-linejoin="round" />
								</svg>
								<span class="sr-only">(complete)</span>
							{/if}
						</a>
					</li>
				{/each}
			</ol>
		</div>
	{/each}
</nav>
