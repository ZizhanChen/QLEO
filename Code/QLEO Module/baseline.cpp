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

#include "baseline.h"

void tans_map_to_fnw4(std::map<int, int> map, signature_t* ret){
    std::map<int, int>::iterator it;
    int i = 0;
    int n = 0; 
    for (it = map.begin(); it != map.end(); it++) {
        ret->Features[i] = it->first;
        ret->Weights[i] = it->second;
        i++;
        ret->n = ++n; 
    }
}

int main() {
    int epoch_interval = 10; 
    int epoch = 0;
    FILE* fp; 
    int count, key, value; 
    int sum = 0; 
    int index = 0;

    std::map<int, int> content_hist; 
    float shot_emds[BATCH_SIZE_BASE];
    float emd_pre = 0; 

    h_black_base = (signature_t*)malloc(sizeof (signature_t));
    h_black_base->n = 1;
    int tmp_array[1] = {0}; 
    h_black_base->Features = tmp_array;
    float tmp_array2[1] = {1920*1080};
    h_black_base->Weights = tmp_array2;
    
    signature_t* content_sig = (signature_t*)malloc(sizeof (signature_t)); 
    content_sig->n = 0; 
    content_sig->Features = (int*) malloc (256 * sizeof(int));
    content_sig->Weights = (float *) malloc (256 * sizeof(float)); 

    while (1) {
        printf("*****************************Epoch %d*******************************\n", epoch);
        //get content hist
        fp = fopen(PATH_CONTENT, "r");
        if (fp != NULL) {
            content_hist.clear();
            content_sig->n = 0;
            memset(content_sig->Features, 0, 256 * sizeof(int));
            memset(content_sig->Weights, 0, 256 * sizeof(float));
            while (fscanf(fp, "%d%d", &key, &value) != EOF){
                //printf("key: %d, value: %d\n", key, value);
                content_hist[key] = value; 
            }
            fclose(fp);
            tans_map_to_fnw4(content_hist, content_sig);
            if (content_sig->n != 0) {
                shot_emds[index] = emd(content_sig, h_black_base);
                emd_pre = shot_emds[index];
            }
            else {
                shot_emds[index] = emd_pre;
            }
            printf("new emd: %f\n", shot_emds[index]);
            
            fp = fopen(PATH_CONTENT, "w");
            fclose(fp);
            printf("file cleared\n");
        }
        else
        {
            printf("file_content open failed\n");
        }

        //train
        backlight_base[index] = 0;
        qoe_base[index] = qoe(backlight_base[index], shot_emds[index], true);

        epoch++;
        index++;

        if (index > 9) {
            index = 0;
        }
        printf("after train, factors: [");
        for (int i = 0; i < BATCH_SIZE_BASE; i++) {
            printf("%f, ", backlight_base[i]);
        }
        printf("]\n");
        printf("after train, qoes: [");
        for (int i = 0; i < BATCH_SIZE_BASE; i++) {
            printf("%f, ", qoe_base[i]);
        }
        printf("]\n");
        printf("*****************************Epoch End*******************************\n");

        sleep(epoch_interval);
    } 
    return 0; 
}