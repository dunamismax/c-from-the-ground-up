import { readFileSync } from 'node:fs';

export interface HealthReport {
	status: 'ok' | 'error';
	revision: string;
	database: 'ok' | 'error';
}

/**
 * The commit this release was built from: the `REVISION` file the deploy
 * writes at the top of the release directory (the server's working
 * directory), or `unknown` when there is none, as in development.
 */
export function readRevision(file = 'REVISION'): string {
	try {
		return readFileSync(file, 'utf8').trim() || 'unknown';
	} catch {
		return 'unknown';
	}
}

/**
 * Run `ping` (a trivial database query) and report whether it succeeded within
 * `timeoutMs`. Never throws: a failure or a timeout becomes an `error` report.
 */
export async function checkHealth(
	ping: () => Promise<unknown>,
	revision: string,
	timeoutMs = 3000
): Promise<HealthReport> {
	let timer: ReturnType<typeof setTimeout> | undefined;
	const timeout = new Promise<never>((_, reject) => {
		timer = setTimeout(() => reject(new Error('database ping timed out')), timeoutMs);
	});

	try {
		await Promise.race([ping(), timeout]);
		return { status: 'ok', revision, database: 'ok' };
	} catch {
		return { status: 'error', revision, database: 'error' };
	} finally {
		clearTimeout(timer);
	}
}
