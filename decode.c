#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "types.h"
#include "common.h"

// Function to read and validate command-line arguments
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    // Check if the stego image file has a .bmp extension
    if (strstr(argv[2], ".bmp") != NULL)
    {
        decInfo->stego_image_fname = argv[2];
    }
    else
    {
        printf("Error: The entered file is not a .bmp file\n");
        return e_failure;
    }

    // Check if the output file has a .txt extension
    if (argv[3] != NULL)
    {
        if (strstr(argv[3], ".txt") != NULL)
        {
            decInfo->decoded_fname = argv[3];
        }
        else
        {
            printf("Error: Please provide a correct file extension for the output\n");
            return e_failure;
        }
    }
    else
    {
        printf("INFO: Output file not mentioned. Creating decoded.txt\n");
        decInfo->decoded_fname = "decoded.txt";
    }

    return e_success;
}

// Function to perform decoding process
Status do_decoding(DecodeInfo *decInfo)
{
    printf("INFO: Opening required files\n");

    // Open stego image and output files
    if (open_decode_files(decInfo) == e_success)
    {
        printf("INFO: Done\n");

        // Decode magic string from stego image
        if (decode_magic_string(decInfo) == e_success)
        {
	    printf("INFO: Done\n");

            // Decode secret file extension size
            if (decode_secret_file_extn_size(decInfo) == e_success)
            {
	       	printf("INFO: Done\n");
                // Decode secret file extension
                if (decode_secret_file_extn(decInfo->image_data_size, decInfo) == e_success)
                {
		    printf("INFO: Done\n");

                    // Decode secret file size
                    if (decode_secret_file_size(decInfo) == e_success)
                    {
		    	printf("INFO: Done\n");
                        printf("INFO: The secret file size is %d\n", decInfo->image_data_size);

                        // Decode secret file data
                        if (decode_secret_file_data(decInfo) == e_success)
                        {
			    printf("INFO: Done\n");
                            return e_success;
                        }
                        else
                        {
                            printf("Error: Decoding the secret data failed\n");
                            return e_failure;
                        }
                    }
                    else
                    {
                        printf("Error: Secret file size not decoded successfully\n");
                        return e_failure;
                    }
                }
                else
                {
                    printf("Error: File extension not decoded successfully\n");
                    return e_failure;
                }
            }
            else
            {
                printf("Error: File extension size not decoded successfully\n");
                return e_failure;
            }
        }
        else
        {
            printf("Error: Magic string not successfully decoded\n");
            return e_failure;
        }
    }
    else
    {
        printf("Error: Files were not opened successfully\n");
        return e_failure;
    }
}

// Function to open stego image and output files
Status open_decode_files(DecodeInfo *decInfo)
{
    // Open stego image file
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");
    if ((decInfo->fptr_stego_image) == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);
        return e_failure;
    }
    printf("INFO: Opened %s\n", decInfo->stego_image_fname);

    // Open output file for decoded data
    decInfo->fptr_decoded_file = fopen(decInfo->decoded_fname, "w");
    if ((decInfo->fptr_decoded_file) == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->decoded_fname);
        return e_failure;
    }
    printf("INFO: Opened %s\n", decInfo->decoded_fname);

    return e_success;
}

// Function to decode magic string from stego image
Status decode_magic_string(DecodeInfo *decInfo)
{
    printf("INFO: Decoding Magic String Signature\n");
    fseek(decInfo->fptr_stego_image, 54, SEEK_SET);
    char magic_string[3];

    for (int i = 0; i < 2; i++)
    {
        fread(decInfo->image_data, sizeof(char), 8, decInfo->fptr_stego_image);
        decode_byte_from_lsb(&magic_string[i], decInfo->image_data);
    }

    //printf("INFO: Magic string -> %s\n", magic_string);

    // Check if the magic string is correct
    if (strcmp(magic_string, MAGIC_STRING) == 0)
    {
        return e_success;
    }

    return e_failure;
}

// Function to decode a byte from the least significant bit (LSB) of an image data byte
Status decode_byte_from_lsb(char *decode_data, char *image_data)
{
    decode_data[0] = 0;

    for (int i = 0; i < 8; i++)
    {
        decode_data[0] = (decode_data[0] | (((image_data[i]) & (0x01)) << i));
    }

    return e_success;
}

// Function to decode the size of the secret file extension
Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    printf("INFO: Decoding Output File Extenstion\n");
    char str[FILE_EXTN_SIZE];
    fread(str, sizeof(char), 32, decInfo->fptr_stego_image);
    decode_size_from_lsb(str, decInfo);
   // printf("INFO: The extension size is %d\n", decInfo->image_data_size);
    return e_success;
}

// Function to decode size from the least significant bit (LSB) of a buffer
Status decode_size_from_lsb(char *buffer, DecodeInfo *decInfo)
{
    decInfo->image_data_size = 0;

    for (int j = 0; j < FILE_EXTN_SIZE; j++)
    {
        (decInfo->image_data_size) = ((decInfo->image_data_size) | ((buffer[j] & 0x01) << j));
    }

    return e_success;
}

// Function to decode the secret file extension
Status decode_secret_file_extn(uint extn_size, DecodeInfo *decInfo)
{
    printf("INFO: The decoded file extention is ->");
    for (int i = 0; i < extn_size; i++)
    {
        fread(decInfo->image_data, sizeof(char), 8, decInfo->fptr_stego_image);

        if (decode_byte_from_lsb(decInfo->decode_data, decInfo->image_data) == e_success)
        {
            (decInfo->extn_decoded_file[i]) = (decInfo->decode_data[0]);
            printf("%c", decInfo->extn_decoded_file[i]);
        }
        else
        {
            return e_failure;
        }
    }
    printf("\n");

    decInfo->extn_decoded_file[extn_size] = '\0';
    return e_success;
}
// Function to decode the size of the secret file
Status decode_secret_file_size(DecodeInfo *decInfo)
{
    printf("INFO: Decoding decoded.txt file size\n");
    char str[32];
    // Read 32 bytes to get the size information
    fread(str, sizeof(char), 32, decInfo->fptr_stego_image);
    // Decode the size from the LSB of the buffer
    decode_size_from_lsb(str, decInfo);
    // printf("The size is %d\n", decInfo->image_data_size); // Optional: Uncomment if you want to print the size
    return e_success;
}

// Function to decode the secret file data
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    printf("INFO: Decoding decoded.txt File data\n");
    for (int i = 0; i < decInfo->image_data_size; i++)
    {
        // Read image data into the buffer
        fread(decInfo->image_data, sizeof(char), MAX_IMAGE_BUF_SIZE, decInfo->fptr_stego_image);

        // Decode a byte from the LSB of the image data
        if (decode_byte_from_lsb(decInfo->decode_data, decInfo->image_data) == e_success)
        {
            // Write the decoded byte to the output file
            fwrite(decInfo->decode_data, sizeof(char), 1, decInfo->fptr_decoded_file);
        }
        else
        {
            return e_failure;
        }
    }

    return e_success;
}




