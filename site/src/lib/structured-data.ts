// schema.org structured data (JSON-LD) for search engines.

import { GITHUB_URL, SITE_DESCRIPTION, SITE_NAME, SITE_URL } from './site.js';

const AUTHOR = { '@type': 'Person', name: 'dunamismax', url: 'https://github.com/dunamismax' };

const COURSE_REF = { '@type': 'Course', name: SITE_NAME, url: `${SITE_URL}/` };

export function courseJsonLd(lessonCount: number) {
	return {
		'@context': 'https://schema.org',
		'@type': 'Course',
		name: SITE_NAME,
		description: SITE_DESCRIPTION,
		url: `${SITE_URL}/`,
		inLanguage: 'en',
		isAccessibleForFree: true,
		license: 'https://opensource.org/licenses/MIT',
		educationalLevel: 'Beginner to advanced',
		teaches: 'The C programming language',
		numberOfLessons: lessonCount,
		author: AUTHOR,
		provider: AUTHOR,
		sameAs: GITHUB_URL,
		offers: { '@type': 'Offer', category: 'Free', price: 0, priceCurrency: 'USD' },
		hasCourseInstance: {
			'@type': 'CourseInstance',
			courseMode: 'Online',
			courseWorkload: `P${lessonCount}D`
		}
	};
}

export function lessonJsonLd(lesson: {
	number: number;
	slug: string;
	title: string;
	description: string;
	part: { number: number; title: string };
}) {
	const url = `${SITE_URL}/lessons/${lesson.slug}`;
	return [
		{
			'@context': 'https://schema.org',
			'@type': 'LearningResource',
			name: `Lesson ${lesson.number}: ${lesson.title}`,
			description: lesson.description,
			url,
			inLanguage: 'en',
			isAccessibleForFree: true,
			learningResourceType: 'Lesson',
			position: lesson.number,
			author: AUTHOR,
			isPartOf: COURSE_REF
		},
		{
			'@context': 'https://schema.org',
			'@type': 'BreadcrumbList',
			itemListElement: [
				{ '@type': 'ListItem', position: 1, name: 'Lessons', item: `${SITE_URL}/lessons` },
				{
					'@type': 'ListItem',
					position: 2,
					name: `Part ${lesson.part.number}: ${lesson.part.title}`,
					item: `${SITE_URL}/parts/${lesson.part.number}`
				},
				{ '@type': 'ListItem', position: 3, name: lesson.title, item: url }
			]
		}
	];
}

/**
 * A `<script type="application/ld+json">` element for {@html}. `<` is escaped
 * so no string in the data can close the element early.
 */
export function jsonLdScript(data: unknown): string {
	const json = JSON.stringify(data).replace(/</g, '\\u003c');
	return `<script type="application/ld+json">${json}</script>`;
}
