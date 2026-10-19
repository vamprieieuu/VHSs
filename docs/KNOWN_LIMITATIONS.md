# VHS Master — Known Limitations & Scope Document

This document transparently records technical boundaries, environment constraints, and non-implemented items.

---

## 1. Environment & Final Binary Build Notice
* **Environment Constraint:** The sandbox environment runs Linux x86_64, whereas the required target output is a Windows x64 `.aex` Adobe After Effects plugin compiled with MSVC / Visual Studio 2022.
* **Verification Scope:** Source code syntax, struct layouts, parameter order, C++ render algorithms, and standalone test harnesses have been verified locally. The final binary `.aex` compilation must be executed on a Windows 11 machine with Visual Studio 2022 and After Effects 23.2.1 installed (or via the GitHub Actions build workflow).

---

## 2. Audio Processing Note
* **AE Host Capability:** Audio processing is fully implemented via `PF_Cmd_AUDIO_RENDER` in `src/VHS6RIFT.cpp`. However, After Effects requires RAM preview or audio rendering export to execute audio selectors. Audio processing is not active during static image frame preview calls.

---

## 3. Font Selection & Vector Custom Fonts
* **OSD Font Design:** Text and OSD overlays utilize an embedded 5x7 ASCII bitmap font designed to match vintage camcorder OSD displays. Custom TTF/OTF vector font file loading from disk is not supported natively inside After Effects C++ pixel render calls without external graphics libraries (e.g. FreeType/DirectWrite). Custom text is rendered using the built-in OSD bitmap font.

---

## 4. Proprietary Reference Binary Inaccessible Source
* **Reference Binaries:** Any third-party proprietary `.aex` files located in reference archives were inspected via static string/symbol inspection only. VHS Master does not wrap, hook, or call external `.aex` binaries. All processing logic is 100% original C++ code.
