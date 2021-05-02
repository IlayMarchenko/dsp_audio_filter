#ifndef AUDIO_H
#define AUDIO_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "plot/pbPlots.h"
#include "plot/supportLib.h"

#define AMPLITUDE_THRESHOLD 6000
#define EXTREMUM_THRESHOLD 200
#define NOISE_CHECK_STEP 1000 // can't be less then 100
#define FILTER_COEFF_LOW 6
#define FILTER_COEFF_MED 7
#define FILTER_COEFF_HIG 8

int16_t *filter_noise_array(const int16_t *noise, int length_of_noise);
bool is_noise(const int16_t *possible_noise);

int find;

#endif //AUDIO_H
