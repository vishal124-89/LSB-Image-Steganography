#ifndef DECODE_H
#define DECODE_H

#include "types.h"  
#include "common.h"  

/* macro */
#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4
#define FILE_EXTN_SIZE 32
#define FILE_SIZE 32


/* Structure to store information required */
typedef struct _DecodeInfo
{

    /* stego image info */
    char *stego_image_fname;
    FILE *fptr_stego_image;
    uint image_data_size;
    char image_data[MAX_IMAGE_BUF_SIZE];

    /* decoded file info */
    char *decoded_fname;
    FILE *fptr_decoded_file;
    char extn_decoded_file[MAX_FILE_SUFFIX];
    char decode_data[MAX_SECRET_BUF_SIZE];
} DecodeInfo;


/* Decoding function prototype */

/* Read and validate Decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* perform decoding*/
Status do_decoding(DecodeInfo *decInfo);

/* open decode files */
Status open_decode_files(DecodeInfo *decInfo);


/* Decode magic string */
Status decode_magic_string(DecodeInfo *decInfo);


/* Decode secret file extn */
Status decode_secret_file_extn(uint extn_size, DecodeInfo *decInfo);

/* Decode secret file extn size */
Status decode_secret_file_extn_size(DecodeInfo *decInfo);

/* Decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

/* Decode secret file data */
Status decode_secret_file_data(DecodeInfo *decInfo);

/* Decode byte from lsb */
Status decode_byte_from_lsb(char *decode_data, char *image_data);

/* Decode size from lsb */
Status decode_size_from_lsb(char *buffer, DecodeInfo *decInfo);

#endif
