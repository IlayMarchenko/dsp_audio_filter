#include "audio.h"

static int percent(int x1, int x2) {
    double temp = ((double)x1/x2);
    return (int)(temp*100);
}

static void try_to_add_max(int16_t **arr, int16_t item) {
    int index_of_min = 0;
    for (int i = 1; i < NOISE_CHECK_STEP/100; ++i) {
        if ((*arr)[index_of_min] > (*arr)[i])
            index_of_min = i;
    }
    if ((*arr)[index_of_min] < item)
        (*arr)[index_of_min] = item;
}

static void try_to_add_min(int16_t **arr, int16_t item) {
    int index_of_max = 0;
    for (int i = 1; i < NOISE_CHECK_STEP/100; ++i) {
        if ((*arr)[index_of_max] < (*arr)[i])
            index_of_max = i;
    }
    if ((*arr)[index_of_max] > item)
        (*arr)[index_of_max] = item;
}

static int16_t *new_arr() {
    int16_t *result = malloc(sizeof(int16_t) * (NOISE_CHECK_STEP/100));
    memset(result, 0, 2 * (NOISE_CHECK_STEP/100));
    return result;
}

static int get_amplitude(const int16_t *local_max, const int16_t *local_min) {
    int av_max = 0;
    int av_min = 0;
    int length = NOISE_CHECK_STEP/100;
    for (int i = 0; i < length; ++i) {
        av_max += local_max[i];
        av_min += local_min[i];
    }
    av_max /= length;
    av_min = av_min < 0 ? (av_min/length) * -1 : av_min/length;
    return av_max + av_min;
}

bool is_noise(const int16_t *possible_noise) {
    int extremum = 0;
    int amplitude = 0;
    int16_t *local_max = new_arr(); // length is 1% of section
    int16_t *local_min = new_arr();
    bool is_up = possible_noise[0] < possible_noise[1];
    for (int i = 0; i < NOISE_CHECK_STEP; ++i) {
        if ((is_up && possible_noise[i] <= possible_noise[i + 1]) || (!is_up && possible_noise[i] >= possible_noise[i + 1]))
            continue;
        else {
            if (is_up)
                try_to_add_max(&local_max, possible_noise[i]);
            else
                try_to_add_min(&local_min, possible_noise[i]);
            is_up = !is_up;
            ++extremum;
        }
    }
    amplitude = get_amplitude(local_max, local_min);
    free(local_max);
    free(local_min);

    int a = percent(extremum, EXTREMUM_THRESHOLD) + (100 - percent(amplitude, AMPLITUDE_THRESHOLD));

    return a > 90;
}
