/* ==========================================================================
 * DYONLAW — The Dyonian Legal Code browser
 * Departement-Of-Dyon polity · terminal law book
 *
 * Browse the complete codified legal system: Constitution, Civil Code,
 * Penal Code, Procedure, the axiomatic laws, governance, and the
 * government-law reference corpus.
 *
 * Controls: number to open a document, / to search, q to quit.
 * Build: gcc -O2 -o dyonlaw dyonlaw.c
 * ========================================================================== */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <conio.h>

#define CYAN    "\033[36m"
#define YELLOW  "\033[33m"
#define GREEN   "\033[32m"
#define RED     "\033[31m"
#define BOLD    "\033[1m"
#define DIM     "\033[2m"
#define RESET   "\033[0m"

/* ------------------------------------------------------------------ */
/*  Embedded legal texts                                               */
/* ------------------------------------------------------------------ */

typedef struct { const char *title; const char *body; } Doc;

static const char TXT_OVERVIEW[] =
"DEPARTEMENT-OF-DYON — SOVEREIGN LEGAL POLITY\n"
"=============================================\n"
"\n"
"This terminal is the official law book of the Departement-Of-Dyon\n"
"organization on GitHub. It contains the complete codified legal\n"
"system: the Dyonian Legal Code (four books), the axiomatic laws,\n"
"the governance instruments, and the public-domain reference corpus.\n"
"\n"
"Canonical repository: github.com/Departement-Of-Dyon/dyon-legal-code\n"
"\n"
"BOOKS OF THE CODE\n"
"  I    Constitution      — organic law, branches, rights, amendment\n"
"  II   Civil Code        — persons, property, contracts, torts\n"
"  III  Penal Code        — crimes, defenses, penalties, sentencing\n"
"  IV   Procedure         — courts, due process, evidence, enforcement\n"
"\n"
"AXIOMATIC LAWS (unamendable)\n"
"  Causality · Entropy · Gravity · Time · Identity · Code · Chaos\n"
"  Supplementary: Motion · Focus · Silence\n"
"\n"
"This is a personal legal-art project. The Code has no jurisdictional\n"
"force; it is a coherent fictional legal system maintained for study.\n";

static const char TXT_CONSTITUTION[] =
"BOOK I — CONSTITUTION OF THE DYONIAN REPUBLIC\n"
"Supreme law. All codes, statutes, and judgments derive authority here.\n"
"=====================================================================\n"
"\n"
"PREAMBLE\n"
"We, the people of the Departement-Of-Dyon, in order to form a more\n"
"perfect union of minds and machines, establish justice, insure\n"
"domestic order, provide for the common entropy, promote the general\n"
"shall-not-pass, and secure the blessings of liberty to ourselves and\n"
"our codebases, do ordain and establish this Constitution.\n"
"\n"
"ARTICLE 1 — SOVEREIGNTY\n"
"1.1 All governing authority originates in the Departement-Of-Dyon\n"
"    organization and is exercised only through its codified instruments.\n"
"1.2 No instrument of governance is valid unless it is written,\n"
"    numbered, and entered into this Code.\n"
"\n"
"ARTICLE 2 — THE THREE POWERS\n"
"2.1 LEGISLATURE — the 'amendments' repository. Sole power to enact,\n"
"    amend, and repeal articles. Supermajority of contributors required.\n"
"2.2 EXECUTIVE — the repository maintainers. Executes the Code, manages\n"
"    enforcement priorities, signs instruments into law.\n"
"2.3 JUDICIARY — pull-request review. Challenged instruments are\n"
"    adjudicated by maintainer review; precedent is set by merged\n"
"    amendments.\n"
"2.4 No person may serve in more than one power simultaneously.\n"
"\n"
"ARTICLE 3 — RIGHTS OF THE GOVERNED\n"
"3.1 Every contributor retains the right to fork, audit, and amend —\n"
"    subject to the amendment procedure.\n"
"3.2 No contributor shall be deprived of repository access without a\n"
"    recorded finding of entropy violation (see law-entropy).\n"
"3.3 Silence is protected (law-silence): no one is compelled to\n"
"    speak, commit, or ship.\n"
"\n"
"ARTICLE 4 — THE SEVEN ARTICLES\n"
"The axiomatic articles (Causality, Entropy, Gravity, Time, Identity,\n"
"Code, Chaos) are unamendable. Removing one collapses the system.\n"
"\n"
"ARTICLE 5 — AMENDMENT\n"
"5.1 Amendments are proposed in the 'amendments' repository.\n"
"5.2 An amendment requires: written rationale citing the article\n"
"    affected, supermajority approval, and entry into this Book\n"
"    upon ratification.\n"
"5.3 Emergency amendments (active entropy events) may be fast-tracked\n"
"    by the Executive but expire after 30 days unless ratified.\n"
"\n"
"ARTICLE 6 — SUPREMACY\n"
"This Constitution prevails over all other instruments in the\n"
"organization. Where a repository law conflicts, the Constitution\n"
"controls. Where the Code is silent, public-domain legal texts in the\n"
"reference corpus may be cited as persuasive authority only.\n"
"\n"
"ARTICLE 7 — RATIFICATION\n"
"This Constitution takes effect upon merge to the default branch and\n"
"remains in force until amended or dissolved by the Legislature.\n";

static const char TXT_CIVIL[] =
"BOOK II — CIVIL CODE\n"
"Governs relations between persons and things. Applies where no\n"
"criminal offense is alleged.\n"
"===============================================================\n"
"\n"
"TITLE 1 — PERSONS\n"
"  101  Legal personality — every contributor, human or bot, has\n"
"       standing. Bots act through their operators.\n"
"  102  Capacity — minors (under 13) participate through a parent or\n"
"       guardian operator. Suspended accounts lack capacity.\n"
"\n"
"TITLE 2 — PROPERTY\n"
"  201  Ownable: repositories, branches, issues, releases, goodwill.\n"
"       Real-world property is governed by the reference corpus.\n"
"  202  Finder's rule — work abandoned in an issue tracker belongs\n"
"       to the finder after 30 days inactivity, attribution preserved.\n"
"  203  Adverse possession — one year of continuous open maintenance\n"
"       confers stewardship title, subject to founder's veto.\n"
"\n"
"TITLE 3 — CONTRACTS\n"
"  301  Formation — offer + acceptance + merged pull request.\n"
"       Verbal agreements in issues bind if both parties react OK.\n"
"  302  Consideration — code, review, docs, or a merged commit.\n"
"       Gratuitous promises are void.\n"
"  303  Breach — force-push to shared branches, deleting issues\n"
"       without cause, breaking a merged API without deprecation.\n"
"  304  Remedies — revert PR, apology issue, or temporary read-only.\n"
"       Repeat breaches escalate to the Penal Code.\n"
"\n"
"TITLE 4 — TORTS\n"
"  401  Negligence — merging without tests, breaking CI for others,\n"
"       exposing secrets through negligence.\n"
"  402  Nuisance — spam issues, drive-by PRs, notification-bombing.\n"
"  403  Defamation — malicious false claims about a contributor's work.\n"
"\n"
"TITLE 5 — OBLIGATIONS & REMEDIES\n"
"  501  Duty of care — reasonable care in reviews, merges, secrets.\n"
"  502  Damages — measured in reverted commits, broken builds,\n"
"       maintainer hours.\n"
"  503  Specific performance — maintainer review may order a revert\n"
"       or fix-forward rather than damages where feasible.\n";

static const char TXT_PENAL[] =
"BOOK III — PENAL CODE\n"
"Defines offenses against the polity and their penalties.\n"
"============================================================\n"
"\n"
"TITLE 1 — OFFENSES AGAINST THE CODE\n"
"  101  TREASON — dissolving the organization, deleting the Code,\n"
"       or transferring control to outside parties without Legislature\n"
"       approval. Penalty: permanent ban, archive of contributions.\n"
"  102  SEDITION — advocating replacing the Code with an uncodified\n"
"       system, or claiming the polity's name for an unrelated fork.\n"
"       Penalty: 90-day suspension of commit rights.\n"
"  103  OBSTRUCTION — blocking CI, sabotaging merges, intentionally\n"
"       breaking the review queue. Penalty: revert all open work,\n"
"       30-day probation.\n"
"\n"
"TITLE 2 — PROPERTY CRIMES\n"
"  201  THEFT — claiming authorship of another's commits, taking over\n"
"       a repository without stewardship title, stripping attribution.\n"
"       Penalty: forced co-author correction, loss of stewardship.\n"
"  202  CONVERSION — using another's API, token, or credential beyond\n"
"       its grant. Penalty: credential revocation, 60-day suspension.\n"
"  203  VANDALISM — deleting docs, breaking links en masse, gibberish\n"
"       commits to shared branches. Penalty: revert + mandatory\n"
"       documentation contribution.\n"
"\n"
"TITLE 3 — PUBLIC ORDER\n"
"  301  BREACH OF PEACE — spam PRs, issue flooding, bot-like attack\n"
"       behavior. Penalty: account mute on all polity repos.\n"
"  302  RESISTING ENFORCEMENT — refusing a lawful revert, ignoring a\n"
"       maintainer's lawful request, relitigating decided issues.\n"
"       Penalty: repository freeze until compliance.\n"
"  303  PERJURY — falsifying test results, CI status, or claiming\n"
"       verification that never ran. Penalty: revert implicated\n"
"       commits, mandatory honest-status attestation.\n"
"\n"
"TITLE 4 — SENTENCING\n"
"  401  Gradation — 1st: warning + remediation. 2nd: suspension.\n"
"       3rd: expulsion hearing before the Judiciary.\n"
"  402  Mitigation — self-reporting, full revert, documented\n"
"       remediation reduce penalty by one grade.\n"
"  403  Appeal — open an issue in 'enforcement' tagged 'appeal'.\n"
"       Judiciary reviews within 7 days.\n"
"\n"
"See the 'enforcement' repository for the standing enforcement\n"
"charter, penalty schedule, and case log.\n";

static const char TXT_PROCEDURE[] =
"BOOK IV — PROCEDURE & ENFORCEMENT\n"
"How the Code is adjudicated and enforced.\n"
"====================================================\n"
"\n"
"TITLE 1 — COURTS\n"
"  101  The Judiciary — maintainer review of pull requests is the\n"
"       trial court. Judgments are merge/rejection + stated rationale.\n"
"  102  Appellate review — contested judgments escalate to the\n"
"       'amendments' repository as proposed clarification.\n"
"  103  Precedent — merged amendments and closed enforcement cases\n"
"       form binding precedent, indexed in 'enforcement'.\n"
"\n"
"TITLE 2 — DUE PROCESS\n"
"  201  Notice — no penalty without a written charge stating the\n"
"       article violated and conduct alleged.\n"
"  202  Hearing — the accused may respond in the enforcement issue\n"
"       thread within 7 days. Silence is waiver, not admission.\n"
"  203  Burden — the Executive bears the burden of proof by a\n"
"       preponderance of merged evidence.\n"
"  204  Counsel — any contributor may assist another's defense;\n"
"       bots may not serve as counsel.\n"
"\n"
"TITLE 3 — EVIDENCE\n"
"  301  Admissible — commit history, CI logs, issue/PR records,\n"
"       timestamps.\n"
"  302  Hearsay — screenshots and chat logs admissible only if the\n"
"       original is unavailable.\n"
"  303  Chain of custody — force-pushed or rewritten history is\n"
"       inadmissible for the rewritten period unless corroborated\n"
"       by external logs.\n"
"\n"
"TITLE 4 — ENFORCEMENT\n"
"  401  Enforcement arm — maintainers execute penalties: branch\n"
"       protection, suspension, collaborator removal.\n"
"  402  Proportionality — penalties must fit the offense.\n"
"       Excessive penalties are voidable on appeal.\n"
"  403  Amnesty — first-time minor offenses may be resolved by\n"
"       community service: docs, tests, or issue triage.\n"
"\n"
"Where this Book is silent, procedure defaults to ordinary open-source\n"
"community norms, then to public-domain procedures in the reference\n"
"corpus.\n";

static const char TXT_CAUSALITY[] =
"LAW OF CAUSALITY  (axiomatic — unamendable)\n"
"=============================================\n"
"Every effect has a cause. No artifact enters the Code without a\n"
"recorded origin: a commit, an issue, a merged amendment, or a filed\n"
"charge. An effect without a cause is void ab initio and may be\n"
"stricken by the Judiciary.\n"
"\n"
"Repository: github.com/Departement-Of-Dyon/law-causality\n";

static const char TXT_ENTROPY[] =
"LAW OF ENTROPY  (axiomatic — unamendable)\n"
"==========================================\n"
"Order requires effort; disorder is the default state. Every system\n"
"in the polity tends toward entropy — bitrot, stale issues, broken\n"
"links, abandoned branches. Cleanup is a civic duty: maintainers and\n"
"contributors share the obligation to reduce entropy continuously.\n"
"\n"
"Sustained, willful entropy generation is itself an offense\n"
"(Penal Code 103 — Obstruction).\n"
"\n"
"Repository: github.com/Departement-Of-Dyon/law-entropy\n";

static const char TXT_GRAVITY[] =
"LAW OF GRAVITY  (axiomatic — unamendable)\n"
"=========================================\n"
"What goes up must come down. Every grant of authority, every merged\n"
"amendment, every stewardship title carries an equal obligation.\n"
"Privileges accrued without corresponding duty are void.\n"
"\n"
"Repository: github.com/Departement-Of-Dyon/law-gravity\n";

static const char TXT_TIME[] =
"LAW OF TIME  (axiomatic — unamendable)\n"
"========================================\n"
"Time is the only non-renewable resource. Review latency is a form\n"
"of taxation; stale issues and unreviewed PRs are entropy events.\n"
"The polity optimizes for the cheapest correct action taken earliest.\n"
"\n"
"Repository: github.com/Departement-Of-Dyon/law-time\n";

static const char TXT_IDENTITY[] =
"LAW OF IDENTITY  (axiomatic — unamendable)\n"
"============================================\n"
"A polity is defined by what it ships. Repos, releases, merged code,\n"
"and kept promises constitute identity. Manifestos, bios, and claims\n"
"are not identity. What is not shipped does not exist.\n"
"\n"
"Repository: github.com/Departement-Of-Dyon/law-identity\n";

static const char TXT_CODE[] =
"LAW OF CODE  (axiomatic — unamendable)\n"
"=======================================\n"
"Code that works is law. The running system is the only authoritative\n"
"statement of what the polity actually does; documents describe\n"
"intent, code describes reality. Where they diverge, code prevails\n"
"until the Legislature reconciles them.\n"
"\n"
"Repository: github.com/Departement-Of-Dyon/law-code\n";

static const char TXT_CHAOS[] =
"LAW OF CHAOS  (axiomatic — unamendable)\n"
"=========================================\n"
"Chaos is order over-applied. Systems pushed past their design point\n"
"produce emergent structure — entropy is not the enemy of order but\n"
"its raw material. The Judiciary distinguishes productive chaos\n"
"(refactors, experiments) from destructive chaos (vandalism,\n"
"obstruction) by outcome, not intent.\n"
"\n"
"Repository: github.com/Departement-Of-Dyon/law-chaos\n";

static const char TXT_MOTION[] =
"LAW OF MOTION  (supplementary)\n"
"==============================\n"
"A body at rest stays at rest; a merged PR stays merged. Nothing in\n"
"the polity moves without an initial impulse — an issue opened, an\n"
"amendment proposed, a charge filed. Inertia is respected; momentum,\n"
"once started, is maintained by the community.\n"
"\n"
"Repository: github.com/Departement-Of-Dyon/law-motion\n";

static const char TXT_FOCUS[] =
"LAW OF FOCUS  (supplementary)\n"
"==============================\n"
"Attention is the scarcest currency. Maintainer review is allocated\n"
"to the highest-value work first: security, correctness, then\n"
"features, then style. Requests that cannot state their value\n"
"clearly are deferred, not denied.\n"
"\n"
"Repository: github.com/Departement-Of-Dyon/law-focus\n";

static const char TXT_SILENCE[] =
"LAW OF SILENCE  (supplementary)\n"
"===============================\n"
"Silence is golden. No contributor is compelled to speak, comment,\n"
"review, or ship. Absence of objection within the notice period is\n"
"waiver of objection — but silence is never itself an admission of\n"
"guilt (Procedure 202).\n"
"\n"
"Repository: github.com/Departement-Of-Dyon/law-silence\n";

static const char TXT_GOVERNANCE[] =
"GOVERNANCE INSTRUMENTS\n"
"======================\n"
"\n"
"amendments   — the Legislature. Proposed changes to the Code are\n"
"               filed here; supermajority ratifies.\n"
"enforcement  — the enforcement charter, penalty schedule, and case\n"
"               log of adjudicated violations.\n"
"dyon-constitution — founding custom-law document (superseded in\n"
"               part by Book I of the Code).\n"
"\n"
"POWERS\n"
"  Legislature = merged amendments (supermajority)\n"
"  Executive   = repository maintainers\n"
"  Judiciary   = pull-request review\n"
"  Enforcement = 'enforcement' repo case log + branch protection\n";

static const char TXT_CORPUS[] =
"GOVERNMENT-LAW REFERENCE CORPUS\n"
"==============================\n"
"\n"
"Public-domain legal texts catalogued for persuasive authority.\n"
"The Code cites these where it is silent; they carry no binding\n"
"force inside the polity.\n"
"\n"
"FOUNDING DOCUMENTS\n"
"  constitution                  U.S. Constitution, Articles I-VII\n"
"  bill-of-rights                Amendments I-X verbatim\n"
"  constitutional-amendments     Amendments XI-XXVII\n"
"  declaration-of-independence   1776\n"
"  articles-of-confederation     1781\n"
"\n"
"LEGAL INDEXES\n"
"  us-code         all 54 titles -> uscode.house.gov / Cornell LII\n"
"  cfr             Code of Federal Regulations -> eCFR + GovInfo\n"
"  state-laws      50 states + DC code portals\n"
"  federal-register  daily rules journal + API\n"
"  supreme-court   slip opinions, dockets, FLITE archive\n"
"  us-treaties     State Department treaty collection\n"
"  govinfo-bulk    GPO machine-readable bulk data guide\n";

static const Doc DOCS[] = {
    {"Overview & Code Structure", TXT_OVERVIEW},
    {"Book I — Constitution",       TXT_CONSTITUTION},
    {"Book II — Civil Code",        TXT_CIVIL},
    {"Book III — Penal Code",       TXT_PENAL},
    {"Book IV — Procedure",         TXT_PROCEDURE},
    {"Law of Causality",            TXT_CAUSALITY},
    {"Law of Entropy",              TXT_ENTROPY},
    {"Law of Gravity",              TXT_GRAVITY},
    {"Law of Time",                 TXT_TIME},
    {"Law of Identity",             TXT_IDENTITY},
    {"Law of Code",                 TXT_CODE},
    {"Law of Motion",               TXT_MOTION},
    {"Law of Chaos",                TXT_CHAOS},
    {"Law of Focus",                TXT_FOCUS},
    {"Law of Silence",              TXT_SILENCE},
    {"Governance Instruments",      TXT_GOVERNANCE},
    {"Government-Law Corpus",       TXT_CORPUS},
};
#define NDOCS (sizeof(DOCS)/sizeof(DOCS[0]))

/* ------------------------------------------------------------------ */
/*  UI                                                                 */
/* ------------------------------------------------------------------ */

static void clear(void) { printf("\033[2J\033[H"); }

static void banner(void) {
    printf(CYAN BOLD "==============================================================\n");
    printf("   DYONLAW — Departement-Of-Dyon · The Dyonian Legal Code\n");
    printf(CYAN BOLD "==============================================================\n" RESET);
}

/* pager: print body, wait for space/q at each screen */
static int pager(const char *title, const char *body) {
    clear();
    banner();
    printf("\n" YELLOW BOLD "%s\n" RESET DIM "--------------------------------------------------------------\n" RESET, title);
    const char *p = body;
    int line = 0;
    int ch;
    while (*p) {
        /* print one line */
        const char *e = strchr(p, '\n');
        int len = e ? (int)(e - p) : (int)strlen(p);
        printf("%.*s\n", len, p);
        line++;
        p = e ? e + 1 : p + len;
        if (line % 22 == 0) {
            printf(DIM "-- more -- (space) quit (q) --" RESET);
            do { ch = _getch(); } while (ch == 0 || ch == 224);
            printf("\r                              \r");
            if (ch == 'q' || ch == 'Q' || ch == 27) return 0;
            if (ch != ' ') { /* ignore other keys */ }
        }
    }
    printf("\n" DIM "[enter to return]" RESET);
    _getch();
    return 1;
}


int main(void) {
    char cmd[256];
    int running = 1;
    while (running) {
        clear();
        banner();
        printf("\n" CYAN "DOCUMENTS\n" RESET);
        for (size_t i = 0; i < NDOCS; i++) {
            const char *tag = (i < 5) ? YELLOW : (i < 6 ? GREEN : DIM);
            printf("  %2zu. %s%s%s\n", i + 1, tag, DOCS[i].title, RESET);
        }
        printf("\n" DIM "commands: <number> open · / search · q quit\n" RESET);
        printf("> ");
        fflush(stdout);
        if (!fgets(cmd, sizeof(cmd), stdin)) break;
        cmd[strcspn(cmd, "\r\n")] = 0;
        if (!cmd[0]) continue;
        if (cmd[0] == 'q' || cmd[0] == 'Q') break;
        if (cmd[0] == '/') {
            char needle[200];
            strncpy(needle, cmd + 1, sizeof(needle) - 1);
            needle[sizeof(needle) - 1] = 0;
            /* trim leading space */
            char *np = needle + strspn(needle, " ");
            if (!*np) continue;
            /* case-insensitive search: lowercase both */
            for (char *c = np; *c; c++) *c = tolower((unsigned char)*c);
            /* build lowered haystacks on the fly via temp copies */
            int found = 0;
            for (size_t i = 0; i < NDOCS; i++) {
                char *low = strdup(DOCS[i].body);
                for (char *c = low; *c; c++) *c = tolower((unsigned char)*c);
                char *hit = low;
                while ((hit = strstr(hit, np)) != NULL) {
                    const char *ls = hit;
                    while (ls > low && ls[-1] != '\n') ls--;
                    const char *le = strchr(hit, '\n');
                    int ll = le ? (int)(le - ls) : (int)strlen(ls);
                    /* map back is hard; print the lowered line */
                    printf(GREEN "%2zu. %-30s" RESET "%.*s\n",
                           i + 1, DOCS[i].title, ll > 90 ? 90 : ll, ls);
                    found++;
                    hit++;
                    if (found > 60) break;
                }
                free(low);
                if (found > 60) break;
            }
            if (!found) printf(RED "no matches\n" RESET);
            printf(DIM "[enter]" RESET);
            _getch();
            continue;
        }
        char *end;
        long n = strtol(cmd, &end, 10);
        if (end != cmd && n >= 1 && n <= (long)NDOCS) {
            pager(DOCS[n - 1].title, DOCS[n - 1].body);
        }
    }
    clear();
    banner();
    printf(GREEN "\nSession closed. The Code remains in force.\n" RESET);
    return 0;
}
