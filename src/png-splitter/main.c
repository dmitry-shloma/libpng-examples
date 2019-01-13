#include <stdlib.h>
#include <stdio.h>

#include <png.h>

void read_png_file(const char *filename, int *width, int *height);
void process_png_file(int width, int height);
void write_png_file(const char *filename, int width, int height);

png_bytep *row_pointers = NULL; //FIXME: сделать локальной

/**
 * @brief read_png_file Чтение png-файла в массив байт
 * @param filename
 * @param width
 * @param height
 */
void read_png_file(const char *filename, int *width, int *height)
{
    FILE *fp_read = fopen(filename, "rb");
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

    *width = png_get_image_width(png_read_struct, info);
    *height = png_get_image_height(png_read_struct, info);

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

    //
    FILE *fp_write = fopen("C:\\12.png", "wb");
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
        png_write_struct, info, *width, *height, bit_depth, color_type,
        PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT);

    png_write_info(png_write_struct, info);

    // Allocate memory for one row (3 bytes per pixel - RGB)
    png_bytep row = (png_bytep) malloc(3 * *width * sizeof(png_byte));

    png_read_row();

//    row_pointers = (png_bytep *) malloc(sizeof(png_bytep) * *height);
//    for (int y = 0; y < *height; ++y) {
//        row_pointers[y] = (png_byte *) malloc(
//                    png_get_rowbytes(png_read_struct, info));
//    }

//    png_read_image(png_read_struct, row_pointers);
//    png_write_image(png_write_struct, row_pointers);



    // Write image data
    int x, y;
    for (y=0 ; y<height ; y++) {
       for (x=0 ; x<width ; x++) {
          setRGB(&(row[x*3]), buffer[y*width + x]);
       }
       png_write_row(png_ptr, row);
    }

    // End write
    png_write_end(png_ptr, NULL);

    free(row_pointers);
    fclose(fp_read);
}

void process_png_file() {
  for(int y = 0; y < height; y++) {
    png_bytep row = row_pointers[y];
    for(int x = 0; x < width; x++) {
      png_bytep px = &(row[x * 4]);
      // Do something awesome for each pixel here...
      //printf("%4d, %4d = RGBA(%3d, %3d, %3d, %3d)\n", x, y, px[0], px[1], px[2], px[3]);
    }
  }
}

void process_png_file(int width, int height)
{
    for (int y = 0; y < height; ++y) {
        png_bytep row = row_pointers[y];
        for (int x = 0; x < width; ++x) {
            png_bytep px = &(row[x * 4]);
        }
    }
}

void write_png_file(const char *filename, int width, int height)
{
    FILE *fp_write = fopen(filename, "wb");
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

    png_infop info = png_create_info_struct(png_write_struct);
    if(!info) {
        printf("png_create_info_struct(): error");
        abort();
    }

    if(setjmp(png_jmpbuf(png_write_struct))) {
        printf("setjmp(): error");
        abort();
    }

    png_init_io(png_write_struct, fp_write);

    png_set_IHDR(png_write_struct, info, width, height, 8, PNG_COLOR_TYPE_RGBA,
                 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
                 PNG_FILTER_TYPE_DEFAULT);
    png_write_info(png_write_struct, info);

    png_write_image(png_write_struct, row_pointers);
//    png_write_image(png_write_struct, row_pointers);
    png_write_end(png_write_struct, NULL);

    for(int y = 0; y < height / 2; ++y) {
        free(row_pointers[y]);
    }
    free(row_pointers);
    fclose(fp_write);
}

void cut_png_image_to_sprites(
        const char *fname,
        unsigned int sprite_width,
        unsigned int sprite_height,
        const char *out_dir)
{
    int width = 0;
    int height = 0;

    read_png_file(fname, &width, &height);


//    width /= 2;
//    height /= 2;
//    process_png_file(width, height);
//    write_png_file("C:\\123.png", width, height * 2);
}

int main()
{
//    const char *fname = "./../../../doc/test_30.png";
    const char *fname = "./../../../doc/64000.png";
    const char *out_dir = "C:/out";

    cut_png_image_to_sprites(fname, 256, 256, out_dir);

    return 0;
}
