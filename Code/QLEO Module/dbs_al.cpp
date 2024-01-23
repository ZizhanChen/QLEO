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

#include "dbs_al.h"

void median(signature_t** frames, int m, int n, signature_t* ret);
void sum(signature_t* frame1, signature_t* frame2);
void div(signature_t* frame, int n);
void tansMapToFnW(std::map<int, int> map, signature_t* ret);
void tansFnWtoMap(signature_t* frame, std::map<int, int>& map);

int dbs_al(signature_t** frames, int cnt){
    int i,j,k;
    int n;
    float emd_pre, emd_cur, emd_shot;
    int s_pre; 
    float factor_pre, factor_cur;

    h_black_al = (signature_t*)malloc(sizeof (signature_t));
    h_black_al->n = 1;
    int tmp_array[1] = {0}; 
    h_black_al->Features = tmp_array;
    float tmp_array2[1] = {1920*1080};
    h_black_al->Weights = tmp_array2;

    signature_t* mh_shot = (signature_t*)malloc(sizeof (signature_t));
    mh_shot->n = 0; 
    mh_shot->Features = (int*) malloc (256 * sizeof(int));
    mh_shot->Weights = (float *) malloc (256 * sizeof(float)); 

    if (cnt < BATCH_SIZE)
        n = cnt;
    else
        n = BATCH_SIZE;
    emd_pre = emd(frames[0], frames[1]);
    s_pre = 0;
    factor_pre = 0; 
    for (k = 1; k < n; k++){
        if (k+1<n) {
            emd_cur = emd(frames[k], frames[k+1]);
        } else {
            emd_cur = emd(frames[k], frames[0]);
        }
        //if (emd_cur > ETA && (emd_cur / emd_pre) > ALPHA_DBS){
        //    if (k+1-s_pre > D_MIN) {
                median(frames, s_pre, k+1, mh_shot);
                emd_shot = emd(mh_shot, h_black_al);
        //    }
        //}
        printf("emd_shot is %f\n", emd_shot);
        factor_cur = optimal_factor(emd_shot, QOE_THRESHOLD);
        //printf("factor_cur is %f\n", factor_cur);
        if ((emd_pre >= 144 && emd_pre < 172) && (emd_cur >= 28 && emd_cur < 56)) {
            factor_cur = factor_pre;
        }
        for (i = s_pre; i <= k; i++){
            factors[i] = factor_cur;
            //qoe_al[i] = qoe(factor_cur, emd(frames[k], h_black_al), true);
            qoe_al[i] = qoe(factor_cur, emd_shot, true);
        }
        s_pre = k+1;
        factor_pre = factor_cur;
        emd_pre = emd_shot;
    }
    return 0;
}

void median(signature_t** frames, int m, int n, signature_t* ret){
    for (int i = m; i < n; i++){
        sum(ret, frames[i]);
    }
    div(ret, n-m);
}

void sum(signature_t* frame1, signature_t* frame2){
    std::map<int, int> tmp_map1, tmp_map2;
    int n2; 
    tansFnWtoMap(frame1, tmp_map1);
    
    tansFnWtoMap(frame2, tmp_map2);
    for (auto itr2 = tmp_map2.begin(); itr2 != tmp_map2.end(); itr2++){
        auto itr1 = tmp_map1.find(itr2->first);
        if (itr1 == tmp_map1.end())
            tmp_map1[itr2->first] = 1;
        else
        {
            itr1->second += itr2->second;
        }
    }
    tansMapToFnW(tmp_map1, frame1);
}

void div(signature_t* frame, int m) {
    int n = frame->n;  
    for (int i = 0; i < n; i++) {
        frame->Weights[i] = frame->Weights[i] / m; 
    }
}

void tansMapToFnW(std::map<int, int> map, signature_t* ret){
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

void tansFnWtoMap(signature_t* frame, std::map<int, int>& map) {
    int* features =  frame->Features;
    float* weights = frame->Weights;
    int n = frame->n; 
    for (int i = 0; i < n; i++){
        map[features[i]] = (int) weights[i];
    }
}

int main() {
    int epoch_interval = 10; 
    int epoch = 0;
    FILE* fp; 
    int count, key, value; 
    int sum = 0; 
    std::map<int, int> content_hist; 
    std::map<int, int> hist_pre;
    signature_t* shots[BATCH_SIZE];
    int index = 0;

    for (int i = 0; i < BATCH_SIZE; i++){
        shots[i] = (signature_t*)malloc(sizeof (signature_t)); 
        shots[i]->n = 0; 
        shots[i]->Features = (int*) malloc (256 * sizeof(int));
        shots[i]->Weights = (float *) malloc (256 * sizeof(float)); 
    }

    //h_black_al = (signature_t*)malloc(sizeof (signature_t));
    //h_black_al->n = 1;
    //int tmp_array[1] = {0}; 
    //h_black_al->Features = tmp_array;
    //float tmp_array2[1] = {1920*1080};
    //h_black_al->Weights = tmp_array2;

    //signature_t* content_sig = (signature_t*)malloc(sizeof (signature_t)); 
    //content_sig->n = 0; 
    //content_sig->Features = (int*) malloc (256 * sizeof(int));
    //content_sig->Weights = (float *) malloc (256 * sizeof(float)); 
    
    while (1) {
        printf("*****************************Epoch %d*******************************\n", epoch);
        //get content hist
        fp = fopen(PATH_CONTENT, "r");
        if (fp != NULL) {
            content_hist.clear();
            while (fscanf(fp, "%d%d", &key, &value) != EOF){
                //printf("key: %d, value: %d\n", key, value);
                content_hist[key] = value; 
            }
            fclose(fp);
            //tansMapToFnW(content_hist, content_sig);
            if (!content_hist.empty()) {

                tansMapToFnW(content_hist, shots[index]);
                //update hist_pre
                hist_pre.clear();
                for (auto it = content_hist.begin(); it != content_hist.end(); it++) {
                    hist_pre[it->first] = it->second;
                }
            }
            else {
                tansMapToFnW(hist_pre, shots[index]);
            }
            printf("new shots.n: %d\n", shots[index]->n);
            fp = fopen(PATH_CONTENT, "w");
            fclose(fp);
            printf("file cleared\n");
        }
        else
        {
            printf("file_content open failed\n");
        }

        //train
        printf("before train, current shots.n: [");
        for (int i = 0; i < BATCH_SIZE; i++) {
            printf("%d, ", shots[i]->n);
        }
        printf("]\n");
        dbs_al(shots, BATCH_SIZE);

        epoch++;
        index++;

        if (index > 9) {
            index = 0;
        }
        printf("after train, factors: [");
        for (int i = 0; i < BATCH_SIZE; i++) {
            printf("%f, ", factors[i]);
        }
        printf("]\n");
        printf("after train, qoes: [");
        for (int i = 0; i < BATCH_SIZE; i++) {
            printf("%f, ", qoe_al[i]);
        }
        printf("]\n");
        printf("*****************************Epoch End*******************************\n");

        sleep(epoch_interval);
    } 
    return 0; 
}