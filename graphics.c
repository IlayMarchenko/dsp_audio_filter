#include "audio.h"

/**
 * @brief Function 'create_graphic' creates graphic by points from array 'buf'
 * @param 'length_of_buf' - length of parameter 'buf'
 * @param 'png_name' - name of future .png file
 * @param 'width' - width in pixels of future .png file
 * @param 'height' - height in pixels of future .png file
 */
void create_graphic(const int16_t *buf, int length_of_buf, char *png_name, int width, int height) {
    double *x = malloc(sizeof(double) * length_of_buf);
    double *y = malloc(sizeof(double) * length_of_buf);

    for (int i = 0; i < length_of_buf; ++i)
        x[i] = i;

    for (int i = 0; i < length_of_buf; ++i)
        y[i] = buf[i];

    RGBABitmapImageReference *canvasReference = CreateRGBABitmapImageReference();
    DrawScatterPlot(canvasReference, width, height, x, length_of_buf, y, length_of_buf);

    size_t length;
    double *pngdata = ConvertToPNG(&length, canvasReference->image);
    WriteToFile(pngdata, length, png_name);
    DeleteImage(canvasReference->image);
}
