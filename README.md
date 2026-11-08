# DyonLaw

The official terminal law book of the **Departement-Of-Dyon** polity —
the complete Dyonian Legal Code in a single C program.

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

## Usage

```
dyonlaw.exe
```

- Type a document number to open it (space = next page, `q` = back)
- `/keyword` to search all documents case-insensitively
- `q` to quit

## Build

```bash
gcc -O2 -o dyonlaw dyonlaw.c
```

Windows binary (`dyonlaw.exe`) is attached to the latest release.

## Canonical sources

- Legal code: https://github.com/Departement-Of-Dyon/dyon-legal-code
- Org profile: https://github.com/Departement-Of-Dyon

MIT licensed. Personal legal-art project — the Code has no
jurisdictional force.
