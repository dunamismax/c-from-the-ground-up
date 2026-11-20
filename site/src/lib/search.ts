// The site search: a MiniSearch index over lesson prose, one document per
// section (the text between two headings), plus one per source file holding
// its comments, which in this course are the lesson itself. It is built at build time into
// /search-index.json and loaded by the browser only when search opens.

import type { Options, SearchOptions } from 'minisearch';

export interface SearchDocument {
	/** `<slug>#<anchor>`, or just the slug for a lesson's introduction. */
	id: string;
	slug: string;
	number: number;
	/** Indexed only on a lesson's introduction, so one lesson is one title match. */
	title: string;
	lessonTitle: string;
	/** The section heading, or the file name for a source file's comments. */
	heading: string;
	text: string;
	/** A short excerpt of `text` to show in results. */
	excerpt: string;
}

export type SearchResultFields = Omit<SearchDocument, 'text' | 'title'>;

export const SEARCH_INDEX_PATH = '/search-index.json';

/** The same options must be used to build the index and to load it. */
export const INDEX_OPTIONS: Options<SearchDocument> = {
	fields: ['title', 'heading', 'text'],
	storeFields: ['slug', 'number', 'lessonTitle', 'heading', 'excerpt']
};

export const QUERY_OPTIONS: SearchOptions = {
	boost: { title: 4, heading: 2 },
	prefix: true,
	fuzzy: 0.15,
	combineWith: 'AND'
};

/** Where a result points: the section's heading, or the top of the lesson. */
export function resultHref(result: { slug: string; id: string }): string {
	const hash = result.id.includes('#') ? result.id.slice(result.id.indexOf('#')) : '';
	return `/lessons/${result.slug}${hash}`;
}
