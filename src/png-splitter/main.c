#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <png.h>

int main()
{
    const char *fname_in = "./../../../doc/64000.png";
    const char *fname_out = "./../../../doc/64000_out.png";

    FILE *fp_read = fopen(fname_in, "rb");
    if (!fp_read) {
        printf("fopen(): error");
        abort();
    }

    png_structp png_read_struct = png_create_read_struct(
                PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if(!png_read_struct) {
        printf("png_create_read_struct(): error");
        abort();
    }

    png_infop info = png_create_info_struct(png_read_struct);
    if(!info) {
        printf("png_create_info_struct(): error");
        abort();
    }

    if(setjmp(png_jmpbuf(png_read_struct))) {
        printf("setjmp(): error in png_read_struct");
        abort();
    }

    png_init_io(png_read_struct, fp_read);
    png_read_info(png_read_struct, info);

    int width = png_get_image_width(png_read_struct, info);
    int height = png_get_image_height(png_read_struct, info);

    png_byte color_type = png_get_color_type(png_read_struct, info);
    png_byte bit_depth = png_get_bit_depth(png_read_struct, info);

    if(bit_depth == 16) {
        png_set_strip_16(png_read_struct);
    }

    if(color_type == PNG_COLOR_TYPE_PALETTE) {
        png_set_palette_to_rgb(png_read_struct);
    }

    if(color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) {
        png_set_expand_gray_1_2_4_to_8(png_read_struct);
    }

    if(png_get_valid(png_read_struct, info, PNG_INFO_tRNS)) {
        png_set_tRNS_to_alpha(png_read_struct);
    }

    if(color_type == PNG_COLOR_TYPE_RGB ||
            color_type == PNG_COLOR_TYPE_GRAY ||
            color_type == PNG_COLOR_TYPE_PALETTE) {
        png_set_filler(png_read_struct, 0xFF, PNG_FILLER_AFTER);
    }

    if(color_type == PNG_COLOR_TYPE_GRAY ||
            color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
        png_set_gray_to_rgb(png_read_struct);
    }

    png_read_update_info(png_read_struct, info);


    FILE *fp_write = fopen(fname_out, "wb");
    if (!fp_write) {
        printf("fopen(): error");
        abort();
    }

    png_structp png_write_struct = png_create_write_struct(
                PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if(!png_write_struct) {
        printf("png_create_write_struct(): error");
        abort();
    }

    info = png_create_info_struct(png_write_struct);
    if(!info) {
        printf("png_create_info_struct(): error");
        abort();
    }

    if(setjmp(png_jmpbuf(png_write_struct))) {
        printf("setjmp(): error in png_write_struct");
        abort();
    }

    png_init_io(png_write_struct, fp_write);

    png_set_IHDR(
        png_write_struct, info, width, height, bit_depth, color_type,
        PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT);

    png_write_info(png_write_struct, info);

//    timespec ts_beg, ts_end;
//    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts_beg);

    png_bytep row = (png_bytep) malloc(4 * width * sizeof(png_byte));
    for (int i = 0; i < height; ++i) {
        png_read_row(png_read_struct, row, NULL);
        png_write_row(png_write_struct, row);
        printf("%d %d\n", height, i);
    }

//    row_pointers = (png_bytep *) malloc(sizeof(png_bytep) * *height);
//    for (int y = 0; y < *height; ++y) {
//        row_pointers[y] = (png_byte *) malloc(
//                    png_get_rowbytes(png_read_struct, info));
//    }


    // Write image data
//    int x, y;
//    for (y=0 ; y<height ; y++) {
//       for (x=0 ; x<width ; x++) {
//          setRGB(&(row[x*3]), buffer[y*width + x]);
//       }
//       png_write_row(png_ptr, row);
//    }

    png_write_end(png_write_struct, info);

    free(row);
    fclose(fp_read);
    fclose(fp_write);

//    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts_end);
//    printf("%d sec", (ts_end.tv_sec - ts_beg.tv_sec) + (ts_end.tv_nsec - ts_beg.tv_nsec) / 1e9;
    return 0;
}
