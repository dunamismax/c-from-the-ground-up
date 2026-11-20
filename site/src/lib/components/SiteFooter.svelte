<script lang="ts">
	import { BOOK_URL, GITHUB_URL, LICENSE_URL } from '#lib/site.js';
</script>

<footer class="mt-24 border-t border-line bg-sunken/50">
	<div
		class="mx-auto flex max-w-7xl flex-col gap-6 px-4 py-10 text-sm text-muted sm:flex-row sm:items-start sm:justify-between sm:px-6"
	>
		<div class="max-w-sm">
			<p class="font-semibold text-ink">C From The Ground Up</p>
			<p class="mt-1">
				A free, progressive C course. Every lesson is a small, runnable program, and the comments
				are the lesson.
			</p>
		</div>
		<nav aria-label="Footer" class="grid grid-cols-2 gap-x-10 gap-y-2">
			<a class="hover:text-ink" href="/lessons">All lessons</a>
			<a class="hover:text-ink" href={GITHUB_URL}>Source on GitHub</a>
			<a class="hover:text-ink" href="/setup">Setup guide</a>
			<a class="hover:text-ink" href={BOOK_URL}>Read as a book</a>
			<a class="hover:text-ink" href="/about">About</a>
			<a class="hover:text-ink" href={LICENSE_URL}>MIT License</a>
		</nav>
	</div>
</footer>
