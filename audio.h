#ifndef AUDIO_H
#define AUDIO_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "plot/pbPlots.h"
#include "plot/supportLib.h"

#define NUMBER_OF_STAGES 3 //number of stages of filtering
#define FILTER_COEFFICIENT 4 // length of buffer in moving average algorithm
#define NOISE_CHECK_STEP 1000 // can't be less then 100
#define EXTREMUM_THRESHOLD 250 // max number of extremums in one noise check step
#define AMPLITUDE_THRESHOLD 6000 // max number of amplitude in one noise check step (sum of max point and (-1 * min point))

#define MAX 1
#define MIN 0

bool is_noise(const int16_t *possible_noise);
void filter_noise_array(int16_t **noise, int length_of_noise);
void create_graphic(const int16_t *buf, int num_of_samples, char *png_name, int width, int height);

#endif //AUDIO_H
