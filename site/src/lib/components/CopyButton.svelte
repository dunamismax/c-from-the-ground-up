<script lang="ts">
	// Copies text to the clipboard and says so, visibly and to screen readers.
	let {
		getText,
		label = 'Copy',
		class: className = ''
	}: { getText: () => string; label?: string; class?: string } = $props();

	let state = $state<'idle' | 'copied' | 'failed'>('idle');
	let timer: ReturnType<typeof setTimeout> | undefined;

	async function copy() {
		try {
			await navigator.clipboard.writeText(getText());
			state = 'copied';
		} catch {
			state = 'failed';
		}
		clearTimeout(timer);
		timer = setTimeout(() => (state = 'idle'), 2000);
	}
</script>

<button
	type="button"
	class="inline-flex items-center gap-1.5 rounded-md px-2 py-1 text-xs font-medium text-code-muted hover:bg-white/10 hover:text-code-ink {className}"
	onclick={copy}
>
	<svg
		class="size-3.5"
		viewBox="0 0 24 24"
		fill="none"
		stroke="currentColor"
		stroke-width="2"
		aria-hidden="true"
	>
		{#if state === 'copied'}
			<path d="M20 6 9 17l-5-5" stroke-linecap="round" stroke-linejoin="round" />
		{:else}
			<rect x="9" y="9" width="12" height="12" rx="2" />
			<path d="M5 15H4a1 1 0 0 1-1-1V4a1 1 0 0 1 1-1h10a1 1 0 0 1 1 1v1" />
		{/if}
	</svg>
	<span aria-live="polite">
		{state === 'copied' ? 'Copied' : state === 'failed' ? 'Copy failed' : label}
	</span>
</button>
