#ifndef FORMATS_PNG_H
#define FORMATS_PNG_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Saves a 16-bit luminosity .png image
    @param output_file_name Target filename
    @param ni Image width (pixels)
    @param nj Image height (pixels)
    @param bounds Image bounds (mm) in the order [xmin, ymin, zmin, xmax, ymax, zmax]
*/
void save_png16L(const char *output_file_name, const int ni, const int nj,
                 const float bounds[6], uint16_t const*const*const pixels);


/** @brief Loads various image parameters from a .png header
    @param filename Target .png image to examine
    @param ni Field to store image width
    @param nj Field to store image height
    @param dx Field to store image width (or NAN if invalid)
    @param dy Field to store image height (or NAN if invalid)
    @param dz Field to store image depth (or NAN if invalid)
*/
void load_png_stats(const char* filename, int* const ni, int* const nj,
                    float* const dx, float* const dy, float* const dz);


/** @brief Copies src onto dst, applying the color (R,G,B)
    and filtering by height.
    @details If src is brighter than depth at a given pixel, then we modify depth and save a colored version of the src pixel into rgb.
    src and depth need to have the same scales on both axes and in terms of bits per mm (for height-map).

    @param src New image's height-map
    @param depth Destination image's height-map
    @param rgb Target RGB image
    @param x Position of src's left edge within depth
    @param y Position of src's bottom edge within depth
    @param ni src's width
    @param nj src's height
    @param R Red color (0-1)
    @param G Green color (0-1)
    @param B Blue color (0-1)
*/
void depth_blit(uint8_t const*const*const src,
                uint8_t* const*const depth,
                uint8_t  (*const*const rgb)[3],
                const int x, const int y,
                const int ni, const int nj,
                const float R, const float G, const float B);

#ifdef __cplusplus
}
#endif

#endif
