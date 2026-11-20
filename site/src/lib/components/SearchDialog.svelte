<script lang="ts">
	import { goto } from '$app/navigation';
	import { resolve } from '$app/paths';
	import { searchUi } from '#lib/search-ui.svelte.js';
	import type MiniSearch from 'minisearch';
	import {
		INDEX_OPTIONS,
		QUERY_OPTIONS,
		resultHref,
		SEARCH_INDEX_PATH,
		type SearchDocument,
		type SearchResultFields
	} from '#lib/search.js';

	// Site search in a modal dialog. The index (built at build time) and the
	// search library load the first time the dialog opens.
	let dialog: HTMLDialogElement | undefined = $state();
	let input: HTMLInputElement | undefined = $state();
	let index: MiniSearch<SearchDocument> | null = null;
	let status = $state<'idle' | 'loading' | 'ready' | 'failed'>('idle');
	let query = $state('');
	let active = $state(0);

	type Result = SearchResultFields & { href: string };
	let results = $state<Result[]>([]);

	async function loadIndex() {
		if (status === 'loading' || status === 'ready') return;
		status = 'loading';
		try {
			const [{ default: MiniSearchClass }, response] = await Promise.all([
				import('minisearch'),
				fetch(SEARCH_INDEX_PATH)
			]);
			if (!response.ok) throw new Error(String(response.status));
			index = MiniSearchClass.loadJSON<SearchDocument>(await response.text(), INDEX_OPTIONS);
			status = 'ready';
			search();
		} catch {
			status = 'failed';
		}
	}

	function search() {
		active = 0;
		const text = query.trim();
		if (!index || text.length < 2) {
			results = [];
			return;
		}
		results = index
			.search(text, QUERY_OPTIONS)
			.slice(0, 12)
			.map((hit) => {
				const fields = hit as unknown as SearchResultFields;
				return {
					...fields,
					id: String(hit.id),
					href: resultHref({ slug: fields.slug, id: String(hit.id) })
				};
			});
	}

	// searchUi.open drives the dialog; closing it (Esc, the button, a click
	// outside, choosing a result) sets it back.
	$effect(() => {
		if (!dialog) return;
		if (searchUi.open && !dialog.open) {
			dialog.showModal();
			input?.select();
			void loadIndex();
		} else if (!searchUi.open && dialog.open) {
			dialog.close();
		}
	});

	function close() {
		searchUi.open = false;
	}

	async function choose(result: Result) {
		close();
		const fragment = result.id.includes('#') ? result.id.slice(result.id.indexOf('#') + 1) : '';
		await goto(
			fragment
				? resolve(`/lessons/[slug]#${fragment}`, { slug: result.slug })
				: resolve('/lessons/[slug]', { slug: result.slug })
		);
	}

	function onKeydown(event: KeyboardEvent) {
		if (event.key === 'ArrowDown' && results.length) {
			event.preventDefault();
			active = (active + 1) % results.length;
		} else if (event.key === 'ArrowUp' && results.length) {
			event.preventDefault();
			active = (active - 1 + results.length) % results.length;
		} else if (event.key === 'Enter' && results[active]) {
			event.preventDefault();
			void choose(results[active]);
		}
	}

	$effect(() => {
		document.getElementById(`search-result-${active}`)?.scrollIntoView({ block: 'nearest' });
	});
</script>

<dialog
	bind:this={dialog}
	aria-labelledby="search-title"
	onclose={close}
	class="m-0 mx-auto mt-[10vh] w-[min(40rem,calc(100vw-2rem))] max-w-none rounded-xl border border-line bg-raised p-0 text-ink shadow-2xl backdrop:bg-black/50"
	onclick={(event) => {
		if (event.target === dialog) close();
	}}
>
	<h2 id="search-title" class="sr-only">Search the course</h2>
	<div class="flex items-center gap-3 border-b border-line px-4">
		<svg
			class="size-5 shrink-0 text-muted"
			viewBox="0 0 24 24"
			fill="none"
			stroke="currentColor"
			stroke-width="2"
			aria-hidden="true"
		>
			<circle cx="11" cy="11" r="7" /><path d="m20 20-3.5-3.5" stroke-linecap="round" />
		</svg>
		<input
			bind:this={input}
			bind:value={query}
			oninput={search}
			onkeydown={onKeydown}
			type="text"
			placeholder="Search lessons: pointers, malloc, sockets..."
			aria-label="Search lessons"
			role="combobox"
			aria-expanded={results.length > 0}
			aria-controls="search-results"
			aria-activedescendant={results.length ? `search-result-${active}` : undefined}
			aria-autocomplete="list"
			autocomplete="off"
			spellcheck="false"
			class="h-14 min-w-0 flex-1 bg-transparent text-base outline-none placeholder:text-muted"
		/>
		<button
			type="button"
			class="rounded-md border border-line px-2 py-0.5 text-xs text-muted hover:text-ink"
			onclick={close}
		>
			Esc
		</button>
	</div>

	<!-- A scrollable region must be reachable by keyboard (WCAG 2.1.1). -->
	<!-- svelte-ignore a11y_no_noninteractive_tabindex -->
	<div
		class="max-h-[60vh] overflow-y-auto p-2"
		tabindex="0"
		role="region"
		aria-label="Search results"
	>
		{#if status === 'failed'}
			<p class="px-3 py-6 text-center text-sm text-muted">
				Search could not load. Check your connection and try again.
			</p>
		{:else if query.trim().length < 2}
			<p class="px-3 py-6 text-center text-sm text-muted">
				Type at least two letters. Press <kbd class="font-mono">/</kbd> anywhere to search.
			</p>
		{:else if status !== 'ready'}
			<p class="px-3 py-6 text-center text-sm text-muted">Loading the index&hellip;</p>
		{:else if results.length === 0}
			<p class="px-3 py-6 text-center text-sm text-muted">
				No lessons mention &ldquo;{query.trim()}&rdquo;.
			</p>
		{/if}
		<ul
			id="search-results"
			role="listbox"
			aria-label="Results"
			class={results.length ? '' : 'hidden'}
		>
			{#each results as result, i (result.id)}
				<!-- The input owns the keyboard (combobox pattern); options take clicks. -->
				<!-- svelte-ignore a11y_click_events_have_key_events -->
				<li
					id="search-result-{i}"
					role="option"
					aria-selected={i === active}
					class="cursor-pointer rounded-lg px-3 py-2.5 aria-selected:bg-accent-soft"
					onclick={() => void choose(result)}
					onmousemove={() => (active = i)}
				>
					<span class="block text-xs font-medium text-muted">
						Lesson {result.number}: {result.lessonTitle}
					</span>
					<span class="block font-semibold">{result.heading || result.lessonTitle}</span>
					{#if result.excerpt}
						<span class="mt-0.5 line-clamp-2 block text-sm text-muted">{result.excerpt}</span>
					{/if}
				</li>
			{/each}
		</ul>
	</div>
	<p class="border-t border-line px-4 py-2 text-xs text-muted">
		<kbd class="font-mono">&uarr;</kbd> <kbd class="font-mono">&darr;</kbd> to choose,
		<kbd class="font-mono">Enter</kbd> to open. On a lesson, <kbd class="font-mono">j</kbd> and
		<kbd class="font-mono">k</kbd> go to the next and previous lesson.
	</p>
</dialog>
