#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _DecodeInfo
{
    /* Source Image info */
    char *stego_image_fname;
    char *output_fname;

    /* Secret File Info */
    FILE *fptr_stego_image;
    FILE *fptr_output;

    char image_data[MAX_IMAGE_BUF_SIZE];
 
    int extn_secret_file_size;
    char extn_secret_file_decode[MAX_FILE_SUFFIX];
    int size_secret_file_decode;

   
} DecodeInfo;


/* Read and validate Decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status open_files_decode(DecodeInfo *decInfo);

/* Decode Magic String */
Status decode_magic_string(DecodeInfo *decInfo);

/*Decode secret file extension size*/
Status decode_secret_file_extn_size(DecodeInfo *decInfo);

/* Decode secret file extenstion */
Status decode_secret_file_extn(DecodeInfo *decInfo);

/* Decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

/* Decode secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo);

/* Decode data from image*/
Status decode_data_from_image(char *data, int size, DecodeInfo *decInfo);

/* Decode a byte into LSB of image data array */
char decode_byte_from_lsb(char *image_buffer);

/*Decode a size into LSB of image data array */
int decode_size_from_lsb(char *image_buffer);

/*Close all files for decode*/
Status close_all_files_decode(DecodeInfo *decInfo);

#endif
