#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <png.h>

#include "formats/png.h"

void save_png16L(const char *output_file_name, const int ni, const int nj,
                 const float bounds[6], uint16_t const*const*const pixels)
{

    // Open up a file for writing
    FILE* output = fopen(output_file_name, "wb");

    // Create a png pointer without any special callbacks
    png_structp png_ptr = png_create_write_struct(
        PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    // Create an info pointer
    png_infop info_ptr = png_create_info_struct(png_ptr);

    // Set physical vars
    png_set_IHDR(png_ptr, info_ptr, ni, nj, 16, PNG_COLOR_TYPE_GRAY,
                 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE,
                 PNG_FILTER_TYPE_BASE);

    if (!isnan(bounds[2]) && !isnan(bounds[5])) {
        png_text text[2];

        char zmin[10];
        snprintf(zmin, 10, "%g", bounds[2]);
        text[0].compression = PNG_TEXT_COMPRESSION_NONE;
        text[0].key = "zmin";
        text[0].text = zmin;

        char zmax[10];
        snprintf(zmax, 10, "%g", bounds[5]);
        text[1].compression = PNG_TEXT_COMPRESSION_NONE;
        text[1].key = "zmax";
        text[1].text = zmax;

        png_set_text(png_ptr, info_ptr, text, 2);
    }

    // bounds should be mm in the order {xmin, ymin, zmin, xmax, ymax, zmax}
    png_set_pHYs(png_ptr, info_ptr,
                 1000 * ni / (bounds[3]-bounds[0]),
                 1000 * nj / (bounds[4]-bounds[1]),
                 PNG_RESOLUTION_METER);

    // Write the PNG to file
    png_init_io(png_ptr, output);
    png_set_rows(png_ptr, info_ptr, (png_bytepp)pixels);
    png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_SWAP_ENDIAN, NULL);
    fclose(output);

    png_destroy_write_struct(&png_ptr, &info_ptr);
}



/*  depth_blit
 *
 *  Copies src onto dst, applying the color (R,G,B)
 *  and filtering by height.
 */
void depth_blit(uint8_t const*const*const src,
                uint8_t* const*const depth,
                uint8_t  (*const*const rgb)[3],
                const int x, const int y, const int ni, const int nj,
                const float R, const float G, const float B)
{
    for (int j=0; j < nj; ++j) {
        for (int i=0; i < ni; ++i) {

            if (src[j][i] > depth[y+j][x+i]) {
                depth[y+j][x+i] = src[j][i];

                rgb[y+j][x+i][0] = R*src[j][i];
                rgb[y+j][x+i][1] = G*src[j][i];
                rgb[y+j][x+i][2] = B*src[j][i];
            }
        }
    }
}


void load_png_stats(const char* filename, int* const ni, int* const nj,
                    float* const dx, float* const dy, float* const dz)
{
    png_structp png_ptr = png_create_read_struct(
        PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop info_ptr = png_create_info_struct(png_ptr);

    FILE* input = fopen(filename, "rb");

    png_init_io(png_ptr, input);
    png_read_info(png_ptr, info_ptr);

    // Find image dimensions
    *ni = png_get_image_width(png_ptr, info_ptr);
    *nj = png_get_image_height(png_ptr, info_ptr);

    // Find image's physical size
    png_uint_32 res_x, res_y;
    int unit_type;
    png_get_pHYs(png_ptr, info_ptr, &res_x, &res_y, &unit_type);

    if (unit_type == PNG_RESOLUTION_METER) {
        *dx = (1000. * (*ni)) / res_x;
        *dy = (1000. * (*nj)) / res_y;
    } else {
        *dx = NAN;
        *dy = NAN;
    }

    // Look for zmin and zmax values in image text chunks
    png_textp text_ptr;
    int num_text;
    int z_found = 0;
    float zmax = 0, zmin = 0;
    png_get_text(png_ptr, info_ptr, &text_ptr, &num_text);
    for (int t=0; t < num_text; ++t) {
        if (!strcmp(text_ptr[t].key, "zmax")) {
            sscanf(text_ptr[t].text, "%g", &zmax);
            z_found |= 1;
        }  else if (!strcmp(text_ptr[t].key, "zmin")) {
            sscanf(text_ptr[t].text, "%g", &zmin);
            z_found |= 2;
        }
    }

    if (z_found == 3)   *dz = zmax - zmin;
    else                *dz = NAN;

    fclose(input);

    // Free the various structures allocated to read the png
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
}


void load_png(const char* filename, uint16_t *const*const img)
{

    png_structp png_ptr = png_create_read_struct(
        PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop info_ptr = png_create_info_struct(png_ptr);

    FILE* input = fopen(filename, "rb");

    png_init_io(png_ptr, input);
    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

    png_bytep* rows = png_get_rows(png_ptr, info_ptr);

    int ni = png_get_image_width(png_ptr, info_ptr),
        nj = png_get_image_height(png_ptr, info_ptr);

    // Get a color palette if this is a paletted png
    png_colorp palette = NULL;
    int num_palette = 0;
    if (png_get_color_type(png_ptr, info_ptr) == PNG_COLOR_TYPE_PALETTE) {
        png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette);
    }

    // Get various properties that determine how we interpret the image
    int bit_depth = png_get_bit_depth(png_ptr, info_ptr);
    png_byte color_type = png_get_color_type(png_ptr, info_ptr);

    _Bool error = false;
    for (int j=0; j < nj && !error; ++j) {
        for (int i=0; i < ni && !error; ++i) {
            if (bit_depth == 1)
            {
                if (color_type == PNG_COLOR_TYPE_PALETTE) {
                    int value = (rows[j][i/8] & (1 << (7-(i%8)))) ? 1 : 0;
                    png_color color = palette[value];
                    int sum = color.red + color.green + color.blue;
                    img[nj-j-1][i] = (sum << 8) / 3;
                } else if (color_type == PNG_COLOR_TYPE_GRAY) {
                    img[nj-j-1][i] = (rows[j][i/8] & (1 << (7-(i%8)))) ? 65535 : 0;
                } else {
                    error = true;
                }
            }
            else if (bit_depth == 8)
            {
                if (color_type == PNG_COLOR_TYPE_GRAY) {
                    img[nj-j-1][i] = rows[j][i] << 8;
                } else if (color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
                    img[nj-j-1][i] = rows[j][i*2] << 8;
                } else if (color_type == PNG_COLOR_TYPE_RGB) {
                    int sum = rows[j][i*3] + rows[j][i*3+1] + rows[j][i*3+2];
                    img[nj-j-1][i] = (sum << 8) / 3;
                } else if (color_type == PNG_COLOR_TYPE_RGB_ALPHA) {
                    int sum = rows[j][i*4] + rows[j][i*4+1] + rows[j][i*4+2];
                    img[nj-j-1][i] = (sum << 8) / 3;
                } else if (color_type == PNG_COLOR_TYPE_PALETTE) {
                    png_color color = palette[rows[j][i]];
                    int sum = color.red + color.green + color.blue;
                    img[nj-j-1][i] = (sum << 8) / 3;
                } else {
                    error = true;
                }
            }
            else if (bit_depth == 16)
            {
                if (color_type == PNG_COLOR_TYPE_GRAY) {
                    img[nj-j-1][i] = (rows[j][i*2] << 8) + rows[j][i];
                } else {
                    error = true;
                }
            } else {
                error = true;
            }
        }
    }

    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    fclose(input);

    if (error) {
        printf("Could not read png with bit depth %i and color type %i\n",
                bit_depth, color_type);
    }

}
