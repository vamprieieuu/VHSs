# VHS6RIFT — Current Validation Status

This replaces older reports that described a smaller prototype and claimed visual tests not reproducible from the current source package.

## Source validation

| Check | Result |
|---|---|
| `python tools/validate_project.py` | PASS — 192 registered parameters, UI registration order, 21 camera profiles, PiPL identity/outflags, source regression checks. |
| `python tools/audit_parameter_wiring.py` | PASS — all non-structural parameter IDs are referenced after registration. |
| `python -m pytest -q` | PASS — 11 Python tests, including source-level repair regressions. |
| Native C++ render harness | SKIPPED — Cargo is unavailable in this environment. |
| Windows MSVC build | NOT RUN — the full licensed SDK headers and PiPLtool are not present in this archive. |
| After Effects visual/runtime test | NOT RUN — requires the compiled plugin in the target host. |

## What source checks do not prove

Static parameter checks do not prove that a look is perceptually correct, that all frames render without artifacts, that audio is synchronized, or that the plugin loads in After Effects. Those claims require the Windows build and the manual test plan in `docs/TEST_PLAN.md`.
