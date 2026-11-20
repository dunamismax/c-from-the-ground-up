<script lang="ts">
	import { onMount } from 'svelte';
	import type { Commands, LessonMeta } from '#lib/content/types.js';
	import CopyButton from './CopyButton.svelte';
	import TryOnline from './TryOnline.svelte';

	// "How to Compile and Run": one-time setup per operating system, then the
	// build and run commands generated from the lesson metadata.
	let {
		slug,
		meta,
		commands,
		notesHtml,
		fileCount
	}: {
		slug: string;
		meta: LessonMeta;
		commands: Commands;
		notesHtml: string;
		fileCount: number;
	} = $props();

	// Why a lesson cannot run in Compiler Explorer, if it cannot.
	const localReason = $derived(
		meta.platform === 'posix'
			? 'It uses POSIX system calls'
			: meta.platform === 'ncurses'
				? 'It draws a full-screen terminal interface with ncurses'
				: meta.build === 'make'
					? 'It is several files built with make'
					: meta.usesFiles
						? 'It reads or writes files'
						: null
	);

	type Os = 'macos' | 'linux' | 'windows';
	const tabs: { id: Os; label: string }[] = [
		{ id: 'macos', label: 'macOS' },
		{ id: 'linux', label: 'Linux' },
		{ id: 'windows', label: 'Windows (WSL)' }
	];
	const STORAGE_KEY = 'run-panel-os';

	let selected = $state<Os>('macos');
	const ncurses = $derived(meta.libraries.includes('ncurses'));

	onMount(() => {
		let saved: string | null = null;
		try {
			saved = localStorage.getItem(STORAGE_KEY);
		} catch {
			// No storage: fall back to guessing from the browser.
		}
		if (saved === 'macos' || saved === 'linux' || saved === 'windows') {
			selected = saved;
		} else if (/Windows/.test(navigator.userAgent)) {
			selected = 'windows';
		} else if (/Linux|X11|CrOS/.test(navigator.userAgent) && !/Android/.test(navigator.userAgent)) {
			selected = 'linux';
		}
	});

	function choose(os: Os) {
		selected = os;
		try {
			localStorage.setItem(STORAGE_KEY, os);
		} catch {
			// Not remembered; that is fine.
		}
	}

	function onKeydown(event: KeyboardEvent) {
		const index = tabs.findIndex((tab) => tab.id === selected);
		const moves: Record<string, number> = {
			ArrowRight: (index + 1) % tabs.length,
			ArrowLeft: (index - 1 + tabs.length) % tabs.length,
			Home: 0,
			End: tabs.length - 1
		};
		const next = moves[event.key];
		if (next === undefined) return;
		event.preventDefault();
		choose(tabs[next].id);
		document.getElementById(`run-tab-${tabs[next].id}`)?.focus();
	}

	const linuxPackages = $derived(ncurses ? 'build-essential libncurses-dev' : 'build-essential');
	const fedoraPackages = $derived(ncurses ? 'gcc make ncurses-devel' : 'gcc make');
</script>

<section id="how-to-compile-and-run" aria-labelledby="run-heading" class="not-prose my-10">
	<h2 id="run-heading" class="text-2xl font-bold tracking-tight">
		<a class="hover:underline" href="#how-to-compile-and-run">How to Compile and Run</a>
	</h2>

	<div class="mt-4 mb-6">
		{#if commands.onlineOptions}
			<TryOnline
				fileHref="/lessons/{slug}/files/{meta.programs[0].source}"
				options={commands.onlineOptions}
				args={meta.programs[0].args}
			/>
		{:else if localReason}
			<p class="text-sm text-muted">
				{localReason}, so run this lesson on your own machine (macOS, Linux or WSL) rather than
				online.
			</p>
		{/if}
	</div>

	<div class="overflow-hidden rounded-xl border border-line bg-raised">
		<div
			role="tablist"
			aria-label="Operating system"
			class="flex gap-1 border-b border-line px-2 pt-2"
		>
			{#each tabs as tab (tab.id)}
				<button
					type="button"
					role="tab"
					id="run-tab-{tab.id}"
					aria-selected={selected === tab.id}
					aria-controls="run-panel"
					tabindex={selected === tab.id ? 0 : -1}
					class="-mb-px rounded-t-lg border border-transparent px-3 py-2 text-sm font-medium text-muted hover:text-ink aria-selected:border-line aria-selected:border-b-raised aria-selected:bg-raised aria-selected:text-ink"
					onclick={() => choose(tab.id)}
					onkeydown={onKeydown}
				>
					{tab.label}
				</button>
			{/each}
		</div>

		<div
			id="run-panel"
			role="tabpanel"
			aria-labelledby="run-tab-{selected}"
			class="space-y-5 p-5 text-[0.9375rem]"
		>
			<div class="text-muted">
				<p class="font-medium text-ink">Once, before your first lesson</p>
				{#if selected === 'macos'}
					<p class="mt-1">
						Install Apple's command line tools. They include the <code class="font-mono text-sm">
							cc
						</code>
						compiler{meta.build === 'make' ? ', make' : ''}{ncurses ? ' and ncurses' : ''}:
						<code class="mt-1 block font-mono text-sm text-ink">xcode-select --install</code>
					</p>
				{:else if selected === 'linux'}
					<p class="mt-1">
						Debian and Ubuntu:
						<code class="mt-1 block font-mono text-sm text-ink">
							sudo apt install {linuxPackages}
						</code>
					</p>
					<p class="mt-2">
						Fedora:
						<code class="mt-1 block font-mono text-sm text-ink">
							sudo dnf install {fedoraPackages}
						</code>
					</p>
				{:else}
					<p class="mt-1">
						Run the lessons inside Ubuntu on the Windows Subsystem for Linux. In PowerShell, run
						<code class="font-mono text-sm text-ink">wsl --install</code>, restart, open
						<strong class="font-medium text-ink">Ubuntu</strong>, then install the tools:
						<code class="mt-1 block font-mono text-sm text-ink">
							sudo apt install {linuxPackages}
						</code>
					</p>
				{/if}
				<p class="mt-2">
					New to the terminal? The <a class="font-medium text-accent hover:underline" href="/setup">
						setup guide
					</a>
					walks through it.
				</p>
			</div>

			{@render block('Build', commands.build)}
			{@render block(
				commands.run.length > 1 ? 'Run (one terminal each, in this order)' : 'Run',
				commands.run
			)}

			<ul class="list-disc space-y-1 pl-5 text-muted marker:text-line-strong">
				<li>
					Run these commands in the folder that holds the lesson's {fileCount > 1
						? 'files'
						: 'file'}.
					<a class="font-medium text-accent hover:underline" href="/downloads/{slug}.zip">
						Download {fileCount > 1 ? 'them' : 'it'} as a zip
					</a>.
				</li>
				{#if meta.interactive}
					<li>The program waits for your input: type an answer and press Enter.</li>
				{/if}
				{#if meta.needsArgs}
					<li>The arguments after the program name are an example. Try your own.</li>
				{/if}
				{#if ncurses}
					<li>
						If linking fails with <code class="font-mono text-sm">-lncurses</code>, try
						<code class="font-mono text-sm">-lncursesw</code>.
					</li>
				{/if}
			</ul>
		</div>
	</div>

	{#if notesHtml}
		<div class="prose mt-6 max-w-prose">
			<!-- eslint-disable-next-line svelte/no-at-html-tags -- rendered from our own chapter at build time -->
			{@html notesHtml}
		</div>
	{/if}
</section>

{#snippet block(title: string, lines: string[])}
	<div>
		<div class="mb-1.5 flex items-center justify-between">
			<p class="text-sm font-medium">{title}</p>
		</div>
		<div class="flex items-start gap-2 rounded-lg border border-code-line bg-code py-3 pr-2 pl-4">
			<!-- A scrollable region must be reachable by keyboard (WCAG 2.1.1). -->
			<!-- svelte-ignore a11y_no_noninteractive_tabindex -->
			<pre
				tabindex="0"
				aria-label="{title} commands"
				class="min-w-0 flex-1 overflow-x-auto font-mono text-[0.8125rem] leading-relaxed text-code-ink"><code
					>{#each lines as line, i (i)}<span class="text-code-muted select-none"
							>$ </span>{`${line}\n`}{/each}</code
				></pre>
			<CopyButton getText={() => lines.join('\n') + '\n'} label="Copy" />
		</div>
	</div>
{/snippet}
