export interface HealthReport {
	status: 'ok' | 'error';
	database: 'ok' | 'error';
}

/**
 * Run `ping` (a trivial database query) and report whether it succeeded within
 * `timeoutMs`. Never throws: a failure or a timeout becomes an `error` report.
 */
export async function checkHealth(
	ping: () => Promise<unknown>,
	timeoutMs = 3000
): Promise<HealthReport> {
	let timer: ReturnType<typeof setTimeout> | undefined;
	const timeout = new Promise<never>((_, reject) => {
		timer = setTimeout(() => reject(new Error('database ping timed out')), timeoutMs);
	});

	try {
		await Promise.race([ping(), timeout]);
		return { status: 'ok', database: 'ok' };
	} catch {
		return { status: 'error', database: 'error' };
	} finally {
		clearTimeout(timer);
	}
}
