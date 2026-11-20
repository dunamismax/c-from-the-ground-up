<script lang="ts">
	import type { SourceFile } from '#lib/content/types.js';
	import CopyButton from './CopyButton.svelte';

	// A full lesson source listing: file name bar, line numbers (CSS), copy,
	// download and a link to the file on GitHub. The highlighted HTML is
	// produced at build time by Shiki.
	let { source, downloadHref }: { source: Omit<SourceFile, 'contents'>; downloadHref: string } =
		$props();

	let body: HTMLElement | undefined = $state();

	function text(): string {
		// Read the code back from the page instead of shipping it twice.
		const lines = body?.querySelectorAll('.line') ?? [];
		return Array.from(lines, (line) => line.textContent ?? '').join('\n') + '\n';
	}
</script>

<figure
	id={source.id}
	class="listing not-prose my-8 overflow-hidden rounded-xl border border-code-line bg-code text-code-ink shadow-sm"
>
	<figcaption
		class="flex flex-wrap items-center gap-x-3 gap-y-1 border-b border-code-line bg-code-bar px-4 py-2"
	>
		<span class="flex items-center gap-2 font-mono text-sm font-semibold">
			<svg
				class="size-4 text-code-muted"
				viewBox="0 0 24 24"
				fill="none"
				stroke="currentColor"
				stroke-width="2"
				aria-hidden="true"
			>
				<path d="M14 3H7a2 2 0 0 0-2 2v14a2 2 0 0 0 2 2h10a2 2 0 0 0 2-2V8Z" />
				<path d="M14 3v5h5" />
			</svg>
			{source.name}
		</span>
		<span class="text-xs text-code-muted">{source.lineCount} lines</span>
		<span class="ml-auto flex items-center gap-1">
			<CopyButton getText={text} />
			<a
				class="rounded-md px-2 py-1 text-xs font-medium text-code-muted hover:bg-white/10 hover:text-code-ink"
				href={downloadHref}
				download={source.name}
			>
				Download
			</a>
			<a
				class="rounded-md px-2 py-1 text-xs font-medium text-code-muted hover:bg-white/10 hover:text-code-ink"
				href={source.githubUrl}
			>
				View on GitHub
			</a>
		</span>
	</figcaption>
	<div bind:this={body} class="[&_pre]:px-4 [&_pre]:py-4">
		<!-- eslint-disable-next-line svelte/no-at-html-tags -- Shiki output built from our own lesson files -->
		{@html source.html}
	</div>
</figure>
