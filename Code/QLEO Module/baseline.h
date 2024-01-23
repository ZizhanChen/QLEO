#ifndef _BASELINE_H
#define _BASELINE_H

#include "emd.h"
#include "qoe.h"

#define PATH_RGB "/data/semantics_rgb"
#define PATH_INPUT "/data/semantics_input"
#define PATH_REFRESH "/data/semantics_refresh"
#define PATH_CONTENT "/data/semantics_content"
#define PATH_BRIGHTNESS "/data/semantics_brightness"

#define BATCH_SIZE_BASE 10

float backlight_base[BATCH_SIZE_BASE];
float qoe_base[BATCH_SIZE_BASE];

signature_t* h_black_base;

#endif
