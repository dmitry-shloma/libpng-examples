#ifndef PNGRWDATAMSVCCOMPATIBLE_H
#define PNGRWDATAMSVCCOMPATIBLE_H

#include <png.h>

#define READFILE(file, data, length, check) \
    check=(png_size_t)fread(data, (png_size_t)1, length, file)

#define WRITEFILE(file, data, length, check) \
    check=(png_size_t)fwrite(data, (png_size_t)1, length, file)

/**
 * @brief png_read_data чтение данных для работы с MSVC
 * @param png_ptr
 * @param data
 * @param length
 */
static void png_read_data(png_structp png_ptr,
                              png_bytep data, png_size_t length)
{
    png_size_t check = 0;
    png_voidp io_ptr;

    io_ptr = png_get_io_ptr(png_ptr);
    if (io_ptr != NULL) {
        READFILE((png_FILE_p)io_ptr, data, length, check);
    }

    if (check != length) {
        png_error(png_ptr, "read Error!");
    }
}

/**
 * @brief png_write_data запись данных для работы с MSVC
 * @param png_ptr
 * @param data
 * @param length
 */
static void png_write_data(png_structp png_ptr,
                               png_bytep data, png_size_t length)
{
    png_uint_32 check;

    WRITEFILE((png_FILE_p)png_ptr->io_ptr,  data, length, check);
    if (check != length) {
        png_error(png_ptr, "write Error");
    }
}

#endif // PNGRWDATAMSVCCOMPATIBLE_H
