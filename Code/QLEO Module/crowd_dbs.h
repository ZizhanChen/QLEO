#ifndef _CROWD_DBS_H
#define _CROWD_DBS_H

#define A 0.9
#define C 0.14
#define BATCH_SIZE 10

#include "emd.h"

#define PATH_INPUT "/data/semantics_input"
#define PATH_REFRESH "/data/semantics_refresh"
#define PATH_CONTENT "/data/semantics_content"
#define PATH_BRIGHTNESS "/data/semantics_brightness"

signature_t* h_black_crowd;


float backlight_opt[BATCH_SIZE];
float qoe_crowd[BATCH_SIZE];

int crowd_dbs(signature_t* shots, int cnt);

#endif