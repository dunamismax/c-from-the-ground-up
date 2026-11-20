import { spawn, spawnSync, type ChildProcess } from 'node:child_process';
import { mkdtempSync, readFileSync, rmSync, writeFileSync } from 'node:fs';
import { createServer } from 'node:net';
import { tmpdir } from 'node:os';
import { join } from 'node:path';
import { expect, test } from '@playwright/test';

// The client address behind the production proxy (BUILD.md, W5). These tests
// start their own `node build` with the proxy settings from the systemd unit,
// so they check what production runs rather than a copy of it.

const UNIT = new URL('../../../../deploy/cfromthegroundup.service', import.meta.url);

/** ADDRESS_HEADER, XFF_DEPTH, ... from the unit's Environment= lines. */
function unitEnvironment(): Record<string, string> {
	const env: Record<string, string> = {};
	for (const line of readFileSync(UNIT, 'utf8').split('\n')) {
		if (!line.startsWith('Environment=')) continue;
		for (const pair of line.slice('Environment='.length).trim().split(/\s+/)) {
			const [key, ...value] = pair.split('=');
			env[key] = value.join('=');
		}
	}
	return env;
}

/** The environment minus any account settings, so accounts are off. */
function withoutAccounts(env: NodeJS.ProcessEnv): NodeJS.ProcessEnv {
	const rest = { ...env };
	for (const key of [
		'BETTER_AUTH_SECRET',
		'BETTER_AUTH_URL',
		'GITHUB_CLIENT_ID',
		'GITHUB_CLIENT_SECRET'
	]) {
		delete rest[key];
	}
	return rest;
}

function freePort(): Promise<number> {
	return new Promise((resolve, reject) => {
		const server = createServer();
		server.once('error', reject);
		server.listen(0, '127.0.0.1', () => {
			const address = server.address();
			server.close(() => resolve(typeof address === 'object' && address ? address.port : 0));
		});
	});
}

async function waitFor(url: string) {
	for (let attempt = 0; attempt < 100; attempt++) {
		try {
			await fetch(url);
			return;
		} catch {
			await new Promise((resolve) => setTimeout(resolve, 100));
		}
	}
	throw new Error(`${url} did not come up`);
}

async function addressAt(url: string, headers: Record<string, string> = {}) {
	const response = await fetch(url, { headers });
	expect(response.status).toBe(200);
	expect(response.headers.get('cache-control')).toBe('no-store');
	return ((await response.json()) as { address: string }).address;
}

const unit = unitEnvironment();
let app: ChildProcess | undefined;
let appPort = 0;

test.beforeAll(async () => {
	appPort = await freePort();
	app = spawn(process.execPath, ['build'], {
		env: {
			...withoutAccounts(process.env),
			ADDRESS_HEADER: unit.ADDRESS_HEADER,
			XFF_DEPTH: unit.XFF_DEPTH,
			HOST: '127.0.0.1',
			PORT: String(appPort),
			// Nothing tested here touches the database.
			DATABASE_URL: 'postgres://none@127.0.0.1:1/none'
		},
		stdio: 'ignore'
	});
	await waitFor(`http://127.0.0.1:${appPort}/healthz/address`);
});

test.afterAll(() => {
	app?.kill();
});

test('the unit trusts exactly one proxy hop of X-Forwarded-For', () => {
	expect(unit.ADDRESS_HEADER).toBe('X-Forwarded-For');
	expect(unit.XFF_DEPTH).toBe('1');
});

test('the app takes the address Caddy appended, not what the client claimed', async () => {
	// Caddy is the only proxy: whatever the client sent, the right-most entry
	// is the address Caddy saw.
	const url = `http://127.0.0.1:${appPort}/healthz/address`;
	expect(await addressAt(url, { 'x-forwarded-for': '198.51.100.23' })).toBe('198.51.100.23');
	expect(await addressAt(url, { 'x-forwarded-for': '203.0.113.7, 198.51.100.23' })).toBe(
		'198.51.100.23'
	);
});

test('without account settings, accounts are off and the API says so', async () => {
	// This server has no BETTER_AUTH_* or GITHUB_* variables, like production
	// before a GitHub app is set up.
	const base = `http://127.0.0.1:${appPort}`;
	const headers = { 'x-forwarded-for': '198.51.100.23' };
	const state = await fetch(`${base}/api/account`, { headers });
	expect(await state.json()).toEqual({ enabled: false, user: null });
	expect((await fetch(`${base}/api/progress`, { headers })).status).toBe(401);
	expect((await fetch(`${base}/api/auth/get-session`, { headers })).status).toBe(404);
});

// Through a real Caddy, when one is installed (CI has none; cutover.sh repeats
// this check through the production Caddy).
const caddy = spawnSync('caddy', ['version'], { stdio: 'ignore' }).status === 0;

test('through Caddy, a spoofed X-Forwarded-For is ignored', async () => {
	test.skip(!caddy, 'caddy is not installed');
	const caddyPort = await freePort();
	const dir = mkdtempSync(join(tmpdir(), 'cftgu-caddy-'));
	const caddyfile = join(dir, 'Caddyfile');
	writeFileSync(
		caddyfile,
		`{\n\tadmin off\n\tpersist_config off\n\tauto_https off\n}\n` +
			`http://127.0.0.1:${caddyPort} {\n\treverse_proxy 127.0.0.1:${appPort}\n}\n`
	);
	const proxy = spawn('caddy', ['run', '--config', caddyfile, '--adapter', 'caddyfile'], {
		env: { ...process.env, XDG_DATA_HOME: dir, XDG_CONFIG_HOME: dir, HOME: dir },
		stdio: 'ignore'
	});
	try {
		const url = `http://127.0.0.1:${caddyPort}/healthz/address`;
		await waitFor(url);
		expect(await addressAt(url)).toBe('127.0.0.1');
		expect(await addressAt(url, { 'x-forwarded-for': '203.0.113.7' })).toBe('127.0.0.1');
	} finally {
		proxy.kill();
		rmSync(dir, { recursive: true, force: true });
	}
});
