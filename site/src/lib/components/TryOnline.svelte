<script lang="ts">
	import { COMPILER_NAME, shortLink } from '#lib/compiler-explorer.js';

	// Opens the lesson in Compiler Explorer with its flags and example
	// arguments, ready to compile and run.
	let { fileHref, options, args }: { fileHref: string; options: string; args: string } = $props();

	let state = $state<'idle' | 'opening' | 'failed'>('idle');

	async function open() {
		// Open the tab now, while the click still counts as a user action, so
		// pop-up blockers allow it; point it at Compiler Explorer once the link
		// is ready.
		const tab = window.open('', '_blank');
		if (tab) tab.opener = null;
		state = 'opening';
		try {
			const response = await fetch(fileHref);
			if (!response.ok) throw new Error(String(response.status));
			const url = await shortLink({ source: await response.text(), options, args });
			if (tab) tab.location.href = url;
			else window.location.href = url;
			state = 'idle';
		} catch {
			tab?.close();
			state = 'failed';
		}
	}
</script>

<div class="flex flex-wrap items-center gap-x-4 gap-y-2">
	<button
		type="button"
		class="inline-flex items-center gap-2 rounded-lg bg-accent px-4 py-2 text-sm font-semibold text-on-accent hover:bg-accent-hover disabled:opacity-70"
		disabled={state === 'opening'}
		onclick={open}
	>
		<svg class="size-4" viewBox="0 0 24 24" fill="currentColor" aria-hidden="true">
			<path d="M8 5v14l11-7z" />
		</svg>
		{state === 'opening' ? 'Opening Compiler Explorer…' : 'Try it online'}
	</button>
	<p class="text-sm text-muted" aria-live="polite">
		{#if state === 'failed'}
			Compiler Explorer did not answer. Try again, or compile it on your machine below.
		{:else}
			Runs the lesson in <a
				class="font-medium text-accent hover:underline"
				href="https://godbolt.org/"
			>
				Compiler Explorer
			</a>
			({COMPILER_NAME}), nothing to install.
		{/if}
	</p>
</div>
