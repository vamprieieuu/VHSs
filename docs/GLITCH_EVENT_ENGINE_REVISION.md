# VHS6RIFT — Glitch event engine revision

This revision adds a deterministic horizontal sync-tear path to the existing discrete event engine. During an event envelope, selected horizontal bands sample a horizontally displaced source location; a small channel-offset contribution recreates chroma breakup. Existing dropout and block-corruption branches remain intact.

The tear selection is a function of the seed, event slot, and band index, so scrubbing or rendering the same frame is deterministic. It does not keep mutable frame history. The effect is gated by the existing Event Glitches control and scales with the event envelope and block-corruption setting.

Validation status: source-level patch and archive checks only. A Windows/MSVC build and After Effects 23.2.1 visual render have not been run in this environment.
