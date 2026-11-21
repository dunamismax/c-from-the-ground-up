import { mkdtempSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import path from 'node:path';
import { describe, expect, it } from 'vitest';
import { checkHealth, readRevision } from './health.js';

const COMMIT = '0123456789abcdef0123456789abcdef01234567';

describe('checkHealth', () => {
	it('reports ok and the revision when the ping resolves', async () => {
		const report = await checkHealth(() => Promise.resolve({ rows: [{ '?column?': 1 }] }), COMMIT);
		// The deploy compares this JSON, key order included.
		expect(JSON.stringify(report)).toBe(`{"status":"ok","revision":"${COMMIT}","database":"ok"}`);
	});

	it('reports an error when the ping rejects', async () => {
		expect(
			await checkHealth(() => Promise.reject(new Error('connection refused')), COMMIT)
		).toEqual({ status: 'error', revision: COMMIT, database: 'error' });
	});

	it('reports an error when the ping hangs past the timeout', async () => {
		const report = await checkHealth(() => new Promise(() => {}), COMMIT, 20);
		expect(report.status).toBe('error');
	});
});

describe('readRevision', () => {
	it('reads the REVISION file without its newline', () => {
		const file = path.join(mkdtempSync(path.join(tmpdir(), 'revision-')), 'REVISION');
		writeFileSync(file, `${COMMIT}\n`);
		expect(readRevision(file)).toBe(COMMIT);
	});

	it('says unknown when there is no REVISION file', () => {
		expect(readRevision(path.join(tmpdir(), 'no-such-dir', 'REVISION'))).toBe('unknown');
	});
});
