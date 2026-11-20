// Open Graph images (the preview card shown when a page is shared), rendered
// at build time: satori lays the card out as SVG, resvg (WebAssembly) turns it
// into a PNG. Fonts come from the @fontsource packages (OFL licensed), so no
// font file is committed. Only prerendering calls this; the production server,
// which has no node_modules, never does.

import { readFile } from 'node:fs/promises';
import { createRequire } from 'node:module';
import path from 'node:path';
import { initWasm, Resvg } from '@resvg/resvg-wasm';
import satori from 'satori';

export const OG_WIDTH = 1200;
export const OG_HEIGHT = 630;

export interface OgCard {
	/** Small line above the title, for example "Part 1 · Lesson 7". */
	eyebrow: string;
	title: string;
	description: string;
}

// The site's dark theme and the logo's colors (src/app.css, Logo.svelte).
const colors = {
	background: '#0d1117',
	ink: '#e6edf3',
	muted: '#a1a8b3',
	accent: '#8fa6ff',
	logo: '#2945b0',
	ember: '#f2b360'
};

const LOGO = `data:image/svg+xml,${encodeURIComponent(
	`<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 32 32">` +
		`<rect width="32" height="32" rx="7" fill="${colors.logo}"/>` +
		`<path d="M21.8 8.2A7.5 7.5 0 1 0 21.8 17.8" fill="none" stroke="#fff" stroke-width="3.5" stroke-linecap="round"/>` +
		`<rect x="7" y="24" width="18" height="2.25" rx="1.1" fill="#fff"/>` +
		`<rect x="10" y="27.5" width="12" height="1.75" rx="0.9" fill="${colors.ember}"/></svg>`
)}`;

type Fonts = Parameters<typeof satori>[1]['fonts'];
let setup: Promise<Fonts> | undefined;

// Resolve packages from the site's own node_modules (the build runs in site/).
function load(specifier: string): Promise<Buffer> {
	const require = createRequire(path.join(process.cwd(), 'package.json'));
	return readFile(require.resolve(specifier));
}

function prepare(): Promise<Fonts> {
	setup ??= (async () => {
		await initWasm(await load('@resvg/resvg-wasm/index_bg.wasm'));
		const [regular, bold, mono] = await Promise.all([
			load('@fontsource/inter/files/inter-latin-400-normal.woff'),
			load('@fontsource/inter/files/inter-latin-700-normal.woff'),
			load('@fontsource/jetbrains-mono/files/jetbrains-mono-latin-500-normal.woff')
		]);
		return [
			{ name: 'Inter', data: regular, weight: 400, style: 'normal' },
			{ name: 'Inter', data: bold, weight: 700, style: 'normal' },
			{ name: 'JetBrains Mono', data: mono, weight: 500, style: 'normal' }
		];
	})();
	return setup;
}

/** A satori element; satori takes React-like objects, so no JSX is needed. */
function h(
	type: string,
	style: Record<string, string | number>,
	children?: unknown,
	props: Record<string, unknown> = {}
) {
	return { type, props: { ...props, style, children } };
}

const PAD_X = 80;
const PAD_Y = 72;

function card({ eyebrow, title, description }: OgCard) {
	// Explicit sizes rather than padding: satori adds padding outside a
	// 100% box, which would push the footer off the image.
	const inner = h(
		'div',
		{
			width: OG_WIDTH - 2 * PAD_X,
			height: OG_HEIGHT - 2 * PAD_Y,
			marginTop: PAD_Y,
			marginLeft: PAD_X,
			display: 'flex',
			flexDirection: 'column',
			justifyContent: 'space-between'
		},
		[
			h('div', { display: 'flex', alignItems: 'center', gap: 24 }, [
				h('img', { width: 64, height: 64 }, undefined, { src: LOGO, width: 64, height: 64 }),
				h('div', { fontSize: 32, fontWeight: 700 }, 'C From The Ground Up')
			]),
			h('div', { display: 'flex', flexDirection: 'column', gap: 16 }, [
				h(
					'div',
					{ fontFamily: 'JetBrains Mono', fontSize: 26, color: colors.ember, letterSpacing: 1 },
					eyebrow
				),
				h(
					'div',
					{ fontSize: title.length > 26 ? 60 : 72, fontWeight: 700, lineHeight: 1.1 },
					title
				),
				h('div', { fontSize: 28, color: colors.muted, lineHeight: 1.4 }, description)
			]),
			h('div', { display: 'flex', alignItems: 'center', justifyContent: 'space-between' }, [
				h(
					'div',
					{ fontFamily: 'JetBrains Mono', fontSize: 26, color: colors.accent },
					'cfromthegroundup.com'
				),
				h('div', { display: 'flex', flexDirection: 'column', alignItems: 'center', gap: 8 }, [
					h('div', { width: 160, height: 10, borderRadius: 5, backgroundColor: colors.ink }),
					h('div', { width: 108, height: 8, borderRadius: 4, backgroundColor: colors.ember })
				])
			])
		]
	);
	return h(
		'div',
		{
			width: OG_WIDTH,
			height: OG_HEIGHT,
			display: 'flex',
			backgroundColor: colors.background,
			fontFamily: 'Inter',
			color: colors.ink
		},
		[inner]
	);
}

/** Render a 1200 x 630 PNG. */
export async function renderOgImage(content: OgCard): Promise<Uint8Array> {
	const fonts = await prepare();
	const svg = await satori(card(content) as Parameters<typeof satori>[0], {
		width: OG_WIDTH,
		height: OG_HEIGHT,
		fonts
	});
	return new Resvg(svg, { fitTo: { mode: 'original' } }).render().asPng();
}
