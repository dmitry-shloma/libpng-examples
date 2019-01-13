#ifndef PNGSPLITTERHELPER_H
#define PNGSPLITTERHELPER_H

#include <stdlib.h>

#include "pngrwdatamsvccompatible.h"

void create_png_read_stream(FILE *fp_read, png_structp *png_read_struct,
                            png_infop *read_info_ptr, png_infop *end_info_ptr,
                            int *width, int *height);

void create_png_write_stream(FILE *fp_write, png_structp *png_write_struct,
                             png_infop *write_info, int width, int height);

void get_png_size(const char *filename, int *width, int *height); // HACK

void create_png_read_stream(FILE *fp_read, png_structp *png_read_struct,
                            png_infop *read_info, int *width, int *height)
{
    *png_read_struct = png_create_read_struct(
                PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if(!*png_read_struct) {
        printf("\'%s\' error in function: %s\n",
               "png_create_read_struct()", Q_FUNC_INFO);
        abort();
    }

    *read_info = png_create_info_struct(*png_read_struct);
    if(!*read_info) {
        printf("\'%s\' error in function: %s\n",
               "png_create_info_struct()", Q_FUNC_INFO);
        abort();
   }

    if(setjmp(png_jmpbuf(*png_read_struct)) != 0) {
        printf("\'%s\' error in function: %s\n",
               "setjmp()", Q_FUNC_INFO);
        abort();
    }

    png_init_io(*png_read_struct, fp_read);

    // функции для работы под MSVC
    png_set_read_status_fn(*png_read_struct, png_read_status_ptr_NULL);
    png_set_read_fn(*png_read_struct, (png_voidp)fp_read, png_read_data);
    //

    png_read_info(*png_read_struct, *read_info);

    *width = png_get_image_width(*png_read_struct, *read_info);
    *height = png_get_image_height(*png_read_struct, *read_info);
    png_byte color_type = png_get_color_type(*png_read_struct, *read_info);
    png_byte bit_depth = png_get_bit_depth(*png_read_struct, *read_info);

    if(bit_depth == 16) {
        png_set_strip_16(*png_read_struct);
    }

    if(color_type == PNG_COLOR_TYPE_PALETTE) {
        png_set_palette_to_rgb(*png_read_struct);
    }

    if(color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) {
        png_set_expand_gray_1_2_4_to_8(*png_read_struct);
    }

    if(png_get_valid(*png_read_struct, *read_info, PNG_INFO_tRNS)) {
        png_set_tRNS_to_alpha(*png_read_struct);
    }

    if(color_type == PNG_COLOR_TYPE_RGB ||
            color_type == PNG_COLOR_TYPE_GRAY ||
            color_type == PNG_COLOR_TYPE_PALETTE) {
        png_set_filler(*png_read_struct, 0xFF, PNG_FILLER_AFTER);
    }

    if(color_type == PNG_COLOR_TYPE_GRAY ||
            color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
        png_set_gray_to_rgb(*png_read_struct);
    }

    png_read_update_info(*png_read_struct, *read_info);
}

void create_png_write_stream(FILE *fp_write, png_structp *png_write_struct,
                             png_infop *write_info, int width, int height)
{
    *png_write_struct = png_create_write_struct(
                PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if(!*png_write_struct) {
        printf("\'%s\' error in function: %s\n",
               "png_create_write_struct()", Q_FUNC_INFO);
        abort();
    }

    *write_info = png_create_info_struct(*png_write_struct);
    if(!*write_info) {
        printf("\'%s\' error in function: %s\n",
               "png_create_info_struct()", Q_FUNC_INFO);
        abort();
    }

    if(setjmp(png_jmpbuf(*png_write_struct)) != 0) {
        printf("\'%s\' error in function: %s\n",
               "setjmp()", Q_FUNC_INFO);
        abort();
    }

    png_init_io(*png_write_struct, fp_write);

// функции для работы под MSVC
    png_set_write_status_fn(*png_write_struct, png_write_status_ptr_NULL);
    png_set_write_fn(*png_write_struct, (png_voidp)fp_write, png_write_data, NULL);
//
    png_set_IHDR(*png_write_struct, *write_info, width, height, 8,
                 PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    png_write_info(*png_write_struct, *write_info);
}

void get_png_width(const char *filename, int *width) // HACK
{
    // get width of source png image
    FILE *fp_read = fopen(filename, "rb");
    if (!fp_read) {
        printf("\'%s\' error in function: %s\n", "fopen()", Q_FUNC_INFO);
        abort();
    }

    png_structp png_read_struct = png_create_read_struct(
                PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if(!png_read_struct) {
        printf("\'%s\' error in function: %s\n",
               "png_create_read_struct()", Q_FUNC_INFO);
        abort();
    }

    png_infop read_info = png_create_info_struct(png_read_struct);
    if(!read_info) {
        printf("\'%s\' error in function: %s\n",
               "png_create_info_struct()", Q_FUNC_INFO);
        abort();
   }

    if(setjmp(png_jmpbuf(png_read_struct)) != 0) {
        printf("\'%s\' error in function: %s\n",
               "setjmp()", Q_FUNC_INFO);
        abort();
    }

    png_init_io(png_read_struct, fp_read);

    // функции для работы под MSVC
    png_set_read_status_fn(png_read_struct, png_read_status_ptr_NULL);
    png_set_read_fn(png_read_struct, (png_voidp)fp_read, png_read_data);
    //

    png_read_info(png_read_struct, read_info);
    *width = png_get_image_width(png_read_struct, read_info);
    fclose(fp_read);
}

#endif // PNGSPLITTERHELPER_H
