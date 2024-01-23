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

#include <math.h>


#include "qoe.h"

float qoe(float backlight, float emd, bool active) {
	float alpha = ALPHA;
	float beta = BETA;
	float gamma = GAMMA;
	if (active) {
		float tmp = alpha + (beta * emd) + (gamma * backlight); 
		//float tmp2 = 1 + exp(-tmp);
		return 1 / (1 + exp(-tmp));
	}
	else {
		return PHI; 
	}
}

float optimal_factor(float emd, float qoe){
	float alpha = ALPHA;
	float beta = BETA;
	float gamma = GAMMA;
    return (log(qoe / (1-qoe)) - alpha - beta * emd) / gamma; 
}
