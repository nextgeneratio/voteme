# Changelog

All notable changes to this project after the cleanup are documented in this file.

## [Unreleased]

- Add more unit tests for edge cases and error handling
- Introduce CI for multiple OS targets

## [2025-10-21] - Repository Simplification

### Added

- Minimal unit tests:
  - `tests/test_voting.c` for voting algorithm basic scenario
  - `tests/test_data_smoke.c` for data handler smoke test
- Simplified `Makefile` with `admin`, `tests`, and `test` targets
- Minimal `README.md` with build/run/test instructions and data schemas

### Changed

- Focus the codebase on three concerns: enhanced data handling, admin console, voting algorithm

### Removed

- Legacy UI and unused modules: `src/main.c`, `src/display.c`, `src/voter_register.*`, `src/candidate_register.*`, `src/data_handler.*`
- Demo scripts and extra docs: `demo*.sh`, `README_Enhanced.md`, `VOTING_ALGORITHM_GUIDE.md`, `VOTING_IMPLEMENTATION_COMPLETE.md`, `docs/*`
- Old tests and debug suites not aligned with the minimal scope
