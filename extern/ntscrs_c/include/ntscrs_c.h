#ifndef NTSCRS_C_H
#define NTSCRS_C_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct NtscrsCParams {
    float chroma_delay;      // Horizontal chroma delay (0..100)
    float head_switching;    // Head switching height / enable (0..100)
    float edge_wave;         // Luma smear / edge wave (0..100)
    float composite_noise;   // Native NTSC-RS composite noise intensity (0..100)
    float composite_noise_frequency; // Native NTSC-RS composite noise frequency (0..1)
    int32_t composite_noise_detail;  // Native NTSC-RS composite noise octaves (1..5)
    float tracking_noise;    // Native ntsc-rs VHS tracking noise amount (0..100)
    float tracking_height;   // Tracking disturbance height as percent of frame (0..100)
    float strength;          // Overall engine strength multiplier (0..1)
    int32_t random_seed;     // Random seed
    uint32_t frame_num;      // Frame index
} NtscrsCParams;

// Processes a 32-bit ARGB pixel buffer (PF_Pixel8: A, R, G, B order).
// Returns 0 on success, non-zero on failure.
int32_t ntscrs_process_argb8(
    const NtscrsCParams* params,
    uint8_t* pixels,
    uint32_t width,
    uint32_t height,
    size_t row_bytes
);

#ifdef __cplusplus
}
#endif

#endif // NTSCRS_C_H
