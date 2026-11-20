// Book edition only (mdBook, added through additional-js in book.toml).
// Puts a link at the top of every page to the same lesson on the course site,
// cfromthegroundup.com (BUILD.md, D6 and W7). Chapter file names are the
// site's lesson slugs, so /chapters/07-functions.html maps to
// /lessons/07-functions; the site's content tests keep the two in step.
(function () {
    'use strict';
    var site = 'https://cfromthegroundup.com';
    var main = document.querySelector('main');
    if (!main) {
        return;
    }
    var match = /\/chapters\/(\d{2}-[a-z0-9-]+)\.html$/.exec(window.location.pathname);
    var link = document.createElement('a');
    link.href = match ? site + '/lessons/' + match[1] : site + '/';
    link.textContent = match ? 'Read this lesson on cfromthegroundup.com' : 'Take the course at cfromthegroundup.com';
    var banner = document.createElement('aside');
    banner.className = 'site-link';
    banner.setAttribute('aria-label', 'Course website');
    var lead = document.createElement('strong');
    lead.textContent = 'The course has a new home. ';
    banner.appendChild(lead);
    banner.appendChild(link);
    banner.appendChild(
        document.createTextNode(': progress tracking, search, and a "Try it online" button for most lessons.')
    );
    main.insertBefore(banner, main.firstChild);
})();
