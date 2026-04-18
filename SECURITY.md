# Security

## Reporting vulnerabilities

If you believe you have found a security vulnerability in this Ledger device application:

1. **Preferred:** open a [GitHub Security Advisory](https://docs.github.com/en/code-security/security-advisories/guidance-on-reporting-and-writing-information-about-vulnerabilities) (private report) against this repository if the maintainer has GitHub Security enabled.
2. **Alternatively:** contact the repository maintainers through the channel they advertise for the Waves / Ledger integration (e.g. project README or organization security policy).

Please do **not** open a public issue for undisclosed vulnerabilities.

## Scope

Security reviews should focus on:

- APDU handling, state machine, and memory clearing around signing ([APP_SPECIFICATION.md](APP_SPECIFICATION.md)).
- Cryptographic use of the Ledger SDK (no custom low-level curve code in this app).

Out of scope for this repository: host wallets, blockchain consensus, and third-party Ledger Live plugins.

## Supported versions

Security fixes are applied to the current release line described in [CHANGELOG.md](CHANGELOG.md). Use the latest tagged release when deploying to devices.
