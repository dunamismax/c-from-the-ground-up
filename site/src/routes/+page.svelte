<script lang="ts">
	import PageMeta from '#lib/components/PageMeta.svelte';
	import { BOOK_URL, GITHUB_URL } from '#lib/site.js';
	import type { PageProps } from './$types';

	let { data }: PageProps = $props();

	// What lesson 1 prints.
	const output = 'Hello, World!\n';

	const steps = [
		{
			title: 'Read the program',
			text: 'Every lesson is one small C file. The explanation lives in its comments, right next to the code it explains.'
		},
		{
			title: 'Compile it yourself',
			text: 'Each page gives you the exact commands for macOS, Linux and Windows (WSL). No IDE, no magic.'
		},
		{
			title: 'Run it and change it',
			text: 'See what it does, then break it on purpose. The fastest way to learn C is to argue with a compiler.'
		}
	];
</script>

<PageMeta />

<section class="relative overflow-hidden border-b border-line">
	<div
		class="mx-auto grid max-w-7xl items-center gap-12 px-4 py-16 sm:px-6 sm:py-24 lg:grid-cols-[minmax(0,1fr)_minmax(0,28rem)]"
	>
		<div>
			<p class="font-mono text-sm font-semibold text-ember">
				Free &middot; Open source &middot; No sign-up
			</p>
			<h1 class="mt-4 text-[2.5rem] leading-[1.05] font-bold tracking-tight sm:text-display">
				Learn C from the ground up.
			</h1>
			<p class="mt-6 max-w-xl text-lg leading-relaxed text-muted">
				{data.lessonCount} small, runnable programs take you from your first
				<code class="rounded bg-sunken px-1.5 py-0.5 font-mono text-[0.9em] text-ink">printf</code> to
				pointers, memory, sockets, threads and a terminal game you build yourself.
			</p>
			<div class="mt-8 flex flex-wrap gap-3">
				<a
					href="/lessons/{data.first.slug}"
					class="rounded-lg bg-accent px-5 py-3 font-semibold text-on-accent shadow-sm hover:bg-accent-hover"
				>
					Start lesson 1
				</a>
				<a
					href="/lessons"
					class="rounded-lg border border-line-strong bg-raised px-5 py-3 font-semibold hover:border-accent hover:text-accent"
				>
					Browse the course
				</a>
			</div>
		</div>

		<figure
			class="overflow-hidden rounded-xl border border-code-line bg-code font-mono text-[0.8125rem] text-code-ink shadow-lg"
			aria-label="Compiling and running lesson 1 in a terminal"
		>
			<div class="flex items-center gap-1.5 border-b border-code-line bg-code-bar px-4 py-2.5">
				<span class="size-2.5 rounded-full bg-[#ff5f57]"></span>
				<span class="size-2.5 rounded-full bg-[#febc2e]"></span>
				<span class="size-2.5 rounded-full bg-[#28c840]"></span>
				<span class="ml-2 text-xs text-code-muted">Lesson 1: {data.first.title}</span>
			</div>
			<!-- A scrollable region must be reachable by keyboard (WCAG 2.1.1). -->
			<!-- svelte-ignore a11y_no_noninteractive_tabindex -->
			<pre tabindex="0" class="overflow-x-auto p-5 leading-relaxed"><code
					>{#each data.first.commands.build as line (line)}<span class="text-code-muted"
							>$ </span>{`${line}\n`}{/each}{#each data.first.commands.run as line (line)}<span
							class="text-code-muted"
							>$ </span>{`${line}\n`}{/each}<span class="text-[#7ee787]">{output}</span><span
						class="text-code-muted"
						>$ </span><span class="cursor">&#9612;</span></code
				></pre>
		</figure>
	</div>
</section>

<section class="mx-auto max-w-7xl px-4 py-16 sm:px-6" aria-labelledby="how-title">
	<h2 id="how-title" class="text-2xl font-bold tracking-tight">How it works</h2>
	<ol class="mt-8 grid gap-6 md:grid-cols-3">
		{#each steps as step, i (step.title)}
			<li class="rounded-xl border border-line bg-raised p-6">
				<span
					class="flex size-8 items-center justify-center rounded-full bg-accent-soft font-mono text-sm font-bold text-accent"
					aria-hidden="true"
				>
					{i + 1}
				</span>
				<h3 class="mt-4 font-semibold">{step.title}</h3>
				<p class="mt-2 text-sm leading-relaxed text-muted">{step.text}</p>
			</li>
		{/each}
	</ol>
</section>

<section class="border-y border-line bg-sunken/50" aria-labelledby="curriculum-title">
	<div class="mx-auto max-w-7xl px-4 py-16 sm:px-6">
		<h2 id="curriculum-title" class="text-2xl font-bold tracking-tight">The path</h2>
		<p class="mt-3 max-w-prose text-muted">
			Five parts, each building on the last. Earlier lessons only use what you have already learned.
		</p>
		<ol class="mt-10 grid gap-6 lg:grid-cols-5">
			{#each data.parts as part (part.number)}
				<li class="flex flex-col rounded-xl border border-line bg-raised p-5">
					<p class="font-mono text-xs font-semibold text-ember">Part {part.number}</p>
					<h3 class="mt-1 leading-snug font-semibold">
						<a class="hover:text-accent" href="/parts/{part.number}">{part.title}</a>
					</h3>
					<p class="mt-2 text-sm text-muted">{part.summary}.</p>
					<ol class="mt-4 space-y-1 border-t border-line pt-4 text-sm">
						{#each part.lessons as lesson (lesson.slug)}
							<li>
								<a class="flex gap-2 text-muted hover:text-accent" href="/lessons/{lesson.slug}">
									<span class="w-5 shrink-0 text-right tabular-nums">{lesson.number}</span>
									<span>{lesson.title}</span>
								</a>
							</li>
						{/each}
					</ol>
				</li>
			{/each}
		</ol>
	</div>
</section>

<section class="mx-auto max-w-7xl px-4 py-16 text-center sm:px-6" aria-labelledby="start-title">
	<h2 id="start-title" class="text-2xl font-bold tracking-tight">All you need is a terminal.</h2>
	<p class="mx-auto mt-3 max-w-prose text-muted">
		Install a compiler once with the <a
			class="font-medium text-accent hover:underline"
			href="/setup"
		>
			setup guide
		</a>, then work through the lessons at your own pace. Prefer to read offline? The course is also
		a
		<a class="font-medium text-accent hover:underline" href={BOOK_URL}>book</a>, and every file is
		on
		<a class="font-medium text-accent hover:underline" href={GITHUB_URL}>GitHub</a>.
	</p>
	<a
		href="/lessons/{data.first.slug}"
		class="mt-8 inline-block rounded-lg bg-accent px-5 py-3 font-semibold text-on-accent shadow-sm hover:bg-accent-hover"
	>
		Start lesson 1
	</a>
</section>

<style>
	@media (prefers-reduced-motion: no-preference) {
		.cursor {
			animation: blink 1.1s steps(1) infinite;
		}
	}

	@keyframes blink {
		50% {
			opacity: 0;
		}
	}
</style>
