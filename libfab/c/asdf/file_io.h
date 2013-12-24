#ifndef FILE_IO_H
#define FILE_IO_H

#include <stdio.h>

struct ASDF_;

/** @brief Saves an ASDF to a file
    @param asdf Pointer to an ASDF
    @param filename Name of file
*/
void  asdf_write(struct ASDF_* const asdf, const char* filename);

/** @brief Loads an ASDF from a file
    @param filename Filename
    @returns The loaded ASDF
*/
struct ASDF_*  asdf_read(const char* filename);

#endif
