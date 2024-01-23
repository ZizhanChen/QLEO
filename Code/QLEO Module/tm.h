#ifndef _TM_H
#define _TM_H

#include "emd.h"
#include "qoe.h"

#define PATH_RGB "/data/semantics_rgb"
#define PATH_INPUT "/data/semantics_input"
#define PATH_REFRESH "/data/semantics_refresh"
#define PATH_CONTENT "/data/semantics_content"
#define PATH_BRIGHTNESS "/data/semantics_brightness"

#define BATCH_SIZE_TM 10

float backlight_tm[BATCH_SIZE_TM];
float qoe_tm[BATCH_SIZE_TM];

signature_t* h_black_tm;

float cal_energy(float backlight);

#endif