use ntsc_rs::{
    NtscEffect,
    ctx::Context,
    yiq_fielding::{BlitInfo, DeinterlaceMode, Rect, Xrgb, YiqField, YiqView},
};
use std::mem::MaybeUninit;

#[repr(C)]
pub struct NtscrsCParams {
    pub chroma_delay: f32,
    pub head_switching: f32,
    pub edge_wave: f32,
    pub composite_noise: f32,
    pub tracking_noise: f32,
    pub strength: f32,
    pub random_seed: i32,
    pub frame_num: u32,
}

#[no_mangle]
pub unsafe extern "C" fn ntscrs_process_argb8(
    params: *const NtscrsCParams,
    pixels: *mut u8,
    width: u32,
    height: u32,
    row_bytes: usize,
) -> i32 {
    if params.is_null() || pixels.is_null() || width == 0 || height == 0 || row_bytes == 0 {
        return -1;
    }
    let p = &*params;

    let strength = p.strength.clamp(0.0, 10.0);
    if strength <= 0.0001 {
        return 0; // Identity bypass
    }

    let w = width as usize;
    let h = height as usize;

    // Save original alpha values to preserve transparency
    let mut original_alphas = vec![255u8; w * h];
    for y in 0..h {
        let row_start = y * row_bytes;
        for x in 0..w {
            let px_idx = row_start + x * 4;
            if px_idx < row_bytes * h {
                original_alphas[y * w + x] = *pixels.add(px_idx); // byte 0 is Alpha in PF_Pixel8 (ARGB)
            }
        }
    }

    let mut effect = NtscEffect::default();
    effect.random_seed = p.random_seed;
    effect.chroma_delay_horizontal = p.chroma_delay * strength * 0.2;

    if p.head_switching > 0.001 {
        effect.head_switching.enabled = true;
        effect.head_switching.settings.height = (p.head_switching * strength * 0.15) as i32;
    } else {
        effect.head_switching.enabled = false;
    }

    effect.luma_smear = p.edge_wave * strength * 0.01;

    if p.composite_noise > 0.001 {
        effect.composite_noise.enabled = true;
        effect.composite_noise.settings.intensity = p.composite_noise * strength * 0.005;
    } else {
        effect.composite_noise.enabled = false;
    }

    // Use the actual ntsc-rs tracking-noise engine. Its output is an animated
    // bottom-anchored waveform/snow disturbance, not a synthetic solid stripe.
    if p.tracking_noise > 0.001 {
        effect.tracking_noise.enabled = true;
        effect.tracking_noise.settings.height = (6.0 + p.tracking_noise * 0.42 * strength) as i32;
        effect.tracking_noise.settings.wave_intensity = p.tracking_noise * strength * 0.45;
        effect.tracking_noise.settings.snow_intensity = p.tracking_noise * strength * 0.0012;
        effect.tracking_noise.settings.snow_anisotropy = 0.85;
        effect.tracking_noise.settings.noise_intensity = p.tracking_noise * strength * 0.006;
    } else {
        effect.tracking_noise.enabled = false;
    }

    let ctx = Context::new();
    let yiq_field = YiqField::Lower;

    let out_buf_size = YiqView::buf_length_for((w, h), yiq_field);
    let mut out_buf = vec![0.0f32; out_buf_size];
    let mut view = YiqView::from_parts(&mut out_buf, (w, h), yiq_field);

    let blit_info = BlitInfo {
        rect: Rect::new(0, 0, h, w),
        destination: (0, 0),
        row_bytes,
        other_buffer_height: h,
        flip_y: false,
    };

    let slice = std::slice::from_raw_parts_mut(pixels as *mut MaybeUninit<u8>, row_bytes * h);

    view.set_from_strided_buffer_maybe_uninit::<Xrgb, u8, _>(&ctx, slice, blit_info, ());
    effect.apply_effect_to_yiq(&ctx, &mut view, p.frame_num as usize, [1.0, 1.0]);
    view.write_to_strided_buffer_maybe_uninit::<Xrgb, u8, _>(
        &ctx,
        slice,
        blit_info,
        DeinterlaceMode::Bob,
        (),
    );

    // Restore original alpha values so transparency is preserved
    for y in 0..h {
        let row_start = y * row_bytes;
        for x in 0..w {
            let px_idx = row_start + x * 4;
            if px_idx < row_bytes * h {
                *pixels.add(px_idx) = original_alphas[y * w + x];
            }
        }
    }

    0
}
