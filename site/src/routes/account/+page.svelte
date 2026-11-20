<script lang="ts">
	import { onMount } from 'svelte';
	import PageMeta from '#lib/components/PageMeta.svelte';
	import ProgressBar from '#lib/components/ProgressBar.svelte';
	import { account } from '#lib/account/account.svelte.js';
	import { progress } from '#lib/progress/progress.svelte.js';
	import type { PageProps } from './$types';

	let { data }: PageProps = $props();
	const overall = $derived(progress.progressFor(data.slugs));
	let confirmingDelete = $state(false);
	let busy = $state(false);
	// Better Auth sends the learner back here with ?error=... if GitHub sign-in
	// fails or is cancelled. The page is prerendered, so read it in the browser.
	let signInFailed = $state(false);

	onMount(() => {
		signInFailed = new URLSearchParams(window.location.search).has('error');
	});

	async function run(action: () => Promise<void>) {
		busy = true;
		await action();
		busy = false;
		confirmingDelete = false;
	}

	const button = 'rounded-lg px-4 py-2 text-sm font-semibold disabled:cursor-not-allowed';
</script>

<PageMeta
	title="Your account"
	description="Sign in with GitHub to keep your progress through the course on every device."
/>

<div class="mx-auto max-w-prose px-4 py-12 sm:px-6">
	<h1 class="text-title font-bold">Your account</h1>

	<div class="mt-8" aria-live="polite">
		{#if account.status === 'loading'}
			<p class="text-muted">Checking whether you are signed in...</p>
		{:else if account.status === 'unavailable'}
			<p>
				Accounts are not available right now. You do not need one: the whole course works without
				signing in, and your progress is saved in this browser.
			</p>
		{:else if account.status === 'signed-out'}
			<p class="text-lg">
				Sign in to keep your progress on every device. Without an account, it is saved in this
				browser only.
			</p>
			<p class="mt-3 text-muted">
				Signing in uses your GitHub account. We keep your name, email address and which lessons you
				have finished, and nothing else. See the <a
					class="font-medium text-accent hover:underline"
					href="/privacy"
				>
					privacy policy
				</a>.
			</p>
			{#if signInFailed}
				<p class="mt-4 rounded-lg border border-line bg-sunken p-3 text-sm" role="alert">
					Signing in did not finish. You can try again.
				</p>
			{/if}
			<button
				type="button"
				class="mt-6 inline-flex items-center gap-2 bg-ink text-canvas hover:opacity-90 {button}"
				disabled={busy}
				onclick={() => run(() => account.signIn('/account'))}
			>
				<svg class="size-5" viewBox="0 0 24 24" fill="currentColor" aria-hidden="true">
					<path
						d="M12 .5a11.5 11.5 0 0 0-3.64 22.41c.58.1.79-.25.79-.56v-2c-3.2.7-3.88-1.37-3.88-1.37-.52-1.33-1.28-1.69-1.28-1.69-1.05-.71.08-.7.08-.7 1.16.08 1.77 1.19 1.77 1.19 1.03 1.77 2.7 1.26 3.36.96.1-.75.4-1.26.73-1.55-2.55-.29-5.24-1.28-5.24-5.68 0-1.26.45-2.28 1.19-3.09-.12-.29-.52-1.46.11-3.05 0 0 .97-.31 3.17 1.18a11 11 0 0 1 5.77 0c2.2-1.49 3.17-1.18 3.17-1.18.63 1.59.23 2.76.11 3.05.74.81 1.19 1.83 1.19 3.09 0 4.41-2.69 5.38-5.26 5.67.41.36.78 1.06.78 2.14v3.17c0 .31.21.67.8.56A11.5 11.5 0 0 0 12 .5Z"
					/>
				</svg>
				Sign in with GitHub
			</button>
			{#if progress.ready && overall.done > 0}
				<p class="mt-6 text-sm text-muted">
					The {overall.done} lesson{overall.done === 1 ? '' : 's'} you finished in this browser will be
					added to your account when you sign in.
				</p>
			{/if}
		{:else if account.user}
			<p class="text-lg">
				Signed in as <strong>{account.user.name}</strong>
				<span class="text-muted">({account.user.email})</span>.
			</p>
			<p class="mt-2 text-muted">Your progress is saved to your account on every device.</p>
			{#if progress.ready}
				<ProgressBar class="mt-6" done={overall.done} total={overall.total} label="Your progress" />
			{/if}

			<div class="mt-10 flex flex-wrap gap-3">
				<a
					class="border border-line-strong bg-raised hover:border-ink {button}"
					href="/api/account/export"
					download="cfromthegroundup-account.json"
				>
					Download your data
				</a>
				<button
					type="button"
					class="border border-line-strong bg-raised hover:border-ink {button}"
					disabled={busy}
					onclick={() => run(() => account.signOut())}
				>
					Sign out
				</button>
			</div>

			<section class="mt-12 rounded-xl border border-line p-5" aria-labelledby="delete-heading">
				<h2 id="delete-heading" class="font-semibold">Delete your account</h2>
				<p class="mt-1 text-sm text-muted">
					This removes your account and the progress saved in it, straight away. It cannot be
					undone. You can keep taking the course without an account.
				</p>
				{#if confirmingDelete}
					<p class="mt-4 text-sm font-medium" role="alert">Delete your account for good?</p>
					<div class="mt-3 flex flex-wrap gap-3">
						<button
							type="button"
							class="bg-danger text-canvas hover:opacity-90 {button}"
							disabled={busy}
							onclick={() => run(() => account.deleteAccount())}
						>
							Yes, delete my account
						</button>
						<button
							type="button"
							class="border border-line-strong bg-raised hover:border-ink {button}"
							onclick={() => (confirmingDelete = false)}
						>
							Cancel
						</button>
					</div>
				{:else}
					<button
						type="button"
						class="mt-4 border border-line-strong bg-raised hover:border-ink {button}"
						onclick={() => (confirmingDelete = true)}
					>
						Delete account...
					</button>
				{/if}
			</section>
		{/if}

		{#if account.error}
			<p class="mt-6 rounded-lg border border-line bg-sunken p-3 text-sm" role="alert">
				{account.error}
			</p>
		{/if}
	</div>
</div>
