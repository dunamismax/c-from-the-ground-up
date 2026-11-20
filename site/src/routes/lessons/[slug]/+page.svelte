<script lang="ts">
	import CodeBlock from '#lib/components/CodeBlock.svelte';
	import CourseSidebar from '#lib/components/CourseSidebar.svelte';
	import LessonBadges from '#lib/components/LessonBadges.svelte';
	import PageMeta from '#lib/components/PageMeta.svelte';
	import PrevNext from '#lib/components/PrevNext.svelte';
	import RunPanel from '#lib/components/RunPanel.svelte';
	import TableOfContents from '#lib/components/TableOfContents.svelte';
	import type { PageProps } from './$types';

	let { data }: PageProps = $props();
	const lesson = $derived(data.lesson);
</script>

<PageMeta title="Lesson {lesson.number}: {lesson.title}" description={data.summary} />

<div
	class="mx-auto max-w-7xl px-4 sm:px-6 lg:grid lg:grid-cols-[15rem_minmax(0,1fr)] lg:gap-10 xl:grid-cols-[15rem_minmax(0,1fr)_13rem]"
>
	<aside
		class="pt-6 lg:sticky lg:top-14 lg:h-[calc(100vh-3.5rem)] lg:overflow-y-auto lg:pt-8 lg:pb-8"
	>
		<CourseSidebar parts={data.outline} current={lesson.slug} />
	</aside>

	<article class="min-w-0 py-8" aria-labelledby="lesson-title">
		<nav aria-label="Breadcrumb" class="text-sm text-muted">
			<ol class="flex flex-wrap items-center gap-1.5">
				<li><a class="hover:text-ink" href="/lessons">Lessons</a></li>
				<li aria-hidden="true">/</li>
				<li>
					<a class="hover:text-ink" href="/parts/{lesson.part.number}">Part {lesson.part.number}</a>
				</li>
				<li aria-hidden="true">/</li>
				<li><a aria-current="page" href="/lessons/{lesson.slug}">Lesson {lesson.number}</a></li>
			</ol>
		</nav>

		<header class="mt-6 mb-8 max-w-prose">
			<p class="font-mono text-sm font-semibold text-ember">
				Part {lesson.part.number} &middot; Lesson {lesson.number}
			</p>
			<h1 id="lesson-title" class="mt-2 text-title font-bold">{lesson.title}</h1>
			<div class="mt-4"><LessonBadges meta={lesson.meta} /></div>
		</header>

		<details class="mb-8 max-w-prose rounded-lg border border-line bg-raised px-4 py-2.5 xl:hidden">
			<summary class="cursor-pointer text-sm font-medium">On this page</summary>
			<div class="mt-3 mb-1"><TableOfContents entries={lesson.toc} /></div>
		</details>

		{#each lesson.segments as segment, i (i)}
			{#if segment.kind === 'html'}
				<div class="prose max-w-prose">
					<!-- eslint-disable-next-line svelte/no-at-html-tags -- rendered from our own chapter at build time -->
					{@html segment.html}
				</div>
			{:else if segment.kind === 'source'}
				<CodeBlock
					source={lesson.sources[segment.index]}
					downloadHref="/lessons/{lesson.slug}/files/{lesson.sources[segment.index].name}"
				/>
			{:else}
				<RunPanel meta={lesson.meta} commands={lesson.commands} notesHtml={lesson.runNotesHtml} />
			{/if}
		{/each}

		<div class="max-w-prose">
			<PrevNext prev={lesson.prev} next={lesson.next} />
		</div>
	</article>

	<aside class="hidden xl:block">
		<div class="sticky top-14 max-h-[calc(100vh-3.5rem)] overflow-y-auto py-8">
			<TableOfContents entries={lesson.toc} />
		</div>
	</aside>
</div>
