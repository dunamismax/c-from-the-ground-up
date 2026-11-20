<script lang="ts">
	// The mark: a C standing on two layers of ground. Colors follow the theme.
	let { class: className = 'size-8' }: { class?: string } = $props();
</script>

<svg class={className} viewBox="0 0 32 32" aria-hidden="true" focusable="false">
	<rect width="32" height="32" rx="7" class="fill-accent" />
	<path
		d="M21.8 8.2A7.5 7.5 0 1 0 21.8 17.8"
		fill="none"
		class="stroke-on-accent"
		stroke-width="3.5"
		stroke-linecap="round"
	/>
	<rect x="7" y="24" width="18" height="2.25" rx="1.1" class="fill-on-accent" />
	<rect x="10" y="27.5" width="12" height="1.75" rx="0.9" class="fill-ember" />
</svg>
