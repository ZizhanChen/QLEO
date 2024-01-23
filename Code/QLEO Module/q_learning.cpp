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

#include "q_learning.h"


void q_learning_init(){
    q_state.queue_pos = -1;
    q_state.q_value_prev = 0;
    q_state.q_value = 0;
    q_state.reward = -100;
    //q_state.learning_rate = 0.000001;
    q_state.learning_rate = 0.000001;
    q_state.future_influence = 0.1;
    for(int i = 0; i < 30 ; i++){
        //q_state.weight[i] = -0.01;
        q_state.weight[i] = -0.0001;
    }
}


void print_history(q_state_t q_state){
    int pos = q_state.queue_pos+1;
    int i;
    for (i = 0; i < 10; i++)
    {
        printf("[Q_STATE %d] pos: %d, input_freq: %d, refresh_rate: %d, brightness_history: %d.\n", i, pos, \
        q_state.input_freq[pos], q_state.refresh_rate[pos], q_state.brightness_history[pos]);
        pos++; 
        if(pos >= 10){
            pos -= 10;
        }
    }
}

q_state_t add_new_state(q_state_t q_state, int input_freq, int refresh_rate, int brightness, float emd){
    q_state.queue_pos += 1;

    if (q_state.queue_pos >= 10){
        q_state.queue_pos -= 10;
        q_state.train_flag = true;
    }

    q_state.input_freq[q_state.queue_pos] = input_freq;
    q_state.refresh_rate[q_state.queue_pos] = refresh_rate;
    q_state.brightness_history[q_state.queue_pos] = brightness;
    q_state.emd_shots[q_state.queue_pos] = emd;

    return q_state;
}

q_state_t cal_q_state(q_state_t q_state) {
    q_state.q_value_prev = q_state.q_value;
    double q = 0;
    int pos = q_state.queue_pos+1;
    int i = 0;
    for (int i = 0; i < 10; i++){
        if(pos >= 10){
            pos -= 10;
        }
        q += (q_state.weight[pos] * q_state.input_freq[pos]);
        q += (q_state.weight[pos + 10] * q_state.refresh_rate[pos]);
        q += (q_state.weight[pos + 20] * q_state.brightness_history[pos]);
        
        printf("[Q %d] pos: %d, value: %f\n", i, pos, q);
        pos ++; 
    }
    q_state.q_value = q;
    printf("q_value: %f, q_value_prev %f\n", q_state.q_value, q_state.q_value_prev);
    return q_state;
}

q_state_t cal_diff(q_state_t q_state, int real){
    q_state.diff = q_state.reward + q_state.future_influence*real - q_state.q_value;

    printf("diff: %f\n", q_state.diff);

    return q_state;
}

q_state_t update(q_state_t q_state){
    int i = 0;
    int pos = q_state.queue_pos+1;
    for(i = 0 ;i < 10; i++){
        if(pos >= 10){
            pos -= 10;
        }
        q_state.weight[i] += (q_state.learning_rate * q_state.input_freq[pos] * q_state.diff);
        q_state.weight[i+10] += (q_state.learning_rate * q_state.refresh_rate[pos] * q_state.diff);
        q_state.weight[i+20] += (q_state.learning_rate * q_state.brightness_history[pos] * q_state.diff);
        pos++;
    }
    for(i = 0; i < 30; i++){
        printf("weight %d = %f\n",i, q_state.weight[i]);
    }
    return q_state;
}

void train(semantics_t semantic){
    q_state = add_new_state(q_state, semantic.new_input_freq, semantic.new_refresh_rate, semantic.new_brightness, semantic.new_emd);
    print_history(q_state);

    if(q_state.train_flag == true){
        if(q_state.q_value != 0){
            if(q_state.weight[29] > 0){
                    brightness_value = brightness_value > 10 ? brightness_value - 10 : 0;
                    q_state.reward = 0;
            }
            else if(q_state.weight[29] <= 0){
                brightness_value = (int) (optimal_factor(semantic.new_emd, QOE_THRESHOLD_Q) * 100);
                q_state.reward = -1000;
            }
            q_state = cal_diff(q_state, brightness_value);
            q_state = update(q_state);

            printf("After train, brightness = %d, reward = %d\n", brightness_value, q_state.reward);
            float brightness_factor = float(brightness_value) / 100;
            qoe_q[q_state.queue_pos] = qoe(brightness_factor, semantic.new_emd, true);
            printf("emd = %f, QoE = %f\n", semantic.new_emd, qoe_q[q_state.queue_pos]);
        }
        q_state = cal_q_state(q_state);
    }
}

void tans_map_to_fnw(std::map<int, int> map, signature_t* ret){
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
    semantics_t sem;
    int epoch_interval = 10; 
    int epoch = 0;
    FILE* fp; 
    int count, key, value; 
    int sum = 0; 
    std::map<int, int> content_hist; 
    float emd_pre; 

    h_black_q = (signature_t*)malloc(sizeof (signature_t));
    h_black_q->n = 1;
    int tmp_array[1] = {0}; 
    h_black_q->Features = tmp_array;
    float tmp_array2[1] = {1920*1080};
    h_black_q->Weights = tmp_array2;

    signature_t* content_sig = (signature_t*)malloc(sizeof (signature_t)); 
    content_sig->n = 0; 
    content_sig->Features = (int*) malloc (256 * sizeof(int));
    content_sig->Weights = (float *) malloc (256 * sizeof(float)); 

    //init
    brightness_value = 75;
    q_learning_init();

    while (1) {
        printf("*****************************Epoch %d*******************************\n", epoch);
        //get input freq
        fp = fopen(PATH_INPUT, "r");
        if (fp != NULL) {
            sum = 0;
            while (fscanf(fp, "%d", &count) != EOF){
                //printf("input count: %d\n", count);
                sum += count; 
            }
            fclose(fp);
            sem.new_input_freq = sum;
            printf("new input frequency: %d\n", sem.new_input_freq);
            fp = fopen(PATH_INPUT, "w");
            fclose(fp);
            printf("file cleared\n");
        }
        else
        {
            printf("file_input open failed\n");
        }

        //get display refresh rate
        fp = fopen(PATH_REFRESH, "r");
        if (fp != NULL) {
            sum = 0;
            while (fscanf(fp, "%d", &count) != EOF){
                //printf("refresh count: %d\n", count);
                sum += count; 
            }
            fclose(fp);
            sem.new_refresh_rate = (int) (sum / 60);
            printf("new refresh frequency: %d\n", sem.new_refresh_rate);
            fp = fopen(PATH_REFRESH, "w");
            fclose(fp);
            printf("file cleared\n");
        }
        else
        {
            printf("file_refresh open failed\n");
        }

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
            tans_map_to_fnw(content_hist, content_sig);

            if (content_sig->n != 0) {
                sem.new_emd = emd(content_sig, h_black_q);
                emd_pre = sem.new_emd;
            }
            else {
                sem.new_emd = emd_pre;
            }
            printf("new emd: %f\n", sem.new_emd);
            
            fp = fopen(PATH_CONTENT, "w");
            fclose(fp);
            printf("file cleared\n");
        }
        else
        {
            printf("file_content open failed\n");
        }

        //get brightness value
        sem.new_brightness = brightness_value;

        //train
        train(sem);
        epoch++;
        printf("*****************************Epoch End*******************************\n");

        sleep(epoch_interval);
    } 
    return 0; 
}
