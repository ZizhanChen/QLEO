#ifndef __Q_LEARNING_H__
#define __Q_LEARNING_H__

#define BATCH_SIZE_Q 10
#define QOE_THRESHOLD_Q 0.9

#define PATH_INPUT "/data/semantics_input"
#define PATH_REFRESH "/data/semantics_refresh"
#define PATH_CONTENT "/data/semantics_content"
#define PATH_BRIGHTNESS "/data/semantics_brightness"

#include "emd.h"
#include "qoe.h"

signature_t* h_black_q;
float qoe_q[BATCH_SIZE_Q];


typedef struct qstate{
    bool train_flag;    //available for training

    int input_freq[BATCH_SIZE_Q];
    int refresh_rate[BATCH_SIZE_Q];
    float emd_shots[BATCH_SIZE_Q];
    int brightness_history[BATCH_SIZE_Q];

    int queue_pos; 

    double q_value_prev;
    double q_value;

    int reward;

    double weight[4 * BATCH_SIZE_Q];
    double learning_rate;
    double future_influence; 

    double diff;    
}q_state_t;

typedef struct semantics{
    int new_input_freq;
    int new_refresh_rate;
    int new_brightness;
    float new_emd; 
    //int brightness_set;
}semantics_t;

static q_state_t q_state;
int brightness_value;


#endif