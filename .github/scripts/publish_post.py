#!/usr/bin/env python3
"""Publish a GitHub issue as a DyonLaw wiki post.

Issue title  -> post title (also used for the slug)
Issue body   -> post markdown (first paragraph = excerpt)
Label        -> 'wiki-post' (set by the workflow trigger)

Runs inside GitHub Actions with GITHUB_TOKEN (contents:write).
"""
import json
import os
import re
import subprocess
import sys
import urllib.request

REPO = os.environ.get("GITHUB_REPOSITORY", "Departement-Of-Dyon/dyon-law")
DIR = "docs/wiki/posts"
ISSUE = int(os.environ["ISSUE_NUMBER"])
TITLE = os.environ["ISSUE_TITLE"].strip()
BODY = os.environ.get("ISSUE_BODY", "")
API = f"https://api.github.com/repos/{REPO}"


def gh_api(path, method="GET", data=None):
    req = urllib.request.Request(
        API + path,
        method=method,
        data=json.dumps(data).encode() if data is not None else None,
        headers={
            "Authorization": f"Bearer {os.environ['GITHUB_TOKEN']}",
            "Accept": "application/vnd.github+json",
            "User-Agent": "dyon-law-wiki",
        },
    )
    try:
        with urllib.request.urlopen(req) as r:
            return r.status, json.loads(r.read() or b"{}")
    except urllib.error.HTTPError as e:
        try:
            return e.code, json.loads(e.read() or b"{}")
        except Exception:
            return e.code, {}


def sh(*args, check=True):
    p = subprocess.run(args, capture_output=True, text=True)
    if check and p.returncode != 0:
        print(p.stdout, p.stderr, file=sys.stderr)
        sys.exit(p.returncode)
    return p.stdout.strip()


def slugify(t):
    s = re.sub(r"[^a-z0-9]+", "-", t.lower()).strip("-")
    return (s or "post")[:60]


def main():
    slug = slugify(TITLE)
    # excerpt = first non-empty, non-heading line, trimmed
    excerpt = ""
    for line in BODY.splitlines():
        line = line.strip().lstrip("#").strip("*`> ")
        if line and not line.startswith(("!", "[", "```")):
            excerpt = line[:160]
            break

    md = f"# {TITLE}\n\n{BODY.strip()}\n"
    path = f"{DIR}/{slug}.md"

    # write the post file (create or update)
    st, existing = gh_api(f"/contents/{path}?ref=main")
    payload = {
        "message": f"wiki: publish post '{slug}' (issue #{ISSUE})",
        "content": __import__("base64").b64encode(md.encode()).decode(),
    }
    if st == 200 and existing.get("sha"):
        payload["sha"] = existing["sha"]
    st, res = gh_api(f"/contents/{path}", "PUT", payload)
    if st not in (200, 201):
        print("file write failed:", res)
        sys.exit(1)

    # update index.json
    st, idx_file = gh_api(f"/contents/{DIR}/index.json?ref=main")
    idx = {"posts": []}
    if st == 200 and idx_file.get("content"):
        import base64
        try:
            idx = json.loads(base64.b64decode(idx_file["content"]))
        except Exception:
            pass
    idx.setdefault("posts", [])
    entry = {"slug": slug, "title": TITLE,
             "date": __import__("datetime").date.today().isoformat(),
             "excerpt": excerpt}
    idx["posts"] = [p for p in idx["posts"] if p.get("slug") != slug]
    idx["posts"].insert(0, entry)

    payload = {
        "message": f"wiki: index {'update' if st == 200 else 'create'} for '{slug}'",
        "content": __import__("base64").b64encode(
            json.dumps(idx, indent=2).encode()).decode(),
    }
    if st == 200 and idx_file.get("sha"):
        payload["sha"] = idx_file["sha"]
    st, res = gh_api(f"/contents/{DIR}/index.json", "PUT", payload)
    if st not in (200, 201):
        print("index write failed:", res)
        sys.exit(1)

    # confirm on the issue + close it
    link = (f"https://departement-of-dyon.github.io/dyon-law/wiki/"
            f"post.html?slug={slug}")
    gh_api(f"/issues/{ISSUE}/comments", "POST",
           {"body": f"Published to the wiki: {link}\n\n"
                    f"(Pages rebuild takes ~1 minute)"})
    gh_api(f"/issues/{ISSUE}", "PATCH", {"state": "closed"})
    print(f"published {slug} -> {link}")


if __name__ == "__main__":
    main()
