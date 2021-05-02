#include "audio.h"

/**
 * @brief Function 'filter' is representation of moving average filter in c programming language
 * @param 'raw_value' - fresh raw value from audio file that will be filtered
 * @param 'buffer' - array with previous values from audio file
 * @param 'length_of_buffer' - length of previous parameter 'buffer[]'
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

int16_t *filter_noise_array(const int16_t *noise, int length_of_noise) {
    int16_t *last_items = malloc(sizeof(uint16_t) * FILTER_COEFF_HIG);
    int16_t *result = malloc(sizeof(int16_t) * length_of_noise);
    for (int i = 0; i < length_of_noise; ++i) {
        if (i < FILTER_COEFF_HIG) {
            result[i] = last_items[i] = noise[i];
            continue;
        }
        if ((i >= length_of_noise / 4 && i <= length_of_noise / 3) || (i >= (length_of_noise/3)*2 && i <= length_of_noise / 4 + length_of_noise / 2)) { // 25% - 33.3% || 66.6% - 75%
            int16_t filtered_sample = filter(noise[i], &last_items, FILTER_COEFF_MED);
            result[i] = filtered_sample;
        }
        else if (i > length_of_noise / 3 && i < (length_of_noise/3)*2) { // 33.3% - 66.6%
            int16_t filtered_sample = filter(noise[i], &last_items, FILTER_COEFF_HIG);
            result[i] = filtered_sample;
        }
        else { // 0% - 25% || 75% - 100%
            int16_t filtered_sample = filter(noise[i], &last_items, FILTER_COEFF_LOW);
            result[i] = filtered_sample;
        }
        if (result[i] < -4500) {
            printf("\n%d\n", find);
        }
        ++find;
    }
    free(last_items);
    return result;
}
