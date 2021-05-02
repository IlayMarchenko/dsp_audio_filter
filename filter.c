#include "audio.h"

/**
 * @brief Function 'filter' is representation of moving average filter in c programming language
 * @param 'raw_value' - fresh raw value from audio file that will be filtered
 * @param 'buffer' - pointer to array with previous values from audio file
 * @param 'length_of_buffer' - length of previous parameter 'buffer'
 * @return int16_t filtered value
 */
int16_t filter(int16_t raw_value, int16_t **buffer, uint8_t length_of_buffer) {
    int16_t current_value = 0;
    (*buffer)[length_of_buffer - 1] = raw_value;
    for(uint8_t i = 0; i < length_of_buffer; i++) {
        current_value += (*buffer)[i];
    }
    current_value /= length_of_buffer;
    for(uint8_t i = 0; i < length_of_buffer; i++) {
        (*buffer)[i] = (*buffer)[i+1];
    }
    return current_value;
}

/**
 * @brief Function 'filter_noise_array' takes array with noise and pass it to filter
 * @param 'noise' - pointer to array with samples of noise
 * @param 'length_of_noise' - length of previous parameter 'noise'
 */
void filter_noise_array(int16_t **noise, int length_of_noise) {
    int16_t *last_items = malloc(sizeof(uint16_t) * FILTER_COEFFICIENT);
    for (int i = 0; i < length_of_noise; ++i) {
        if (i < FILTER_COEFFICIENT) {
            last_items[i] = (*noise)[i];
            continue;
        }
        (*noise)[i] = filter((*noise)[i], &last_items, FILTER_COEFFICIENT);
    }
    free(last_items);
}
