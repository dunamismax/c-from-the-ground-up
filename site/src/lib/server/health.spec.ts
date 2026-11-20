import { describe, expect, it } from 'vitest';
import { checkHealth } from './health.js';

describe('checkHealth', () => {
	it('reports ok when the ping resolves', async () => {
		expect(await checkHealth(() => Promise.resolve([{ '?column?': 1 }]))).toEqual({
			status: 'ok',
			database: 'ok'
		});
	});

	it('reports an error when the ping rejects', async () => {
		expect(await checkHealth(() => Promise.reject(new Error('connection refused')))).toEqual({
			status: 'error',
			database: 'error'
		});
	});

	it('reports an error when the ping hangs past the timeout', async () => {
		const report = await checkHealth(() => new Promise(() => {}), 20);
		expect(report.status).toBe('error');
	});
});
