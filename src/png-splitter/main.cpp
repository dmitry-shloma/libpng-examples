#include <stdlib.h>
#include <stdio.h>

#include <png.h>

#include "pngsplitterhelper.h"

/**
 * @brief split_png_to_vertical_tiles
 * @param filename
 * @param tile_width
 * @param out_dir
 */
void split_png_to_vertical_tiles(const char *filename, int tile_width,
                                 const char *out_dir);

void split_png_to_vertical_tiles(const char *filename, int tile_width,
                                 const char *out_dir)
{
    int width = 0;
    get_png_width(filename, &width);

    int tiles_count_max = width / tile_width;

    for (int tile_id = 0; tile_id < tiles_count_max; ++tile_id) {

        FILE *fp_read = fopen(filename, "rb");
        if (!fp_read) {
            printf("\'%s\' error in function: %s\n", "fopen()", Q_FUNC_INFO);
            abort();
        }

        png_structp png_read_struct = NULL;
        png_infop read_info = NULL;
        int width = 0;
        int height = 0;
        create_png_read_stream(fp_read, &png_read_struct, &read_info,
                               &width, &height);

        png_uint_32 row_bytes = png_get_rowbytes(png_read_struct, read_info);
        png_bytep row_buf = (png_bytep) png_malloc(png_read_struct, row_bytes);

        // HARDCODE: 4 is sizeof RGBA
        png_bytep tile_row_buf = (png_bytep) png_malloc(
                    png_read_struct, tile_width * 4);

        char out_file[255] = "";
        sprintf(out_file, "%s/%d.png", out_dir, tile_id);

        FILE *fp_write = fopen(out_file, "wb");
        if (!fp_write) {
            printf("\'%s\' error in function: %s\n", "fopen()", Q_FUNC_INFO);
            abort();
        }

        png_structp png_write_struct = NULL;
        png_infop write_info = NULL;
        create_png_write_stream(fp_write, &png_write_struct, &write_info,
                                tile_width, height);

        for (int y = 0; y < height; ++y) {
            png_read_rows(png_read_struct, (png_bytepp) &row_buf,
                          png_bytepp_NULL, 1);
            memcpy(tile_row_buf, row_buf + tile_id * tile_width * 4,
                   tile_width * 4);
            png_write_rows(png_write_struct, (png_bytepp) &tile_row_buf, 1);
        }

        png_write_end(png_write_struct, NULL);
        png_write_struct = NULL;

        fclose(fp_read);
        fclose(fp_write);
    }
}

int main(int argc, char *argv[])
{
    const char *filename = argv[1];
    int tile_width = atoi(argv[2]);
    const char *out_dir = argv[3];

    split_png_to_vertical_tiles(filename, tile_width, out_dir);

    return 0;
}
