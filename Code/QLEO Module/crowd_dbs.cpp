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

#include "crowd_dbs.h"
#include "emd.h"
#include "qoe.h"

int arg_max(float * array, int cnt);
float max(float a, float b);


int crowd_dbs(float* shot_emds, int cnt){
    int i, i_max;
    int n;
    //h_black_crowd = (signature_t*)malloc(sizeof (signature_t));
    //h_black_crowd->n = 1;
    //int tmp_array[1] = {0}; 
    //h_black_crowd->Features = tmp_array;
    //float tmp_array2[1] = {1920*1080};
    //h_black_crowd->Weights = tmp_array2;
    
    float backlight_init[BATCH_SIZE];

    if (cnt < BATCH_SIZE)
        n = cnt;
    else
        n = BATCH_SIZE;
    for (i = 0; i < n; i++){
        //float emd_shot = emd(shots[i], h_black_crowd);
        backlight_init[i] = optimal_factor(shot_emds[i], A);
    }
    for (i = 0; i < n; i++){
        i_max = arg_max(backlight_init, n);
        backlight_opt[i_max] = backlight_init[i_max];
        qoe_crowd[i_max] = qoe(backlight_opt[i_max], shot_emds[i_max], true);
        backlight_init[i_max] = 0;
        if (i_max + 1 < n) {
            if (backlight_init[i_max + 1] != 0){
                backlight_init[i_max + 1] = max(backlight_init[i_max + 1], (1 - C) * (backlight_opt[i_max]));
            }
        }
    }
    return 0;
}

int arg_max(float * array, int cnt){
    int i;
    int index = 0;
    float max = 0;
    for (i = 0; i < cnt; i++) {
        if (array[i] > max){
            max = array[i];
            index = i;
        }
    }
    return index; 
}

float max(float a, float b){
    return a > b ? a : b;
}

void tans_map_to_fnw2(std::map<int, int> map, signature_t* ret){
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
    std::map<int, int> content_hist; 
    //signature_t* shots[BATCH_SIZE];
    float shot_emds[BATCH_SIZE];
    float emd_pre = 0; 
    int index = 0;

    //for (int i = 0; i < BATCH_SIZE; i++){
    //    shots[i] = (signature_t*)malloc(sizeof (signature_t)); 
    //    shots[i]->n = 0; 
    //    shots[i]->Features = (int*) malloc (256 * sizeof(int));
    //    shots[i]->Weights = (float *) malloc (256 * sizeof(float)); 
    //}

    h_black_crowd = (signature_t*)malloc(sizeof (signature_t));
    h_black_crowd->n = 1;
    int tmp_array[1] = {0}; 
    h_black_crowd->Features = tmp_array;
    float tmp_array2[1] = {1920*1080};
    h_black_crowd->Weights = tmp_array2;

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
            tans_map_to_fnw2(content_hist, content_sig);
            if (content_sig->n != 0) {
                shot_emds[index] = emd(content_sig, h_black_crowd);
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
        printf("before train, current emds: [");
        for (int i = 0; i < BATCH_SIZE; i++) {
            printf("%f, ", shot_emds[i]);
        }
        printf("]\n");
        crowd_dbs(shot_emds, BATCH_SIZE);

        epoch++;
        index++;

        if (index > 9) {
            index = 0;
        }
        printf("after train, backlight_opt: [");
        for (int i = 0; i < BATCH_SIZE; i++) {
            printf("%f, ", backlight_opt[i]);
        }
        printf("]\n");
        printf("after train, qoe_crowd: [");
        for (int i = 0; i < BATCH_SIZE; i++) {
            printf("%f, ", qoe_crowd[i]);
        }
        printf("]\n");
        printf("*****************************Epoch End*******************************\n");

        sleep(epoch_interval);
    } 
    return 0; 
}