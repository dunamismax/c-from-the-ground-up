<script lang="ts">
	import { page } from '$app/state';
	import { SITE_DESCRIPTION, SITE_NAME, SITE_URL } from '#lib/site.js';

	// Per-page <title>, description and canonical URL. (W7 adds Open Graph
	// images and structured data.)
	let { title, description = SITE_DESCRIPTION }: { title?: string; description?: string } =
		$props();

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
</svelte:head>
