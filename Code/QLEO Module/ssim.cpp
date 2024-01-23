#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <termio.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <fcntl.h>
#include <cutils/properties.h>
#include <sys/utsname.h>
#include <cutils/list.h>
#include <log/log.h>
#include <cutils/sockets.h>
#include <cutils/iosched_policy.h>

#include "ssim.h"


float cal_ssim(float lumi_x, float lumi_y) {
    float numerator = (2 * (lumi_x * lumi_y)) + C_ONE;
    float denominator = (lumi_x * lumi_x) + (lumi_y * lumi_y) + C_ONE; 
    float ssim = numerator / denominator;
    return ssim;
}
