<script lang="ts">
	import PageMeta from '#lib/components/PageMeta.svelte';
	import { GITHUB_URL } from '#lib/site.js';
	import type { PageProps } from './$types';

	let { data }: PageProps = $props();

	const toc = [
		{ id: 'what-you-need', text: 'What you need' },
		{ id: 'macos', text: 'macOS' },
		{ id: 'linux', text: 'Linux' },
		{ id: 'windows', text: 'Windows (WSL)' },
		{ id: 'get-the-lessons', text: 'Get the lesson files' },
		{ id: 'first-program', text: 'Your first program' },
		{ id: 'the-flags', text: 'What the flags mean' },
		{ id: 'troubleshooting', text: 'Troubleshooting' }
	];
</script>

<PageMeta
	title="Setup guide"
	description="Install a C compiler on macOS, Linux or Windows (WSL), get the lesson files, and compile and run your first program."
/>

{#snippet shell(lines: string[])}
	<!-- A scrollable region must be reachable by keyboard (WCAG 2.1.1). -->
	<!-- svelte-ignore a11y_no_noninteractive_tabindex -->
	<pre class="shiki" tabindex="0"><code
			>{#each lines as line (line)}<span class="text-code-muted select-none"
					>$ </span>{`${line}\n`}{/each}</code
		></pre>
{/snippet}

<div
	class="mx-auto max-w-7xl px-4 py-12 sm:px-6 xl:grid xl:grid-cols-[minmax(0,1fr)_13rem] xl:gap-10"
>
	<article class="min-w-0">
		<header class="max-w-prose">
			<p class="font-mono text-sm font-semibold text-ember">Before lesson 1</p>
			<h1 class="mt-2 text-title font-bold">Setup guide</h1>
			<p class="mt-4 text-lg text-muted">
				Fifteen minutes, once. You will install a C compiler, get the lesson files and run your
				first program from a terminal.
			</p>
		</header>

		<div class="prose mt-10 max-w-prose">
			<h2 id="what-you-need">What you need</h2>
			<ul>
				<li>
					A <strong>C compiler</strong>. Clang and GCC both work; on every system below the command
					is called <code>cc</code>.
				</li>
				<li>
					A <strong>terminal</strong>, the text window where you type commands. Every lesson is
					compiled and run from one.
				</li>
				<li>
					A <strong>text editor</strong> for reading and changing the code. Any editor works; VS Code
					is a good free choice.
				</li>
			</ul>
			<p>
				Lessons 1 to 25 and 36 to 38 are portable C and run anywhere. Lessons 26 to 35 use POSIX
				system calls or the ncurses library, so they need macOS, Linux, or Linux inside Windows
				(WSL). The instructions below set all of that up.
			</p>

			<h2 id="macos">macOS</h2>
			<p>
				Open <strong>Terminal</strong> (in Applications, Utilities) and install Apple's command line
				tools. They include the Clang compiler, <code>make</code> and ncurses:
			</p>
			{@render shell(['xcode-select --install'])}
			<p>
				A window asks you to confirm the download. When it finishes, check that the compiler
				answers:
			</p>
			{@render shell(['cc --version'])}
			<p>
				On macOS, <code>gcc</code> is another name for Clang. That is fine: every lesson compiles with
				either.
			</p>

			<h2 id="linux">Linux</h2>
			<p>
				Install the compiler, <code>make</code> and the ncurses headers with your package manager.
			</p>
			<p>Debian, Ubuntu and Linux Mint:</p>
			{@render shell(['sudo apt update', 'sudo apt install build-essential libncurses-dev'])}
			<p>Fedora:</p>
			{@render shell(['sudo dnf install gcc make ncurses-devel'])}
			<p>Arch Linux:</p>
			{@render shell(['sudo pacman -S base-devel ncurses'])}
			<p>Then check that the compiler answers:</p>
			{@render shell(['cc --version'])}

			<h2 id="windows">Windows (WSL)</h2>
			<p>
				On Windows, take the course inside the <strong>Windows Subsystem for Linux</strong>: a real
				Ubuntu that runs next to Windows. Every lesson works there, including the POSIX and ncurses
				ones. Open <strong>PowerShell</strong> as administrator and run:
			</p>
			{@render shell(['wsl --install'])}
			<p>
				Restart when asked, then open <strong>Ubuntu</strong> from the Start menu and choose a user name
				and password. From then on you are on Linux: follow the Debian and Ubuntu steps above.
			</p>
			<p>
				Your Windows drives appear inside Ubuntu under <code>/mnt/</code> (for example
				<code>/mnt/c/Users/you</code>), but keep the lessons in your Ubuntu home folder, where
				compiling is faster. VS Code with its WSL extension can edit files there directly.
			</p>

			<h2 id="get-the-lessons">Get the lesson files</h2>
			<p>
				Every lesson page has a <strong>Download</strong> button on each source file. To get the whole
				course at once, clone the repository with git:
			</p>
			{@render shell([`git clone ${GITHUB_URL}.git`, 'cd c-from-the-ground-up'])}
			<p>The folder names contain spaces, so put quotes around them when you change into one:</p>
			{@render shell([`cd "${data.first.folder}"`])}

			<h2 id="first-program">Your first program</h2>
			<p>
				In the folder that holds <code>{data.first.file}</code>, compile it, then run the program
				the compiler made:
			</p>
			{@render shell([...data.first.commands.build, ...data.first.commands.run])}
			<p>
				If the terminal prints <code>Hello, World!</code>, you are ready. Head to
				<a href="/lessons/{data.first.slug}">Lesson 1: {data.first.title}</a> to learn what each line
				of that program does.
			</p>

			<h2 id="the-flags">What the flags mean</h2>
			<ul>
				<li><code>cc</code> runs the C compiler.</li>
				<li>
					<code>-Wall -Wextra</code> turn on warnings. A warning is the compiler pointing at code that
					is legal but probably wrong. Read every one; the course's own code compiles with none.
				</li>
				<li>
					<code>-std={data.standard}</code> picks the version of the C language standard to follow.
				</li>
				<li>
					<code>-o name</code> names the program the compiler writes. Without it, you get
					<code>a.out</code>.
				</li>
				<li>
					<code>./name</code> runs a program from the current folder. The <code>./</code> is needed because
					the shell does not look for programs there by default.
				</li>
			</ul>

			<h2 id="troubleshooting">Troubleshooting</h2>
			<dl>
				<dt><code>cc: command not found</code></dt>
				<dd>The compiler is not installed yet. Repeat the steps for your system above.</dd>
				<dt><code>No such file or directory</code></dt>
				<dd>
					You are not in the folder that holds the lesson file. Run <code>ls</code> to see what is
					there, and <code>cd</code> into the right folder.
				</dd>
				<dt><code>undefined reference to `pthread_create`</code> (or to <code>sqrt</code>)</dt>
				<dd>
					The lesson needs a library. Use the exact command on its lesson page; it includes
					<code>-pthread</code>, <code>-lm</code> or <code>-lncurses</code> where needed.
				</dd>
				<dt><code>cannot find -lncurses</code> or <code>ncurses.h: No such file</code></dt>
				<dd>
					Install the ncurses development package (<code>libncurses-dev</code> or
					<code>ncurses-devel</code>). Some systems call the library <code>ncursesw</code>: try
					<code>-lncursesw</code>.
				</dd>
				<dt>The program waits and nothing happens</dt>
				<dd>
					It is probably waiting for you to type something. Interactive lessons are marked on their
					page. Press <kbd>Ctrl</kbd>+<kbd>C</kbd> to stop any program.
				</dd>
			</dl>
		</div>
	</article>

	<aside class="hidden xl:block">
		<nav aria-labelledby="setup-toc" class="sticky top-14 py-8 text-sm">
			<p id="setup-toc" class="mb-3 font-semibold">On this page</p>
			<ul class="space-y-1.5 border-l border-line">
				{#each toc as entry (entry.id)}
					<li>
						<a
							class="-ml-px block border-l-2 border-transparent py-0.5 pl-3 text-muted hover:text-ink"
							href="#{entry.id}"
						>
							{entry.text}
						</a>
					</li>
				{/each}
			</ul>
		</nav>
	</aside>
</div>
