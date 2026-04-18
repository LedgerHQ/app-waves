# Upstream PR: LedgerHQ/app-waves

English-only pack for a **pull request** against **[LedgerHQ/app-waves](https://github.com/LedgerHQ/app-waves)** (branch **[`develop`](https://github.com/LedgerHQ/app-waves/tree/develop)**).

## Background (for reviewers)

The Waves Ledger app has a long history of community forks. **This submission is a deliberate rewrite:** the on-device application was **simplified** and rebuilt to match **current Ledger device-app guidelines** and the **modern SDK** (NBGL, standard app patterns, Ragger tests). It is **not** an incremental edit of legacy files from older forks; it **replaces** that implementation with a maintainable baseline.

## Relation to PR #17

- **[PR #17 — “Version 2.0.1”](https://github.com/LedgerHQ/app-waves/pull/17)** targets the **legacy** codebase (older protocol and stack). This repository uses **APDU spec 2.4**, **firmware 2.1.0**, and a different architecture.
- Do **not** merge this work into PR #17 unless Ledger explicitly requests it. The normal path is a **new PR** into `develop` with a clear **supersedes / replaces** note.

## What you do locally (Git)

1. **Fork** `LedgerHQ/app-waves` under your GitHub account (or use an existing fork).
2. Follow **[FORK_AND_PR.md](FORK_AND_PR.md)** — clone the fork, branch from `develop`, **replace the tree** with this repo (`rsync` excluding `.git`), commit, push.
3. Open a PR: **base** = `LedgerHQ/app-waves` → **`develop`**, **compare** = your branch.

Step-by-step replace workflow: [FORK_AND_PR.md](FORK_AND_PR.md).

## PR title (copy-paste)

```
WAVES Ledger app: Modern SDK, APDU spec 2.4, firmware 2.1.0
```

## PR description (copy-paste)

Edit if needed, then use **[UPSTREAM_PR_BODY.md](UPSTREAM_PR_BODY.md)** as the GitHub PR body (copy from the file, or `gh pr create ... --body-file doc/UPSTREAM_PR_BODY.md`).

To create the PR from the CLI (after `gh auth login` and `git push` to your fork):

```bash
gh pr create \
  --repo LedgerHQ/app-waves \
  --base develop \
  --head YOUR_GITHUB_LOGIN:YOUR_BRANCH \
  --title "WAVES Ledger app: Modern SDK, APDU spec 2.4, firmware 2.1.0" \
  --body-file doc/UPSTREAM_PR_BODY.md
```

Replace `YOUR_GITHUB_LOGIN:YOUR_BRANCH` with your fork and branch (e.g. `vba2000:modern-sdk-2.1.0`). If the PR is opened from the fork’s UI, paste **`doc/UPSTREAM_PR_BODY.md`** into the description field.

## After opening the PR

- Watch for **Ledger reviewer** feedback.
- Keep **CI green** on the PR branch (`build_and_functional_tests`, `unit_tests`, `guidelines_enforcer`, etc.).

## Repository URL

After you push, your fork will look like:

`https://github.com/<your-org>/app-waves`
