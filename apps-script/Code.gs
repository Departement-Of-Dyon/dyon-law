/**
 * DyonLaw Wiki — community posting bridge (Google Apps Script).
 *
 * SETUP (one time, ~5 min):
 *  1. Go to script.google.com -> New project -> paste this whole file.
 *  2. Project Settings -> Script Properties -> add:
 *       GH_TOKEN = a GitHub PAT with Contents:Read+Write on
 *                  Departement-Of-Dyon/dyon-law
 *  3. Deploy -> New deployment -> Web app:
 *       Execute as: Me
 *       Who has access: Anyone
 *  4. Copy the /exec URL into docs/wiki/submit.html (GAS_URL constant)
 *     and commit — the submit page goes live for everyone.
 */

var REPO = 'Departement-Of-Dyon/dyon-law';
var DIR  = 'docs/wiki/posts/';

function gh(method, path, body) {
  var t = PropertiesService.getScriptProperties().getProperty('GH_TOKEN');
  if (!t) return { code: 500, data: { message: 'GH_TOKEN not set' } };
  var o = {
    method: method,
    headers: {
      'Authorization': 'token ' + t,
      'Accept': 'application/vnd.github+json',
      'User-Agent': 'dyon-law-wiki'
    },
    muteHttpExceptions: true
  };
  if (body) { o.payload = JSON.stringify(body); o.contentType = 'application/json'; }
  var r = UrlFetchApp.fetch('https://api.github.com/repos/' + REPO + '/' + path, o);
  var code = r.getResponseCode();
  try { return { code: code, data: JSON.parse(r.getContentText()) }; }
  catch (e) { return { code: code, data: { message: r.getContentText().slice(0, 200) } }; }
}

function b64(s) { return Utilities.base64Encode(s, 'UTF-8'); }
function out(o) {
  return ContentService.createTextOutput(JSON.stringify(o))
    .setMimeType(ContentService.MimeType.JSON);
}

function doGet() { return out({ ok: true, service: 'dyon-law-wiki' }); }

function doPost(e) {
  var p = (e && e.parameter) || {};
  var title   = (p.title || '').trim();
  var excerpt = (p.excerpt || '').trim();
  var body    = (p.body || '').trim();
  var slug    = (p.slug || '').trim().toLowerCase()
                  .replace(/[^a-z0-9]+/g, '-').replace(/^-+|-+$/g, '');
  if (!title || !body || !slug)
    return out({ ok: false, error: 'title, slug and body are required' });
  if (slug.length > 60) slug = slug.slice(0, 60);

  // never overwrite an existing post
  var ex = gh('GET', 'contents/' + DIR + slug + '.md?ref=main');
  if (ex.code === 200)
    return out({ ok: false, error: 'slug "' + slug + '" already exists — choose another' });

  var md = '# ' + title + '\n\n' + body + '\n';
  var w = gh('PUT', 'contents/' + DIR + slug + '.md', {
    message: 'wiki: community post ' + slug,
    content: b64(md)
  });
  if (w.code >= 300)
    return out({ ok: false, error: 'github write failed: ' + (w.data.message || w.code) });

  // update the index
  var ix = gh('GET', 'contents/' + DIR + 'index.json?ref=main');
  var idx = { posts: [] };
  if (ix.code === 200) {
    try {
      idx = JSON.parse(Utilities.newBlob(
        Utilities.base64Decode(ix.data.content)).getDataAsString());
    } catch (_) {}
  }
  idx.posts = idx.posts || [];
  if (!idx.posts.some(function (x) { return x.slug === slug; })) {
    idx.posts.unshift({
      slug: slug, title: title,
      date: new Date().toISOString().slice(0, 10),
      excerpt: excerpt
    });
  }
  var iw = gh('PUT', 'contents/' + DIR + 'index.json', {
    message: 'wiki: index +' + slug,
    content: b64(JSON.stringify(idx, null, 2)),
    sha: ix.code === 200 ? ix.data.sha : undefined
  });

  return out({ ok: iw.code < 300, slug: slug,
               error: iw.code < 300 ? null : 'post saved but index update failed' });
}
