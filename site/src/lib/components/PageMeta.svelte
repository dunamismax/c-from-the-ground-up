<script lang="ts">
	import { page } from '$app/state';
	import { SITE_DESCRIPTION, SITE_NAME, SITE_URL } from '#lib/site.js';
	import { jsonLdScript } from '#lib/structured-data.js';

	// Per-page <title>, description, canonical URL, Open Graph tags (with a
	// share image from /og/, rendered at build time) and optional schema.org
	// structured data.
	let {
		title,
		description = SITE_DESCRIPTION,
		image = '/og/site.png',
		jsonLd
	}: { title?: string; description?: string; image?: string; jsonLd?: unknown } = $props();

	const fullTitle = $derived(title ? `${title} | ${SITE_NAME}` : SITE_NAME);
	const canonical = $derived(`${SITE_URL}${page.url.pathname === '/' ? '/' : page.url.pathname}`);
</script>

<svelte:head>
	<title>{fullTitle}</title>
	<meta name="description" content={description} />
	<link rel="canonical" href={canonical} />
	<meta property="og:title" content={fullTitle} />
	<meta property="og:description" content={description} />
	<meta property="og:url" content={canonical} />
	<meta property="og:type" content="website" />
	<meta property="og:site_name" content={SITE_NAME} />
	<meta property="og:image" content="{SITE_URL}{image}" />
	<meta property="og:image:width" content="1200" />
	<meta property="og:image:height" content="630" />
	<meta property="og:image:alt" content={fullTitle} />
	<meta name="twitter:card" content="summary_large_image" />
	{#if jsonLd}
		<!-- eslint-disable-next-line svelte/no-at-html-tags -- JSON-LD from our own data, with < escaped -->
		{@html jsonLdScript(jsonLd)}
	{/if}
</svelte:head>
