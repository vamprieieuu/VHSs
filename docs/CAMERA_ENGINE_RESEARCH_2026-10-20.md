# VHS6RIFT Camera Engine Research — 2026-10-20

The current selector contains five actual JVC analog camcorder models. JVC and Sony are separate manufacturers; Sony's competing consumer format was usually Video8/Hi8, while these selected JVC models are VHS-C or S-VHS-C. The plugin models their image transfer as a multi-part response, not a simple color LUT.

| Engine | Recording family | Intended image response in VHS6RIFT | Grounding |
|---|---|---|---|
| JVC GR-C7U (1986) | VHS-C, early CCD generation | Softest luma, more chroma smear, strongest scanline timing movement and tracking instability | JVC described the GR-C7 as a VHS-C camcorder; contemporaneous reporting notes HQ detail enhancement and white-clip extension. |
| JVC GR-AX220U (1997) | Compact VHS | Consumer HQ look, moderate softness and moderate line instability | JVC's 1997 archive lists the GR-AX220 Compact VHS and HQ circuitry, including detail enhancement, white clipping and YNR. |
| JVC GR-AX920U (1990s) | Compact VHS | Stronger edge response, more controlled exposure and reduced timebase wobble | JVC's archived product page documents VHS format, picture stabilizer and HQ processing. |
| JVC GR-SXM37US | S-VHS-C / S-VHS ET | Higher luma bandwidth, lower chroma smear and steadier timing than standard VHS-C | JVC documents S-VHS and S-VHS ET, plus Digital TBC, Digital CNR and picture stabilizer. |
| JVC GR-SXM740U | S-VHS-C / S-VHS ET | Cleanest/most stable selected response, with lower chroma noise and less wobble | JVC documents S-VHS/S-VHS ET, Digital TBC, Digital CNR and picture stabilizer for this model. |

## What is implemented

The profiles are now deliberately separated strongly enough to compare in a side-by-side A/B: the GR-C7U is much softer/warmer with the broadest chroma smear and highest line instability; the AX220U is muted/green-leaning with moderate softness and noise; the AX920U has firmer edges and stronger contrast with steadier timing; the SXM37US has higher luma detail, stronger edge response and cleaner chroma; the SXM740U has the cleanest/stablest S-VHS-C response with low noise and neutral color. Each profile changes luminance softness/bandwidth, horizontal line timing wobble, chroma bleed and delay, edge response, low-level chroma noise, tonal response, and the amount of NTSC-RS tracking noise. This is a deliberately controllable approximation based on format generation and documented processing features. It is **not** claimed to be a pixel-perfect match to each camera: that requires clean sample clips shot by each exact model, captured through a consistent playback/capture chain, then measured and compared.

## Tracking-damage research basis

Tracking error is modeled as a rolling horizontal band with line-by-line timing displacement, chroma breakup, and animated noise rather than a fixed overlay. This follows descriptions of real tracking failure as horizontal noise bands that can roll vertically and, when severe, obscure most or all of the image. NTSC composite signal simulation also treats horizontal/vertical sync noise as timing instability and VHS tracking as a distinct approximate tape artifact.

## Primary references

- JVC GR-C7U historical/model context: https://camcorderpedia.fandom.com/wiki/JVC_VideoMovie_GR-C7
- JVC 1997 Compact VHS model and HQ feature archive: https://jvc-america.com/pass/archive/press_cam_97.html
- JVC GR-AX920U product archive: https://jvc-america.com/camcorders/gr_ax920u.html
- JVC GR-SXM37US official archived product page: https://support.jvc.com/consumer/product.jsp?modelId=MODL027357&page=1
- JVC GR-SXM740U official archived product page: https://support.jvc.com/consumer/product.jsp?modelId=MODL026778
- Analog.NTSC signal-domain VHS/NTSC model and tracking limitations: https://github.com/kdkd/Analog-NTSC
- Canadian Conservation Institute, VHS digitization artifacts and tracking failure: https://www.canada.ca/en/conservation-institute/services/conservation-preservation-publications/technical-bulletins/digitization-vhs-video-tapes.html
