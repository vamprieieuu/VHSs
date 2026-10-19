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

Each profile changes luminance softness/bandwidth, horizontal line timing wobble, chroma bleed and delay, edge response, low-level chroma noise, tonal response, and the amount of NTSC-RS tracking noise. This is a deliberately controllable approximation based on format generation and documented processing features. It is **not** claimed to be a pixel-perfect match to each camera: that requires clean sample clips shot by each exact model, captured through a consistent playback/capture chain, then measured and compared.

## Primary references

- JVC GR-C7U historical/model context: https://camcorderpedia.fandom.com/wiki/JVC_VideoMovie_GR-C7
- JVC 1997 Compact VHS model and HQ feature archive: https://jvc-america.com/pass/archive/press_cam_97.html
- JVC GR-AX920U product archive: https://jvc-america.com/camcorders/gr_ax920u.html
- JVC GR-SXM37US official archived product page: https://support.jvc.com/consumer/product.jsp?modelId=MODL027357&page=1
- JVC GR-SXM740U official archived product page: https://support.jvc.com/consumer/product.jsp?modelId=MODL026778
