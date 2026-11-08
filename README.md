# DyonLaw

The official law book of the **Departement-Of-Dyon** polity —
the complete Dyonian Legal Code.

## Editions

- **`lawwin.exe`** — native Win32 GUI (recommended): document list,
  reader pane, search box, status bar. C++ source: `lawwin.cpp`
- **`dyonlaw.exe`** — terminal version with pager + `/search`. C source:
  `dyonlaw.c`

Both share the same embedded legal texts (`lawtexts.h`).

## What it contains

- **Book I — Constitution**: three powers, rights of the governed,
  unamendable axioms, amendment procedure, supremacy clause
- **Book II — Civil Code**: persons, property, contracts, torts, remedies
- **Book III — Penal Code**: treason through perjury, sentencing, appeals
- **Book IV — Procedure**: courts, due process, evidence, enforcement
- **All 10 laws**: Causality, Entropy, Gravity, Time, Identity, Code,
  Motion, Chaos, Focus, Silence
- **Governance instruments** and the **government-law reference corpus**
  (U.S. founding documents, U.S. Code, CFR, state codes, treaties)

## Build

```bash
# GUI (Windows, zero dependencies)
g++ -O2 -mwindows -o lawwin.exe lawwin.cpp

# Terminal
gcc -O2 -o dyonlaw dyonlaw.c
```

## Canonical sources

- Legal code: https://github.com/Departement-Of-Dyon/dyon-legal-code
- Org profile: https://github.com/Departement-Of-Dyon

MIT licensed. Personal legal-art project — the Code has no
jurisdictional force.
