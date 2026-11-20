import { describe, expect, it } from 'vitest';
import { courseJsonLd, jsonLdScript, lessonJsonLd } from './structured-data.js';

describe('structured data', () => {
	it('describes the course', () => {
		expect(courseJsonLd(35)).toMatchObject({
			'@type': 'Course',
			url: 'https://cfromthegroundup.com/',
			numberOfLessons: 35,
			isAccessibleForFree: true
		});
	});

	it('describes a lesson with breadcrumbs', () => {
		const [resource, breadcrumbs] = lessonJsonLd({
			number: 10,
			slug: '10-pointers',
			title: 'Pointers',
			description: 'About pointers.',
			part: { number: 2, title: 'The Intermediate Path' }
		});
		expect(resource).toMatchObject({
			'@type': 'LearningResource',
			url: 'https://cfromthegroundup.com/lessons/10-pointers',
			position: 10
		});
		expect(breadcrumbs.itemListElement?.map((item) => item.item)).toEqual([
			'https://cfromthegroundup.com/lessons',
			'https://cfromthegroundup.com/parts/2',
			'https://cfromthegroundup.com/lessons/10-pointers'
		]);
	});

	it('cannot be closed early by the data', () => {
		const html = jsonLdScript({ name: '</script><script>alert(1)</script>' });
		expect(html.match(/<\/script>/g)).toHaveLength(1);
		expect(JSON.parse(html.slice(html.indexOf('>') + 1, -'</script>'.length))).toEqual({
			name: '</script><script>alert(1)</script>'
		});
	});
});
