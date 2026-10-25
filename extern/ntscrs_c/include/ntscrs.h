#pragma once
#ifndef NTSCRS_H
#define NTSCRS_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ntscrs_pipeline ntscrs_pipeline;

typedef struct {
  float chroma_delay_x;
  float chroma_delay_y;
  float luma_smear;
  float composite_noise;
  float head_switching;
  uint32_t seed;
} ntscrs_settings;

ntscrs_pipeline* ntscrs_create(uint32_t width, uint32_t height);
void ntscrs_destroy(ntscrs_pipeline* pipeline);
int32_t ntscrs_process_frame_argb8(ntscrs_pipeline* pipeline, const uint8_t* src, uint8_t* dst, uint32_t rowbytes, const ntscrs_settings* settings);

#ifdef __cplusplus
}
#endif

#endif // NTSCRS_H
