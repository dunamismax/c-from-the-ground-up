import MiniSearch from 'minisearch';
import { describe, expect, it } from 'vitest';
import { INDEX_OPTIONS, QUERY_OPTIONS, resultHref, type SearchDocument } from './search.js';

const docs: SearchDocument[] = [
	{
		id: '10-pointers',
		slug: '10-pointers',
		number: 10,
		title: 'Pointers',
		lessonTitle: 'Pointers',
		heading: '',
		text: 'A pointer holds a memory address.',
		excerpt: 'A pointer holds a memory address.'
	},
	{
		id: '13-dynamic-memory-allocation#malloc',
		slug: '13-dynamic-memory-allocation',
		number: 13,
		title: '',
		lessonTitle: 'Dynamic Memory Allocation',
		heading: 'malloc',
		text: 'malloc reserves memory on the heap; free gives it back.',
		excerpt: 'malloc reserves memory on the heap; free gives it back.'
	}
];

describe('search', () => {
	it('finds sections by prefix after a JSON round trip', () => {
		const built = new MiniSearch(INDEX_OPTIONS);
		built.addAll(docs);
		const loaded = MiniSearch.loadJSON<SearchDocument>(JSON.stringify(built), INDEX_OPTIONS);
		const results = loaded.search('mallo', QUERY_OPTIONS);
		expect(results[0]).toMatchObject({
			id: '13-dynamic-memory-allocation#malloc',
			heading: 'malloc'
		});
		expect(loaded.search('memory address', QUERY_OPTIONS).map((r) => String(r.id))).toEqual([
			'10-pointers'
		]);
	});

	it('links a result to its heading, or to the lesson', () => {
		expect(
			resultHref({
				slug: '13-dynamic-memory-allocation',
				id: '13-dynamic-memory-allocation#malloc'
			})
		).toBe('/lessons/13-dynamic-memory-allocation#malloc');
		expect(resultHref({ slug: '10-pointers', id: '10-pointers' })).toBe('/lessons/10-pointers');
	});
});
