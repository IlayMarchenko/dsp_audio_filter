#include "audio.h"

static void add_samples(int16_t **buf, FILE *pipeout) {
    if (is_noise(*buf))
        filter_noise_array(buf, NOISE_CHECK_STEP);
    for (int i = 0; i < NOISE_CHECK_STEP; ++i) {
        fwrite(&(*buf)[i], 2, 1, pipeout);
    }
}

static void add_to_filter_buf(int16_t **filtered_buf, const int16_t *buf, int *fbuf_curr_index) {
    for (int i = 0; i < NOISE_CHECK_STEP; ++i)
        (*filtered_buf)[(*fbuf_curr_index)++] = buf[i];
}

static int number_of_samples(char *request) {
    FILE *file;
    int result = 0;
    file = popen(request, "r");
    while (1) {
        int16_t sample = 0;
        unsigned long count = fread(&sample, 2, 1, file);
        if (count != 1)
            break;
        ++result;
    }
    pclose(file);
    return result;
}

static void create_requests(int current_stage, char **request_to_write, char **request_to_read, char **argv) {
    if (current_stage == 0) {
        asprintf(request_to_read, "ffmpeg -i %s -f s16le -ac 1 -", argv[1]);
        asprintf(request_to_write, "ffmpeg -y -f s16le -ar 44100 -ac 1 -i - %s", argv[2]);
    }
    else if (current_stage % 2) {
        asprintf(request_to_read, "ffmpeg -i %s -f s16le -ac 1 -", argv[2]);
        asprintf(request_to_write, "ffmpeg -y -f s16le -ar 44100 -ac 1 -i - temp.wav");
    }
    else {
        asprintf(request_to_read, "ffmpeg -i temp.wav -f s16le -ac 1 -");
        asprintf(request_to_write, "ffmpeg -y -f s16le -ar 44100 -ac 1 -i - %s", argv[2]);
    }
}

int main(int argc, char **argv) {

    int16_t *filtered_buf;
    char *request_to_read = NULL;

    asprintf(&request_to_read, "ffmpeg -i %s -f s16le -ac 1 -", argv[1]);
    int num_of_samples = number_of_samples(request_to_read);

    for (int stage = 0; stage < NUMBER_OF_STAGES; ++stage) {

        char *request_to_write = NULL;
        int16_t *buf = malloc(sizeof(int16_t) * NOISE_CHECK_STEP);
        filtered_buf = malloc(sizeof(int16_t) * num_of_samples);

        create_requests(stage, &request_to_write, &request_to_read, argv);

        FILE *pipein  = popen(request_to_read, "r");
        FILE *pipeout = popen(request_to_write, "w");

        int16_t sample;
        int buf_curr_index = 0;
        int fbuf_curr_index = 0;

        while (1) {
            unsigned long count = fread(&sample, 2, 1, pipein);
            if (count != 1) {
                if (buf_curr_index > 1) {
                    add_samples(&buf, pipeout);
                    add_to_filter_buf(&filtered_buf, buf, &fbuf_curr_index);
                }
                break;
            }
            if (buf_curr_index < NOISE_CHECK_STEP)
                buf[buf_curr_index++] = sample;
            else {
                add_samples(&buf, pipeout);
                add_to_filter_buf(&filtered_buf, buf, &fbuf_curr_index);
                buf_curr_index = 0;
            }
        }

        if (stage + 1 < argc - 1) {
            free(buf);
            free(filtered_buf);
        }
        free(request_to_write);
        free(request_to_read);

        pclose(pipein);
        pclose(pipeout);
    }

    create_graphic(filtered_buf, num_of_samples, "graphic_of_filtered_audio.png", 5080, 3080);
    return 0;
}
