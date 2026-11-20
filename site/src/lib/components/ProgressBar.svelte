<script lang="ts">
	let {
		done,
		total,
		label,
		class: className = ''
	}: { done: number; total: number; label: string; class?: string } = $props();

	const percent = $derived(total === 0 ? 0 : Math.round((done / total) * 100));
</script>

<div class={className}>
	<div class="flex items-baseline justify-between gap-3 text-sm">
		<span class="text-muted">{label}</span>
		<span class="font-medium tabular-nums">{done} of {total}</span>
	</div>
	<div
		class="mt-1.5 h-2 overflow-hidden rounded-full bg-sunken"
		role="progressbar"
		aria-label={label}
		aria-valuemin={0}
		aria-valuemax={total}
		aria-valuenow={done}
		aria-valuetext="{done} of {total} lessons complete"
	>
		<div
			class="h-full rounded-full bg-success transition-[width] motion-reduce:transition-none"
			style:width="{percent}%"
		></div>
	</div>
</div>
