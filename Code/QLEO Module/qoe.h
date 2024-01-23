#ifndef _QOE_H
#define _QOE_H

/* DEFINITIONS */
#define ALPHA       -11.56
#define BETA        0.0269
#define GAMMA       13.338
#define PHI         0.9

float qoe(float backlight, float emd, bool active);
float optimal_factor(float emd, float qoe);

#endif