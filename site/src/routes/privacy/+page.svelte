<script lang="ts">
	import PageMeta from '#lib/components/PageMeta.svelte';
	import { UMAMI_WEBSITE_ID } from '#lib/analytics.js';

	const analytics = UMAMI_WEBSITE_ID !== '';
	import { GITHUB_URL } from '#lib/site.js';
	import { SESSION_DAYS } from '#lib/session.js';
</script>

<PageMeta
	title="Privacy"
	description="What C From The Ground Up stores about you, why, and how to get it or delete it. No ads, no third-party trackers."
/>

<div class="mx-auto max-w-prose px-4 py-12 sm:px-6">
	<h1 class="text-title font-bold">Privacy</h1>
	<div class="prose mt-8">
		{#if analytics}
			<p>
				C From The Ground Up has no ads and no third-party trackers, and it never sells or shares
				your data. It counts page views with its own copy of Umami, described below. You can take
				the whole course without an account.
			</p>
		{:else}
			<p>
				C From The Ground Up has no ads, no analytics and no trackers, and it never sells or shares
				your data. You can take the whole course without an account.
			</p>
		{/if}

		<h2 id="without-an-account">Without an account</h2>
		<p>
			Nothing about you is stored on our server. Your browser keeps, on this device only, your
			theme, the operating system you picked in the compile instructions, and which lessons you have
			finished. Clearing this site's data in your browser removes it.
		</p>

		<h2 id="with-an-account">With an account</h2>
		<p>
			Accounts are optional and exist only so your progress follows you between devices. You sign in
			with GitHub; we never see your GitHub password. We store:
		</p>
		<ul>
			<li>
				<strong>Your name and email address</strong>, as GitHub reports them, to show who is signed
				in. We do not send you email.
			</li>
			<li>
				<strong>Your GitHub account ID</strong> and the access token GitHub issues at sign-in, encrypted.
				The token is not used after sign-in.
			</li>
			<li>
				<strong>Your progress</strong>: which lessons you finished and when, and the lesson you
				viewed last.
			</li>
			<li>
				<strong>Your sign-in sessions</strong>: for each signed-in browser, when it signed in, the
				IP address and the browser's user agent at the time, so sessions can be secured and abuse
				limited. A session lasts {SESSION_DAYS} days unless you sign out.
			</li>
		</ul>
		<p>
			Cookies are used only to keep you signed in, plus a short-lived one that protects the step
			through GitHub while you sign in. They are only used by this site, and signing out removes
			them.
		</p>

		{#if analytics}
			<h2 id="page-statistics">Page statistics</h2>
			<p>
				To see which lessons help and where learners stop, the site counts page views with
				<a href="https://umami.is">Umami</a>, running on our own server. For each page view it
				records the page, the page you came from, your browser, operating system, device type,
				screen size, language and country. It sets no cookies and does not store your IP address,
				and nothing is shared with anyone else. If your browser sends a Do Not Track signal, nothing
				is recorded at all.
			</p>
		{/if}

		<h2 id="where">Where it is kept</h2>
		<p>
			On our own server, in a PostgreSQL database, with nightly backups on the same server that are
			kept for 30 days. Our web server also keeps access logs (IP address, page and time) for seven
			days to keep the site running.
		</p>

		<h2 id="your-choices">Getting or deleting your data</h2>
		<p>
			On your <a href="/account">account page</a> you can download everything we store about you as a
			JSON file, sign out, or delete your account. Deleting removes your account, sessions and progress
			at once; copies in backups disappear as the backups expire, within 30 days.
		</p>

		<h2 id="contact">Questions</h2>
		<p>
			Open an issue in the <a href="{GITHUB_URL}/issues">GitHub repository</a>. This policy changes
			only with the site's source code, so its history is public there too.
		</p>
	</div>
</div>
