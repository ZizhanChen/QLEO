#ifndef _DBS_AL_H
#define _DBS_AL_H


#define QOE_THRESHOLD 0.9
#define ACC_THRESHOLD 0.8
#define AMBIENT_L 1000
#define BATCH_SIZE 10
#define ETA 4
#define ALPHA_DBS 10
#define D_MIN 5

#include "emd.h"
#include "qoe.h"

#define PATH_INPUT "/data/semantics_input"
#define PATH_REFRESH "/data/semantics_refresh"
#define PATH_CONTENT "/data/semantics_content"
#define PATH_BRIGHTNESS "/data/semantics_brightness"

signature_t* h_black_al;


float factors[BATCH_SIZE];
float qoe_al[BATCH_SIZE];

int dbs_al(signature_t* frames, int cnt);


#endif