#include "audio.h"

static int percent(int x1, int x2) {
    double temp = ((double)x1/x2);
    return (int)(temp*100);
}

static void try_to_add(int16_t **arr, int16_t item, uint8_t mode) {
    int index = 0;
    if (mode) {
        for (int i = 1; i < NOISE_CHECK_STEP/100; ++i) {
            if ((*arr)[index] > (*arr)[i])
                index = i;
        }
        if ((*arr)[index] < item)
            (*arr)[index] = item;
    }
    else {
        for (int i = 1; i < NOISE_CHECK_STEP/100; ++i) {
            if ((*arr)[index] < (*arr)[i])
                index = i;
        }
        if ((*arr)[index] > item)
            (*arr)[index] = item;
    }
}

static int16_t *new_arr() {
    int16_t *result = malloc(sizeof(int16_t) * (NOISE_CHECK_STEP/100));
    memset(result, 0, 2 * (NOISE_CHECK_STEP/100));
    return result;
}

static int get_amplitude(const int16_t *local_max, const int16_t *local_min) {
    int av_max = 0;
    int av_min = 0;
    int length = NOISE_CHECK_STEP/100; // 1% of section
    for (int i = 0; i < length; ++i) {
        av_max += local_max[i];
        av_min += local_min[i];
    }
    av_max /= length;
    av_min = av_min < 0 ? (av_min/length) * -1 : av_min/length;
    return av_max + av_min;
}

/**
 * @brief Function 'is_noise' takes array with possible noise and if possibility more then 95% - returns true
 * @param 'raw_value' - constant array with possible noise
 * @return true if possibility that array is noise more then 95%, false if less
 */
bool is_noise(const int16_t *possible_noise) {
    int amplitude;
    int extremum = 0;
    int16_t *local_max = new_arr();
    int16_t *local_min = new_arr();
    bool is_up = possible_noise[0] < possible_noise[1];
    for (int i = 0; i < NOISE_CHECK_STEP; ++i) {
        if ((is_up && possible_noise[i] <= possible_noise[i + 1]) || (!is_up && possible_noise[i] >= possible_noise[i + 1]))
            continue;
        else {
            if (is_up)
                try_to_add(&local_max, possible_noise[i], MAX);
            else
                try_to_add(&local_min, possible_noise[i], MIN);
            is_up = !is_up;
            ++extremum;
        }
    }
    amplitude = get_amplitude(local_max, local_min);
    free(local_max);
    free(local_min);

    return percent(extremum, EXTREMUM_THRESHOLD) + (100 - percent(amplitude, AMPLITUDE_THRESHOLD)) > 95;
}
