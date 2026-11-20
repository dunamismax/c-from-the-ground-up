<script lang="ts">
	import type { LessonMeta } from '#lib/content/types.js';

	// Small labels for what a lesson needs: platform, libraries, input.
	let { meta, compact = false }: { meta: LessonMeta; compact?: boolean } = $props();

	const badges = $derived.by(() => {
		const list: { label: string; title: string; tone: 'ember' | 'accent' | 'plain' }[] = [];
		if (meta.platform === 'posix') {
			list.push({ label: 'POSIX', title: 'Uses POSIX APIs: macOS, Linux or WSL', tone: 'ember' });
		}
		if (meta.platform === 'ncurses') {
			list.push({
				label: 'ncurses',
				title: 'Needs the ncurses library: macOS, Linux or WSL',
				tone: 'ember'
			});
		}
		if (meta.libraries.includes('pthread')) {
			list.push({ label: 'threads', title: 'Links with -pthread', tone: 'accent' });
		}
		if (meta.libraries.includes('m')) {
			list.push({ label: 'libm', title: 'Links with the math library (-lm)', tone: 'accent' });
		}
		if (meta.build === 'make') {
			list.push({ label: 'make', title: 'Several files, built with make', tone: 'accent' });
		}
		if (!compact && meta.interactive) {
			list.push({ label: 'interactive', title: 'Reads your keyboard input', tone: 'plain' });
		}
		if (!compact && meta.needsArgs) {
			list.push({ label: 'arguments', title: 'Takes command-line arguments', tone: 'plain' });
		}
		return list;
	});

	const tones = {
		ember: 'bg-ember-soft text-ember',
		accent: 'bg-accent-soft text-accent',
		plain: 'bg-sunken text-muted'
	};
</script>

{#if badges.length}
	<ul class="flex flex-wrap gap-1.5" aria-label="Lesson requirements">
		{#each badges as badge (badge.label)}
			<li
				class="rounded-full px-2 py-0.5 text-xs font-medium {tones[badge.tone]}"
				title={badge.title}
			>
				{badge.label}
			</li>
		{/each}
	</ul>
{/if}
