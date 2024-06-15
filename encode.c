#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

// Function to read and validate command line arguments for encoding
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    // Check if the second command line argument has a ".bmp" extension
    if (strstr(argv[2], ".bmp") != NULL)
    {
	encInfo->src_image_fname = argv[2];
    }
    else
    {
	printf("Error: Please provide a .bmp file extension\n");
	return e_failure;
    }

    // Check if the third command line argument is provided
    if (argv[3] != NULL)
    {
	encInfo->secret_fname = argv[3];
	// Extract the extension from the secret file name
	strcpy(encInfo->extn_secret_file, strchr(argv[3], '.'));
    }
    else
    {
	printf("Error: Please provide a file\n");
	return e_failure;
    }

    // Check if the fourth command line argument is provided
    if (argv[4] != NULL)
    {
	if (strstr(argv[4], ".bmp") != NULL)
	{
	    encInfo->stego_image_fname = argv[4];
	}
	else
	{
	    printf("Error: Please pass the correct .bmp file\n");
	    return e_failure;
	}
    }
    else
    {
	encInfo->stego_image_fname = "output_stego.bmp";
	printf("INFO: Output file not mentioned.Creating %s as default\n",encInfo->stego_image_fname);
    }

    // No failure, return e_success
    return e_success;
}

// Function to perform encoding
Status do_encoding(EncodeInfo *encInfo)
{
    printf("INFO: Opening required files\n");

    // Open source image, secret file, and stego image files
    if (open_files(encInfo) == e_success)
    {
	printf("INFO: Done\n");
	printf("INFO: ## Encoding procedure started ##\n");

	// Check capacity of stego image to hold secret file
	if (check_capacity(encInfo) == e_success)
	{

	    // Copy BMP header from source to stego image
	    if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
	    {
		printf("INFO: Done\n");

		// Encode magic string signature
		if (encode_magic_string(MAGIC_STRING, encInfo) == e_success)
		{
		    printf("INFO: Done\n");

		    // Encode secret file extension size
		    if (encode_secret_file_extn_size(strlen(encInfo->extn_secret_file), encInfo) == e_success)
		    {
			printf("INFO: Done\n");

			// Encode secret file extension
			if (encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_success)
			{
			    // Encode secret file size
			    if (encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_success)
			    {
				printf("INFO: Done\n");

				// Encode secret file data
				if (encode_secret_file_data(encInfo) == e_success)
				{
				    printf("INFO: Done\n");

				    // Copy remaining image data from source to stego image
				    if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
				    {
					printf("INFO: Done\n");
					return e_success;
				    }
				    else
				    {
					printf("Error\n");
					return e_failure;
				    }
				}
				else
				{
				    printf("Error\n");
				    return e_failure;
				}
			    }
			    else
			    {
				printf("Error\n");
				return e_failure;
			    }
			}
			else
			{
			    printf("Error\n");
			    return e_failure;
			}
		    }
		    else
		    {
			printf("Error\n");
			return e_failure;
		    }
		}
		else
		{
		    printf("Error: While copying Magic string\n");
		    return e_failure;
		}
	    }
	    else
	    {
		printf("Not copied successfully\n");
		return e_failure;
	    }
	}
	else
	{
	    printf("Error: Secret file size is greater than .bmp file capacity\n");
	    return e_failure;
	}
    }
    else
    {
	printf("File not opened\n");
	return e_failure;
    }
}

// Function to open source image, secret file, and stego image files
Status open_files(EncodeInfo *encInfo)
{
    // Source Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Error handling
    if (encInfo->fptr_src_image == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);
	return e_failure;
    }
    printf("INFO: Opened %s\n", encInfo->src_image_fname);

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Error handling
    if (encInfo->fptr_secret == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);
	return e_failure;
    }
    printf("INFO: Opened %s\n", encInfo->secret_fname);

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);
	return e_failure;
    }
    printf("INFO: Opened %s\n", encInfo->stego_image_fname);

    // No failure, return e_success
    return e_success;
}
// Function to check the capacity of stego image to hold secret file
Status check_capacity(EncodeInfo *encInfo)
{
    // Get the size of the secret file
    printf("INFO: Checking for %s size\n",encInfo->secret_fname);
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);

    // Check if the secret file is not empty
    if (encInfo->size_secret_file != 0)
    {
	printf("Info: Done not Empty\n");
    }
    else
    {
	return e_failure;
	// if secret file is empty then return e_failure
    }

    // Get the capacity of the source image to hold the secret file
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);

    printf("INFO: Checking for %s capacity to handle %s file\n", encInfo->src_image_fname, encInfo->secret_fname);

    // Check if the capacity is sufficient to hold the secret file
    if ((encInfo->image_capacity) > ((encInfo->size_secret_file) + (2 + 4 + 4 + 4)) * 8)
    {
	printf("INFO: Done found ok\n");
    }
    else
    {
	return e_failure;
    }
    // no failure return e_success
    return e_success;
}

// Function to get the size of a BMP image
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(uint), 1, fptr_image);
   // printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(uint), 1, fptr_image);
    //printf("height = %u\n", height);

    // Return image capacity
    return (width * height * 3);
}

// Function to get the size of a file
uint get_file_size(FILE *fptr)
{
    // Seek to the last byte
    fseek(fptr, 0, SEEK_END);
    uint size = ftell(fptr);
    // return size
    return size;
}

// Function to copy the BMP header from source image to destination image
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    printf("INFO: Copying Image Header\n");
    char str[54];
    // Seek to 0th byte of src image
    fseek(fptr_src_image, 0, SEEK_SET);

    // Seek to 0th byte of dest image
    fseek(fptr_dest_image, 0, SEEK_SET);

    // Read first 54 bytes from src_image and store in str
    fread(str, sizeof(char), 54, fptr_src_image);

    // Write the read data to dest_image
    fwrite(str, sizeof(char), 54, fptr_dest_image);

    // No failure return e_success
    return e_success;
}

// Function to encode magic string signature into the stego image
Status encode_magic_string(char *magic_string, EncodeInfo *encInfo)
{
    printf("INFO: Encoding magic string signature\n");
    // Call the function to encode data into image
    encode_data_to_image(magic_string, strlen(magic_string), encInfo->fptr_src_image, encInfo->fptr_stego_image, encInfo);
    return e_success;
}

// Function to encode data into the stego image
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image, EncodeInfo *encInfo)
{
    for (int i = 0; i < size; i++)
    {
	// Read 8 bytes from src_image and store in image_data
	fread(encInfo->image_data, sizeof(char), 8, fptr_src_image);

	// Call the function to encode a byte into the least significant bit of image_data
	encode_byte_to_lsb(data[i], encInfo->image_data);

	// Write the encoded data to stego_image
	fwrite(encInfo->image_data, sizeof(char), 8, fptr_stego_image);
    }
    // No failure return e_success
    return e_success;
}

// Function to encode a byte into the least significant bit of image buffer
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for (int i = 7; i >= 0; i--)
    {
	// Encode the byte into the least significant bit of image_buffer
	image_buffer[i] = ((data & (1 << i)) >> i) | (image_buffer[i] & 0xFE);
    }
    return e_success;
}

// Function to encode the size of the secret file extension into the stego image
Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
    printf("INFO: Encoding %s file extension\n", encInfo->secret_fname);
    char str[32];

    // Read 32 bytes from src_image and store in str
    fread(str, 1, 32, encInfo->fptr_src_image);

    // Encode the size into the least significant bits of str
    encode_size_to_lsb(size, str);

    // Write the encoded data to stego_image
    fwrite(str, 1, 32, encInfo->fptr_stego_image);

    // No failure return e_success
    return e_success;
}

// Function to encode the secret file extension into the stego image
Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo)
{
    // Call the function to encode data into image
    encode_data_to_image(file_extn, strlen(file_extn), encInfo->fptr_src_image, encInfo->fptr_stego_image, encInfo);

    // No failure return e_success
    return e_success;
}

// Function to encode the size of the secret file into the stego image
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    printf("INFO: Encoding %s file size\n", encInfo->secret_fname);
    char str[32];

    // Read 32 bytes from src_image and store in str
    fread(str, 1, 32, encInfo->fptr_src_image);

    // Encode the size into the least significant bits of str
    encode_size_to_lsb(file_size, str);

    // Write the encoded data to stego_image
    fwrite(str, 1, 32, encInfo->fptr_stego_image);

    // No failure return e_success
    return e_success;
}
// Function to encode the data of the secret file into the stego image
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    printf("INFO: Encoding %s file data\n", encInfo->secret_fname);

    // Seek to the beginning of the secret file
    fseek(encInfo->fptr_secret, 0, SEEK_SET);
    char ch;

    // Loop through each byte of the secret file
    for (int i = 0; i < encInfo->size_secret_file; i++)
    {
	// Read 8 bytes from the source image and store in image_data
	fread(encInfo->image_data, 1, 8, encInfo->fptr_src_image);

	// Read one byte from the secret file
	fread(&ch, 1, 1, encInfo->fptr_secret);

	// Call the function to encode the byte into the least significant bits of image_data
	encode_byte_to_lsb(ch, encInfo->image_data);

	// Write the encoded data to the stego_image
	fwrite(encInfo->image_data, 8, 1, encInfo->fptr_stego_image);
    }

    // No failure return e_success
    return e_success;
}

// Function to copy the remaining data from source image to destination image
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    printf("INFO: Copying left over data\n");

    char ch;

    // Copy remaining data byte by byte
    while ((fread(&ch, 1, 1, fptr_src)) > 0)
    {
	fwrite(&ch, 1, 1, fptr_dest);
    }

    // No failure return e_success
    return e_success;
}

// Function to encode the size into the least significant bits of image buffer
Status encode_size_to_lsb(int data, char *image_buffer)
{
    for (int i = 31; i >= 0; i--)
    {
	// Encode the size into the least significant bits of image_buffer
	image_buffer[i] = ((data & (1 << i)) >> i) | (image_buffer[i] & 0xFE);
    }
}


