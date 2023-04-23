#ifndef EMOKIT_THREAD_H_
#define EMOKIT_THREAD_H_

#include "include.h"
#include <queue>
#include <pthread.h>
#include <unistd.h>



void emokit_thread_init();
void push_raw_data(struct emokit_raw_frame_data emokit_raw_frame_dat);
bool pop_raw_data();


#endif
