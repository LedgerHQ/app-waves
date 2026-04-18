# Fork LedgerHQ/app-waves and open a PR with this codebase

Target upstream: **[LedgerHQ/app-waves](https://github.com/LedgerHQ/app-waves)** (branch **`develop`**:  
[https://github.com/LedgerHQ/app-waves/tree/develop](https://github.com/LedgerHQ/app-waves/tree/develop)).

This repo (**LedgerWaves**) is a **full replacement** of the legacy app in that repository (new stack, APDU spec 2.4, firmware 2.1.0). You do **not** merge file-by-file; you replace the tree on a **new branch** and open a PR into **`develop`**.

---

## Step 1 — Create a fork on GitHub (browser)

1. Open [LedgerHQ/app-waves](https://github.com/LedgerHQ/app-waves).
2. Click **Fork** (top right). Choose your account or org.
3. After fork is created, clone **your fork**, not the LedgerHQ URL directly (unless you have write access to LedgerHQ).

Your fork URL will look like:  
`https://github.com/<YOUR_USERNAME_OR_ORG>/app-waves`

---

## Step 2 — Clone the fork and branch from `develop`

```bash
git clone https://github.com/<YOUR_USERNAME_OR_ORG>/app-waves.git
cd app-waves
git remote add upstream https://github.com/LedgerHQ/app-waves.git
git fetch upstream
git checkout develop
git pull upstream develop
git checkout -b modern-sdk-2.1.0
```

(Adjust branch name if you prefer, e.g. `feat/waves-modern-sdk`.)

---

## Step 3 — Replace the tree with this project

From a directory **outside** `app-waves`, with **LedgerWaves** at `/path/to/LedgerWaves`:

**Linux / macOS** (example: your clone is `~/LedgerWaves` and fork clone is `~/app-waves`):

```bash
cd ~/app-waves
# Remove tracked files but keep .git
git rm -rf .
# Copy everything from LedgerWaves except its .git
rsync -a --exclude='.git' ~/LedgerWaves/ ./
git add -A
git status   # review
git commit -m "feat: WAVES Ledger app — Modern SDK, APDU spec 2.4, firmware 2.1.0"
```

**Windows:** use `robocopy` or copy manually, excluding `.git` from LedgerWaves.

---

## Step 4 — Push and open the pull request

```bash
git push -u origin modern-sdk-2.1.0
```

Then on GitHub:

1. Open **your fork** → you should see **Compare & pull request** for the new branch.
2. **Base repository:** `LedgerHQ/app-waves` — **base:** `develop`.
3. **Head:** your fork — branch `modern-sdk-2.1.0`.
4. Paste title/description from [UPSTREAM_PR_APP_WAVES.md](UPSTREAM_PR_APP_WAVES.md).

---

## If you already develop only in LedgerWaves (this repo)

1. Ensure **first commit** exists here: `git commit -m "Initial WAVES Ledger app (modern)"` (after `git add` as needed).
2. Add your fork as remote:  
   `git remote add fork https://github.com/<YOU>/app-waves.git`
3. Fetch upstream:  
   `git remote add upstream https://github.com/LedgerHQ/app-waves.git`  
   `git fetch upstream`
4. **Do not** force-push `master` onto `develop` without coordinating — the histories are unrelated. Prefer the **clone fork → replace tree → new branch** workflow above, or **orphan** branch (advanced). The replace-tree method is easiest for reviewers.

---

## Checklist before opening the PR

- [ ] Green CI on your fork (GitHub Actions enabled on the fork).
- [ ] [CHANGELOG.md](../CHANGELOG.md) and version in [Makefile](../Makefile) match the release you intend.
- [ ] PR body links to [APP_SPECIFICATION.md](../APP_SPECIFICATION.md) and mentions relationship to **legacy** [app-waves](https://github.com/LedgerHQ/app-waves/tree/develop).

---

## Legal / license

- **Upstream** [app-waves](https://github.com/LedgerHQ/app-waves) is **MIT** (see upstream `LICENSE`).
- Ensure **this** repository’s license file (here: [LICENSE.md](../LICENSE.md)) and headers are compatible with how you merge. If you combine both histories, resolve the license file in the merge commit; if you **replace** the tree, the license in LedgerWaves is the one you intend to ship.
