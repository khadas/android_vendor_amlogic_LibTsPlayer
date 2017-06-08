#ifndef _UTIL_H_
#define _UTIL_H_

typedef enum {
    OUTPUT_MODE_480I = 0,
    OUTPUT_MODE_480P,
    OUTPUT_MODE_576I,
    OUTPUT_MODE_576P,
    OUTPUT_MODE_720P,
    OUTPUT_MODE_1080I,
    OUTPUT_MODE_1080P,
    OUTPUT_MODE_4K2K,
    OUTPUT_MODE_4K2KSMPTE,
}OUTPUT_MODE;

OUTPUT_MODE get_display_mode();
void getPosition(OUTPUT_MODE output_mode, int *x, int *y, int *width, int *height);
#endif
