#include "audio.h"


static void add_samples(int16_t *buf, FILE *pipeout) {
    if (is_noise(buf))
        buf = filter_noise_array(buf, NOISE_CHECK_STEP);
    for (int i = 0; i < NOISE_CHECK_STEP; ++i) {
        fwrite(&buf[i], 2, 1, pipeout);
    }
}

static int number_of_samples(char *request) {
    FILE *file;
    int result = 0;
    file = popen(request, "r");
    while (1) {
        int16_t sample = 0;
        int count = fread(&sample, 2, 1, file); // read one 2-byte sample
        if (count != 1)
            break;
        ++result;
    }
    pclose(file);
    return result;
}


int main(int argc, char **argv) {

    find = 0;

    char *request_to_read = NULL;
    char *request_to_write = NULL;
    asprintf(&request_to_read, "ffmpeg -i %s -f s16le -ac 1 -", argv[1]);
    asprintf(&request_to_write, "ffmpeg -y -f s16le -ar 44100 -ac 1 -i - %s", argv[2]);

    int num_of_samples = number_of_samples(request_to_read);

    int16_t *buf = malloc(sizeof(int16_t) * NOISE_CHECK_STEP);
    int16_t *filtered_buf = malloc(sizeof(int16_t) * num_of_samples);


    FILE *pipein;
    FILE *pipeout;
    pipein  = popen(request_to_read, "r");
    pipeout = popen(request_to_write, "w");

    int16_t sample;
    int n = 0;
    int a = 0;

    while (1) {
        int count = fread(&sample, 2, 1, pipein);
        if (count != 1) {
            if (n > 1) {
                add_samples(buf, pipeout);
                if (is_noise(buf))
                    buf = filter_noise_array(buf, NOISE_CHECK_STEP);
                for (int i = 0; i < NOISE_CHECK_STEP; ++i) {
                    filtered_buf[a++] = buf[i];
                }
            }
            break;
        }
        if (n < NOISE_CHECK_STEP)
            buf[n++] = sample;
        else {
            add_samples(buf, pipeout);
            if (is_noise(buf))
                buf = filter_noise_array(buf, NOISE_CHECK_STEP);
            for (int i = 0; i < NOISE_CHECK_STEP; ++i) {
                filtered_buf[a++] = buf[i];
            }
            n = 0;
        }
    }

//     Close input and output pipes
    pclose(pipein);
    pclose(pipeout);

    double *x = malloc(sizeof(double) * num_of_samples);
    double *y = malloc(sizeof(double) * num_of_samples);

    for (int i = 0; i < num_of_samples; ++i) {
        x[i] = i;

    }

    for (int i = 0; i < num_of_samples; ++i) {
        y[i] = filtered_buf[i];
    }

    RGBABitmapImageReference *canvasReference = CreateRGBABitmapImageReference();
    DrawScatterPlot(canvasReference, 5080, 3080, x, num_of_samples, y, num_of_samples);

    size_t length;
    double *pngdata = ConvertToPNG(&length, canvasReference->image);
    WriteToFile(pngdata, length, "clean_hello.png");
    DeleteImage(canvasReference->image);

    return 0;
}

