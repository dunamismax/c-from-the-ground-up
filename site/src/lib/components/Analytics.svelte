<script lang="ts">
	import { onMount } from 'svelte';
	import { trackerAttributes } from '#lib/analytics.js';

	// Off until UMAMI_WEBSITE_ID is set in #lib/analytics.ts. The tracker is
	// added after the page loads rather than rendered as a <script> tag, because
	// Svelte gives rendered scripts inline onload/onerror handlers, which the
	// Content Security Policy (rightly) blocks. The layout mounts once, so the
	// script is added once; it counts later client-side navigations itself.
	onMount(() => {
		const tracker = trackerAttributes();
		if (!tracker) return;
		const script = document.createElement('script');
		script.defer = true;
		for (const [name, value] of Object.entries(tracker)) script.setAttribute(name, value);
		document.head.append(script);
	});
</script>
