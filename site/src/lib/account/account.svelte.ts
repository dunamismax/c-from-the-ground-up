// The learner's account, as the browser sees it (milestone W6). Pages are
// prerendered, so the state is fetched from /api/account after load. Sign-in
// is GitHub OAuth through Better Auth's endpoints under /api/auth/.

import {
	accountBackend,
	browserBackend,
	clearLocalProgress,
	httpProgressApi
} from '#lib/progress/account-backend.js';
import { progress } from '#lib/progress/progress.svelte.js';

export interface AccountUser {
	id: string;
	name: string;
	email: string;
}

export type AccountStatus = 'loading' | 'unavailable' | 'signed-out' | 'signed-in';

class AccountStore {
	/** `unavailable`: accounts are not configured, or the server did not answer. */
	status = $state<AccountStatus>('loading');
	user = $state<AccountUser | null>(null);
	/** Set when an action failed, for the account page to show. */
	error = $state<string | null>(null);

	/** Fetch the account state and connect progress to it. Call once, in the browser. */
	async load() {
		try {
			const response = await fetch('/api/account', { headers: { accept: 'application/json' } });
			if (!response.ok) throw new Error(String(response.status));
			const state = (await response.json()) as { enabled: boolean; user: AccountUser | null };
			this.user = state.user;
			this.status = !state.enabled ? 'unavailable' : state.user ? 'signed-in' : 'signed-out';
		} catch {
			this.status = 'unavailable';
		}
		if (this.status === 'signed-in' && this.user) {
			progress.connect(
				accountBackend({
					storage: window.localStorage,
					target: window,
					userId: this.user.id,
					api: httpProgressApi()
				})
			);
		}
	}

	/** Start GitHub sign-in; comes back to `returnTo` (a path on this site). */
	async signIn(returnTo = '/account') {
		this.error = null;
		try {
			const response = await fetch('/api/auth/sign-in/social', {
				method: 'POST',
				headers: { 'content-type': 'application/json' },
				body: JSON.stringify({
					provider: 'github',
					callbackURL: returnTo,
					errorCallbackURL: '/account'
				})
			});
			const result = (await response.json()) as { url?: string };
			if (!response.ok || !result.url) throw new Error(String(response.status));
			window.location.assign(result.url);
		} catch {
			this.error = 'Could not start signing in. Try again in a moment.';
		}
	}

	async signOut() {
		this.error = null;
		const response = await fetch('/api/auth/sign-out', {
			method: 'POST',
			headers: { 'content-type': 'application/json' },
			body: '{}'
		}).catch(() => null);
		if (!response?.ok) {
			this.error = 'Could not sign out. Try again in a moment.';
			return;
		}
		this.#forget();
	}

	async deleteAccount() {
		this.error = null;
		const response = await fetch('/api/account', { method: 'DELETE' }).catch(() => null);
		if (!response?.ok) {
			this.error = 'Could not delete the account. Try again in a moment.';
			return;
		}
		this.#forget();
	}

	// Signed out: this browser forgets the account's progress (it is safe in
	// the account) and goes back to browser-only progress.
	#forget() {
		clearLocalProgress(window.localStorage);
		this.user = null;
		this.status = 'signed-out';
		progress.connect(browserBackend(window.localStorage, window));
	}
}

export const account = new AccountStore();
