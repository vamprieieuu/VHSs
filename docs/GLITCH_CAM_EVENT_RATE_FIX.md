# Glitch Cam event-rate regression fix

The native renderer derives `eventPeriod` from `effectiveEventRate`, which prefers the Glitch Cam Event Rate control when non-zero and otherwise falls back to the general event rate. The event gate previously checked only `s.eventRate`, creating a mismatch: the Glitch Cam rate could determine slot boundaries while the gate still rejected all events when the general event rate was zero.

The gate now checks `effectiveEventRate` as well. The existing global `s.events` enable remains respected, so this fix does not force glitches on when the general event/glitch system is disabled.

## Verification

- `tools/test_vhs6rift.py`: 4/4 Python-model tests pass.
- `tools/validate_project.py`: source/UI/PiPL checks and the new regression guard pass; full validation then stops because `cargo` is not installed in this environment. Native C++ rendering and After Effects runtime remain unverified.
