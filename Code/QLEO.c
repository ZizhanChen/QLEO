/*
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "RL_test semantic"
//#define LOG_NDEBUG 0

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <inttypes.h>

#include <time.h>
#include <fcntl.h>
#include <utils/Timers.h>
#include <sys/time.h>
//#include <utils/Thread.h>

#define portrait 0
#define landscape 1
#define BILIBILI 0
#define WECHAT 1


#define BRIGHTNESS_PATH1 "/dev/semantic_brightness1"
#define BRIGHTNESS_PATH2 "/dev/semantic_brightness2"
#define INPUT_PATH1 "/dev/semantic_input1"
#define INPUT_PATH2 "/dev/semantic_input2"
#define UPDATE_PATH1 "/dev/semantic_update1"
#define UPDATE_PATH2 "/dev/semantic_update2"
#define BRIGHTNESS_PATH "/dev/ttyS4"
//#define HAL_LIBRARY_PATH1 "/dev/semantic_brightness1"
//#define HAL_LIBRARY_PATH2 "/dev/semantic_brightness2"z



#define portrait 0
#define landscape 1

typedef struct Q_state{

    /**
     * Size of the vectors should be 30 for record the frequency of the past 30 seconds
     */

    //available for training
    int train;

    /*The first screen*/
    int inputFreq_s1[10];
    int refreshRate_s1[10];
    int brightnessHistory_s1[10];

    int queue_pos_s1; 

    double Qvalue_prev_s1;
    double Qvalue_s1;

    int reward_s1;

    double weight_s1[30];
    double learningRate_s1;
    double futureInfluence_s1; 

    double difference_s1;

    /*The second screen*/
    int inputFreq_s2[10];
    int refreshRate_s2[10];
    int brightnessHistory_s2[10];

    int queue_pos_s2; 

    double Qvalue_prev_s2;
    double Qvalue_s2;

    int reward_s2;

    double weight_s2[90];
    double learningRate_s2;
    double futureInfluence_s2;

    double difference_s2; 
    
    //portrait or landscape
    //int mode;


    /*
    int (*addInput)(struct Q_state *state);
    int (*addRefresh)(struct Q_state *state);
    int (*addBrightness)(struct Q_state *state);
    int (*update)(struct Q_state *state);

    double (*getQ)(struct Q_state *state);
    */

    /*int getmode(){
        return mode;
    }*/

    /*void setmode(int newMode){
        this->mode = newMode;
    }*/

    



}Q_state_t;

typedef struct Semantic{
    int newInputFreq_s1;
    int newRefreshRate_s1;
    int newBrightness_s1;

    int newInputFreq_s2;
    int newRefreshRate_s2;
    int newBrightness_s2;

    //portrait or landscape
    int mode;

    int brightness_set1;
    int brightness_set2;

    //todo timer
}semantic;

void printHistory(Q_state_t state){
    printf("**************************************************************************\n");
    int pos = state.queue_pos_s1+1;

    int i ;
    for (i = 0 ; i < 10 ; i ++)
    {
        printf("STATE_S1 FREQUENCY %d pos: %d: input: %d update: %d brightness: %d \n", i, pos, \
        state.inputFreq_s1[pos], state.refreshRate_s1[pos], state.brightnessHistory_s1[pos]);
        pos ++; 
        if(pos >= 10){
            pos -= 10;
        }
    }

    printf("**************************************************************************\n");
    pos = state.queue_pos_s2+1;

    i ;
    for (i = 0 ; i < 10 ; i ++)
    {
        printf("STATE_S2 FREQUENCY %d pos: %d: input: %d update: %d brightness: %d \n", i, pos, \
        state.inputFreq_s2[pos], state.refreshRate_s2[pos], state.brightnessHistory_s2[pos]);
        pos ++; 
        if(pos >= 10){
            pos -= 10;
        }
    }
    
    printf("**************************************************************************\n");
}

Q_state_t addnew(Q_state_t state, int input1, int update1, int brightness1, int input2, int update2, int brightness2, int mode){
    if (mode == portrait){
        state.queue_pos_s1 += 1;

        if (state.queue_pos_s1 >= 10){
            state.queue_pos_s1 -= 10;
            state.train = 1;
        }

        state.inputFreq_s1[state.queue_pos_s1] = input1;
        state.refreshRate_s1[state.queue_pos_s1] = update1;
        state.brightnessHistory_s1[state.queue_pos_s1] = brightness1;

        state.queue_pos_s2 += 1;

        if (state.queue_pos_s2 >= 10){
            state.queue_pos_s2 -= 10;
            state.train = 1;
        }

        state.inputFreq_s2[state.queue_pos_s2] = input2;
        state.refreshRate_s2[state.queue_pos_s2] = update2;
        state.brightnessHistory_s2[state.queue_pos_s2] = brightness2;
    }

    return state;


} 

Q_state_t getQ(Q_state_t state){

    state.Qvalue_prev_s1 = state.Qvalue_s1;

    double q = 0;

    int pos = state.queue_pos_s1+1;

    int i = 0;
    for (i = 0 ; i < 10 ; i ++)
    {
        if(pos >= 10){
            pos -= 10;
        }
        q += (state.weight_s1[pos] * state.inputFreq_s1[pos]);
        q += (state.weight_s1[pos + 10] * state.refreshRate_s1[pos]);
        q += (state.weight_s1[pos + 20] * state.brightnessHistory_s1[pos]);
        
        printf("Q_S1 %d pos: %d value: %f\n", i, pos, q);
        pos ++; 
    
    }

    state.Qvalue_s1 = q;

    printf("\nQ_S1 q_value: %f q_value_prev %f\n\n", state.Qvalue_s1, state.Qvalue_prev_s1);

    state.Qvalue_prev_s2 = state.Qvalue_s2;

    q = 0;

    pos = state.queue_pos_s2+1;

    i = 0;
    for (i = 0 ; i < 10 ; i ++)
    {
        if(pos >= 10){
            pos -= 10;
        }
        q += (state.weight_s2[pos] * state.inputFreq_s2[pos]);
        q += (state.weight_s2[pos + 10] * state.refreshRate_s2[pos]);
        q += (state.weight_s2[pos + 20] * state.brightnessHistory_s2[pos]);
        
        printf("Q_S2%d pos: %d value: %f\n", i, pos, q);
        pos ++; 
    
    }

    state.Qvalue_s2 = q;

    printf("\nQ_S2 q_value: %f q_value_prev %f\n\n", state.Qvalue_s2, state.Qvalue_prev_s2);

    return state;

}

Q_state_t getDiff(Q_state_t state, int real_s1, int real_s2){
    //only approximation
    state.difference_s1 = state.reward_s1 + state.futureInfluence_s1*real_s1 - state.Qvalue_s1;

    printf("DIFFERENCE_S1: %f\n", state.difference_s1);

    state.difference_s2 = state.reward_s2 + state.futureInfluence_s2*real_s2 - state.Qvalue_s2;

    printf("DIFFERENCE_S2: %f\n", state.difference_s2);

    return state;
}

Q_state_t update(Q_state_t state){
    int i = 0;
    int pos = state.queue_pos_s1+1;
    for(i = 0 ;i < 10; i++){
        if(pos >= 10){
            pos -= 10;
        }
        state.weight_s1[i] += (state.learningRate_s1*state.inputFreq_s1[pos]*state.difference_s1);
        state.weight_s1[i+10] += (state.learningRate_s1*state.refreshRate_s1[pos]*state.difference_s1);
        state.weight_s1[i+20] += (state.learningRate_s1*state.brightnessHistory_s1[pos]*state.difference_s1);
        pos++;
    }

    for(i = 0; i < 30; i++){
        printf("Weight_S1 %d = %f\n",i, state.weight_s1[i]);
    }

    pos = state.queue_pos_s2+1;
    for(i = 0 ;i < 10; i++){
        if(pos >= 10){
            pos -= 10;
        }
        state.weight_s2[i] += (state.learningRate_s2*state.inputFreq_s2[pos]*state.difference_s2);
        state.weight_s2[i+10] += (state.learningRate_s2*state.refreshRate_s2[pos]*state.difference_s2);
        state.weight_s2[i+20] += (state.learningRate_s2*state.brightnessHistory_s2[pos]*state.difference_s2);
        pos++;
    }

    for(i = 0; i < 30; i++){
        printf("Weight_S2 %d = %f\n",i, state.weight_s2[i]);
    }

    return state;
}

void adjustBrightness(unsigned char bright1, unsigned char bright2){
    int fd = open(BRIGHTNESS_PATH, O_RDWR, S_IRUSR | S_IRGRP | S_IROTH);

    if (fd >= 0) {
        unsigned char buffer[5];
        buffer[0] = bright1;
        buffer[1] = bright2;
        buffer[2] = '\r';
        buffer[3] = '\n';
        write(fd, buffer, 4);
        printf("brightness file ttyS4 write: %s", buffer);
        close(fd);
    }
    else
    {
        printf("brightness file ttyS4 open failed: %d", fd);
    }
}

int main(int argc, char **argv){

    struct timeval tv;
    gettimeofday(&tv, NULL);
    
    printf("second: %ld\n", tv.tv_sec);
    printf("millisecond: %ld\n", tv.tv_sec * 1000 + tv.tv_usec / 1000);
    printf("microsecond: %ld\n", tv.tv_sec * 1000000 + tv.tv_usec);

    semantic semantics;

    //init portrait q_state
    Q_state_t q_state_portrait;

    q_state_portrait.queue_pos_s1 = -1;
    q_state_portrait.learningRate_s1 = 0.000001;
    q_state_portrait.futureInfluence_s1 = 0.1;

    q_state_portrait.queue_pos_s2 = -1;
    q_state_portrait.learningRate_s2 = 0.000001;
    q_state_portrait.futureInfluence_s2 = 0.1;

    for(int i = 0; i < 30 ; i++){
        q_state_portrait.weight_s1[i] = -0.01;
    }

    for(int i = 0; i < 30 ; i++){
        q_state_portrait.weight_s2[i] = -0.01;
    }
    
    q_state_portrait.Qvalue_prev_s1 = 0;
    q_state_portrait.Qvalue_prev_s2 = 0;
    q_state_portrait.Qvalue_s1 = 0;
    q_state_portrait.Qvalue_s2 = 0;


    //bilibili setup
    q_state_portrait.reward_s1 = -100;
    q_state_portrait.reward_s2 = 15;



    time_t t = time(NULL);

    //flag for loop once
    int flag = 0;

    struct tm tm = *localtime(&t);
    printf("now: %d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    printf("RL_test\n");

    int sec = tm.tm_sec;

    int delay = 5;

    unsigned char b1, b2;
    b1 = 75;
    b2 = 75;


    while(1){
        t = time(NULL);
        tm = *localtime(&t);

        printf("now: %d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

        //read input
        FILE * fp = fopen(INPUT_PATH1, "r");

        if (fp != NULL) {
            long size;
            fseek(fp, 0, SEEK_END);
            size = ftell(fp);
            fclose(fp);
            semantics.newInputFreq_s1 = size/2;
            printf("new input frequency 1 : %d and %d\n", size, semantics.newInputFreq_s1);
            fp = fopen(INPUT_PATH1, "w");
            fclose(fp);
            printf("fp clear : %d\n", size);
        }
        else
        {
            printf("input 1 open failed\n");
        }

        fp = fopen(INPUT_PATH2, "r");

        if (fp != NULL) {
            long size;
            fseek(fp, 0, SEEK_END);
            size = ftell(fp);
            fclose(fp);
            semantics.newInputFreq_s2 = size/2;
            printf("new input frequency 2 : %d and %d\n", size, semantics.newInputFreq_s2);
            fp = fopen(INPUT_PATH2, "w");
            fclose(fp);
            printf("fp clear : %d\n", size);
        }
        else
        {
            printf("input 2 open failed\n");
        }

        // read update

        fp = fopen(UPDATE_PATH1, "r");

        if (fp != NULL) {
            long size;
            fseek(fp, 0, SEEK_END);
            size = ftell(fp);
            fclose(fp);
            semantics.newRefreshRate_s1 = size/2;
            printf("new update frequency 1 : %d and %d\n", size, semantics.newRefreshRate_s1);
            fp = fopen(UPDATE_PATH1, "w");
            fclose(fp);
            printf("fp clear : %d\n", size);
        }
        else
        {
            printf("update 1 open failed\n");
        }

        fp = fopen(UPDATE_PATH2, "r");

        if (fp != NULL) {
            long size;
            fseek(fp, 0, SEEK_END);
            size = ftell(fp);
            fclose(fp);
            semantics.newRefreshRate_s2 = size/2;
            printf("new update frequency 2 : %d and %d\n", size, semantics.newRefreshRate_s2);
            fp = fopen(UPDATE_PATH2, "w");
            fclose(fp);
            printf("fp clear : %d\n", size);
        }
        else
        {
            printf("update 2 open failed\n");
        }

        q_state_portrait = addnew(q_state_portrait, semantics.newInputFreq_s1, semantics.newRefreshRate_s1/10, (int)b1 , semantics.newInputFreq_s2, semantics.newRefreshRate_s2/10, (int)b2, portrait);
        printHistory(q_state_portrait);

        if(q_state_portrait.train == 1){
            if(q_state_portrait.Qvalue_s1 != 0){
                //brightness reward bilibili
                if (BILIBILI){
                    if(q_state_portrait.weight_s1[29] > 0 && b1 > 15){
                        b1 -= 15;
                        q_state_portrait.reward_s1 = -1000;
                    }
                    else if(q_state_portrait.weight_s1[29] <= 0){
                        if (b1 != 75){
                            b1 = 75;
                        }
                        q_state_portrait.reward_s1 = 0;
                    }

                    if(q_state_portrait.weight_s2[29] > 0 && b2 > 15){
                        b2 -= 15;
                        q_state_portrait.reward_s2 = 15;
                    }
                    else if(q_state_portrait.weight_s2[29] <= 0)
                    {
                        b2 = 75;
                        q_state_portrait.reward_s1 = 0;
                    }
                }
                else if(WECHAT){
                    if(q_state_portrait.weight_s1[29] > 0 && b1 > 15 && semantics.newInputFreq_s1 == 0){
                        b1 -= 15;
                        q_state_portrait.reward_s1 = 0;
                    }
                    else if(q_state_portrait.weight_s1[29] <= 0 || semantics.newInputFreq_s1 > 0){
                        if (b1 < 75){
                            b1 += 15;
                        }
                        q_state_portrait.reward_s1 = 0;
                    }

                    if(q_state_portrait.weight_s2[29] > 0 && b2 > 15 && semantics.newInputFreq_s2 == 0){
                        b2 -= 15;
                        q_state_portrait.reward_s2 = 0;
                    }
                    else if(q_state_portrait.weight_s2[29] <= 0 || semantics.newInputFreq_s2 > 0)
                    {
                        if (b2 < 75){
                            b2 += 15;
                        }
                        q_state_portrait.reward_s1 = 0;
                    }
                }
                
                adjustBrightness(b1, b2);

                q_state_portrait = getDiff(q_state_portrait, (int)b1, (int)b2);
                q_state_portrait = update(q_state_portrait);

                
                

                printf("*************************\n");
                printf("b1 = %d, b2 = %d, r1 = %d, r2 = %d\n", b1, b2, q_state_portrait.reward_s1, q_state_portrait.reward_s2);
                printf("*************************\n");
            }
            q_state_portrait = getQ(q_state_portrait);
        }


        
        
        //adjustBrightness(b1, b2);
        sleep(delay); 

        t = time(NULL);
        tm = *localtime(&t);  

        printf("sec = %d\n", sec);
        printf("tm.tm_sec = %d\n", tm.tm_sec);

        delay = 10 - tm.tm_sec + sec;

        if(delay > 60){
            delay -= 60;
        }

        sec = tm.tm_sec;

        printf("delay = %d\n", delay);

    }



    /*
    fd = open(HAL_LIBRARY_PATH2, O_RDWR|O_CREAT, S_IRUSR | S_IRGRP | S_IROTH);

    if (fd >= 0) {
        int value = brightness;
        char buffer[20];
        int bytes = sprintf(buffer, "%d\n", value);
        int amt = write(fd, buffer, bytes);
        ALOGE("brightness write2: %d %s", value, buffer);
        close(fd);
    }
    else
    {
        ALOGE("brightness write open failed2: %d", fd);
    }*/

}