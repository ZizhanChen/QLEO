#ifndef __SSIM_H__
#define __SSIM_H__

#define K_ONE 0.000001;
#define L 255;
#define C_ONE (0.000001 * 255 * 0.000001 * 255);

float cal_ssim(float lumi_x, float lumi_y);

#endif