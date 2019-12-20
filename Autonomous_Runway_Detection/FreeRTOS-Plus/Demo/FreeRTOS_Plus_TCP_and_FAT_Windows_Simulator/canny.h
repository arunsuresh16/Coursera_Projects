#define _USE_MATH_DEFINES
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include "FreeRTOSConfig.h"
 
#include "queue.h"

extern QueueHandle_t edge_encrypt_queue_handle;

#define MAX_BRIGHTNESS 255
 
/*
 * Loading part taken from
 * http://www.vbforums.com/showthread.php?t=261522
 * BMP info:
 * http://en.wikipedia.org/wiki/BMP_file_format
 *
 * Note: the magic number has been removed from the bmpfile_header_t
 * structure since it causes alignment problems
 *     bmpfile_magic_t should be written/read first
 * followed by the
 *     bmpfile_header_t
 * [this avoids compiler-specific alignment pragmas etc.]
 */
 
typedef struct {
    uint8_t magic[2];
} bmpfile_magic_t;
 
typedef struct {
    uint32_t filesz;
    uint16_t creator1;
    uint16_t creator2;
    uint32_t bmp_offset;
} bmpfile_header_t;
 
typedef struct {
    uint32_t header_sz;
    int32_t  width;
    int32_t  height;
    uint16_t nplanes;
    uint16_t bitspp;
    uint32_t compress_type;
    uint32_t bmp_bytesz;
    int32_t  hres;
    int32_t  vres;
    uint32_t ncolors;
    uint32_t nimpcolors;
} bitmap_info_header_t;
 
typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t nothing;
} rgb_t;
 
// Use short int instead `unsigned char' so that we can
// store negative values.
typedef short int pixel_t;

#define DATA_SIZE_DELIMITER		'~' //Unique character which can be used as delimiter to define the message size
#define NUM_OF_FILES			25

#ifdef USE_LENNA_IMAGE
	#define INPUT_FILE_DIR			"Original_Bmp_Files_lenna"
#else
	#define INPUT_FILE_DIR			"Original_Bmp_Files"
#endif

#ifdef USE_LENNA_IMAGE
	#define OUTPUT_FILE_DIR			"Edge_Detected_Bmp_Files_lenna"
#else
	#define OUTPUT_FILE_DIR			"Edge_Detected_Bmp_Files"
#endif
#define FILE_NAME_PATTERN		"bmp_file ("
#define MAX_CHAR_FILE_PATH		50

typedef struct {
	pixel_t out_data[DATA_SIZE];
 } pack_data_t ;
 
pixel_t *load_bmp(const char *filename,
                  bitmap_info_header_t *bitmapInfoHeader);
				  
bool save_bmp(const char *filename, const bitmap_info_header_t *bmp_ih,
              const pixel_t *data);
			  
void convolution(const pixel_t *in, pixel_t *out, const float *kernel,
                 const int nx, const int ny, const int kn,
                 const bool normalize);
				 
void gaussian_filter(const pixel_t *in, pixel_t *out,
                     const int nx, const int ny, const float sigma);
					 
pixel_t *canny_edge_detection(const pixel_t *in,
                              const bitmap_info_header_t *bmp_ih,
                              const int tmin, const int tmax,
                              const float sigma);

/* Edge Detection task to detect the edges from the images using canny filter */
void vEdgeDetectionTask(void* par);
