// Zip archives of lesson files, built at build time with fflate. Entries get a
// fixed timestamp so the same content always produces the same archive.

import { zipSync, strToU8, type Zippable } from 'fflate';

const FIXED_DATE = new Date('2026-01-01T00:00:00Z');

/** Build a zip from `path -> text` entries; paths use forward slashes. */
export function zipFiles(files: Record<string, string>): Uint8Array {
	const entries: Zippable = {};
	for (const [path, text] of Object.entries(files).sort(([a], [b]) => a.localeCompare(b))) {
		entries[path] = [strToU8(text), { mtime: FIXED_DATE, level: 9 }];
	}
	return zipSync(entries);
}
