import { existsSync } from 'node:fs';
import path from 'node:path';

export const GITHUB_REPO = 'https://github.com/dunamismax/c-from-the-ground-up';
const BRANCH = 'main';

/**
 * The repository root: the nearest directory at or above the working directory
 * that holds `book.toml` and `src/SUMMARY.md`. Builds and tests run in `site/`,
 * so this is normally `..`. Course content is only read at build time.
 */
export function courseRoot(start: string = process.cwd()): string {
	const root = findCourseRoot(start);
	if (root === null) {
		throw new Error(`cannot find the course root (book.toml and src/SUMMARY.md) above ${start}`);
	}
	return root;
}

/** Like courseRoot(), but returns null when there is no course above `start`. */
export function findCourseRoot(start: string = process.cwd()): string | null {
	let dir = path.resolve(start);
	for (;;) {
		if (existsSync(path.join(dir, 'book.toml')) && existsSync(path.join(dir, 'src/SUMMARY.md'))) {
			return dir;
		}
		const parent = path.dirname(dir);
		if (parent === dir) return null;
		dir = parent;
	}
}

function encodePath(repoPath: string): string {
	return repoPath.split('/').map(encodeURIComponent).join('/');
}

export function githubBlobUrl(repoPath: string): string {
	return `${GITHUB_REPO}/blob/${BRANCH}/${encodePath(repoPath)}`;
}

export function githubRawUrl(repoPath: string): string {
	return `${GITHUB_REPO}/raw/${BRANCH}/${encodePath(repoPath)}`;
}
