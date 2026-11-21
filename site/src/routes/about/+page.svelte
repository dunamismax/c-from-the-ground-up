<script lang="ts">
	import PageMeta from '#lib/components/PageMeta.svelte';
	import { UMAMI_WEBSITE_ID } from '#lib/analytics.js';
	import { BOOK_URL, GITHUB_URL, LICENSE_URL } from '#lib/site.js';

	const analytics = UMAMI_WEBSITE_ID !== '';
</script>

<PageMeta
	title="About"
	description="Why C From The Ground Up exists, how it is made, and how to help. Free and open source under the MIT License."
/>

<div class="mx-auto max-w-prose px-4 py-12 sm:px-6">
	<h1 class="text-title font-bold">About the course</h1>
	<div class="prose mt-8">
		<p>
			C From The Ground Up is a free, progressive course in the C programming language. It starts
			with a program that prints one line and ends in systems programming: sockets, threads, a tiny
			shell, and a terminal text adventure you build yourself.
		</p>

		<h2 id="how-it-is-made">How it is made</h2>
		<p>
			Every lesson is a single, readable, runnable C program, and its comments are the lesson. The
			files in the repository are the only copy of the code: this site and the
			<a href={BOOK_URL}>book edition</a> are both generated from them, so what you read is always what
			compiles.
		</p>
		<p>
			Every change is checked automatically. Each lesson must compile with no warnings under both
			Clang and GCC, and lessons that read input are fed good input, bad input and end-of-file to
			make sure they do not hang or crash.
		</p>

		<h2 id="who-it-is-for">Who it is for</h2>
		<p>
			Anyone who wants to understand how programs really work: complete beginners, programmers
			coming from higher-level languages, and students who want a second explanation. Each lesson
			only uses ideas that earlier lessons taught.
		</p>

		<h2 id="privacy">Privacy</h2>
		<p>
			{#if analytics}
				There are no ads and no third-party trackers. The site counts page views with its own copy
				of Umami, which sets no cookies and stores no IP addresses.
			{:else}
				There are no ads, no analytics and no trackers.
			{/if}
			Accounts are optional: without one, your browser remembers your progress, theme and preferred operating
			system tab on this device only. The
			<a href="/privacy">privacy policy</a> says exactly what is recorded and what an account stores.
		</p>

		<h2 id="help">Help and contributing</h2>
		<p>
			Found a mistake, or a comment that confused you? A wrong comment is a bug: please
			<a href="{GITHUB_URL}/issues">open an issue on GitHub</a>. The source of the lessons, the book
			and this site is all in <a href={GITHUB_URL}>one repository</a>.
		</p>

		<h2 id="license">License</h2>
		<p>
			Written by <a href="https://github.com/dunamismax">dunamismax</a> and released under the
			<a href={LICENSE_URL}>MIT License</a>. You may use, copy and adapt it, including for teaching.
		</p>
	</div>
</div>
