#include <stdio.h>
#include<string.h>
#include"decode.h"
#include "encode.h"
#include "types.h"
int main(int argc, char *argv[])
{
    // Check the type of operation (encode/decode)
    int operation = check_operation_type(argv);

    // Encode operation
    if (operation == e_encode)
    {
        // Check if the correct number of command-line arguments are provided for encoding
        if ((argc >= 4 && argc <= 5) || (argc >= 5 && argc < 6))
        {
            printf("-------------> You selected Encoding <----------------\n");
            EncodeInfo encInfo;

            // Validate and read command-line arguments for encoding
            if (read_and_validate_encode_args(argv, &encInfo) == e_success)
            {
                // Perform encoding
                if (do_encoding(&encInfo) == e_success)
                {
                    printf("## Encoding done successfully ##\n");
                }
                else
                {
                    printf("Error: Encoding fails\n");
                }
            }
            else
            {
                printf("Error: Please pass correct command-line arguments for encoding\n");
            }
        }
        else
        {
            printf("Please pass 4 or 5 command-line arguments for encoding\n");
        }
    }
    // Decode operation
    else if (operation == e_decode)
    {
        // Check if the correct number of command-line arguments are provided for decoding
        if ((argc >= 3 && argc <= 4) || (argc >= 4 && argc < 5))
        {
            printf("-------------> You selected Decoding <----------------\n");
            DecodeInfo decInfo;

            // Validate and read command-line arguments for decoding
            if (read_and_validate_decode_args(argv, &decInfo) == e_success)
            {
                printf("INFO: Done\n");
                // Perform decoding
                if (do_decoding(&decInfo) == e_success)
                {
                    printf("## Decoding done successfully ##\n");
                }
                else
                {
                    printf("Decoding not done successfully\n");
                }
            }
            else
            {
                printf("Error: Please pass correct command-line arguments for decoding\n");
            }
        }
        else
        {
            printf("Please pass 3 or 4 command-line arguments for decoding\n");
        }
    }
    else
    {
        printf("Error: Please pass the argument like -d or -e\n");
    }

    return 0;
}


// Function to check the type of operation based on command-line arguments
OperationType check_operation_type(char *argv[])
{
    if(!strcmp(argv[1],"-e"))
    {
	return e_encode;
    }

    else if(!strcmp(argv[1],"-d"))
    {
	return e_decode;
    }

    else
    {
	return e_unsupported;
    }


}


