#include<stdio.h>
#include<string.h>
#include"types.h"
#include"decode.h"

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    if(strstr(argv[2],".bmp")!=NULL)
    {
        decInfo->stego_image_fname = argv[2];
        printf(".bmp is present for decoding\n");
    }
    else
    {
        printf(".bmp file is not present for decoding\n");
        return e_failure;
    }

    if(argv[3]!=NULL && strlen(argv[3])!=0)
    {
        decInfo->output_fname = argv[3];
    }
    else
    {
        decInfo->output_fname = "output.txt";
    }

    return e_success;
}
Status do_decoding(DecodeInfo *decInfo)
{
    if(open_files_decode(decInfo)==e_success)
    {
        printf("Files opened successfully for decoding\n");
    }
    else
    {
        printf("Files are not opened for decoding\n");
        return e_failure;
    }

    if(decode_magic_string(decInfo)==e_success)
    {
        printf("Magic string decoded successfully\n");
    }
    else
    {
        printf("Magic string is not decoded\n");
        return e_failure;
    }

    if(decode_secret_file_extn_size(decInfo)==e_success)
    {
        printf("Secret file extension size is decoded successfully\n");
    }
    else
    {
        printf("Secret file extension size is not decoded\n");
        return e_failure;
    }

    if(decode_secret_file_extn(decInfo)==e_success)
    {
        printf("Secret file extension is decoded successfully\n");
    }
    else
    {
        printf("Secret file extension is not decoded\n");
        return e_failure;
    }

    if(decode_secret_file_size(decInfo)==e_success)
    {
        printf("Secret file size is decoded successfully\n");
    }
    else
    {
        printf("Secret file size is not decoded\n");
        return e_failure;
    }

    if(decode_secret_file_data(decInfo)==e_success)
    {
        printf("Secret file data is decoded successfully\n");
    }
    else
    {
        printf("Secret file data is not decoded\n");
        return e_failure;
    }

    if(close_all_files_decode(decInfo)==e_success)
    {
        printf("Files are closed successfully\n");
    }
    else
    {
        printf("Files are not closed \n");
        return e_failure;
    }

    return e_success;
}
Status open_files_decode(DecodeInfo *decInfo)
{
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname,"r");
    if(decInfo->fptr_stego_image==NULL)
    {
        perror("fopen");
        fprintf(stderr,"Error stego file not found\n");
        return e_failure;
    }

    decInfo->fptr_output = fopen(decInfo->output_fname,"w");
    if(decInfo->fptr_output == NULL)
    {
        perror("fopen");
        fprintf(stderr,"Error output file cannot be opened\n");
        return e_failure;
    }
    
    fseek(decInfo->fptr_stego_image,54,SEEK_SET);
    return e_success;
}
Status decode_magic_string(DecodeInfo *decInfo)
{
    FILE *fptr=fopen("magic.txt","r");
    if(fptr==NULL)
    {
        perror("fopen");
        fprintf(stderr,"magic.txt file not found\n");
        return e_failure;
    }

    char magic_string_from_user[50];
    fscanf(fptr," %[^\n]",magic_string_from_user);
    fclose(fptr);

    char decode_magic_from_user[50];
    decode_data_from_image(decode_magic_from_user, strlen(magic_string_from_user), decInfo);
    decode_magic_from_user[strlen(magic_string_from_user)] = '\0';

    if(strcmp(magic_string_from_user, decode_magic_from_user)==0)
    {
        printf("Magic string decoded successfully\n");
        printf("Magic string entered by user ' %s' is equal to the string which is decoded '%s'\n",magic_string_from_user, decode_magic_from_user);
    }
    else
    {
        printf("Magic string is not same\n");
        return e_failure;
    }

    return e_success;
}

Status decode_data_from_image(char *data, int size, DecodeInfo *decInfo)
{
    for(int i=0;i<size;i++)
    {
        fread(decInfo->image_data,8,1,decInfo->fptr_stego_image);
        data[i] = decode_byte_from_lsb(decInfo->image_data);
    }

    return e_success;
}

char decode_byte_from_lsb(char *image_buffer)
{
    char ch = 0;
    for(int i=0;i<8;i++)
    {
        ch = ch | ((image_buffer[i] & 1) << i);
    }
    return ch;
}

Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    char imagebuffer[32];
    fread(imagebuffer,32,1,decInfo->fptr_stego_image);
    decInfo->extn_secret_file_size = decode_size_from_lsb(imagebuffer);
    printf("Size of secret file extension is : %d\n",decInfo->extn_secret_file_size);
    return e_success;
}
int decode_size_from_lsb(char *image_buffer)
{
    int size = 0;
    for(int i=0;i<32;i++)
    {
        size = size | ((image_buffer[i] & 1) << i);
    }
    return size;
}
Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    for(int i=0; i<decInfo->extn_secret_file_size ;i++)
    {
        fread(decInfo->image_data,8,1,decInfo->fptr_stego_image);
        decInfo->extn_secret_file_decode[i] = decode_byte_from_lsb(decInfo->image_data);
    }
    decInfo->extn_secret_file_decode[decInfo->extn_secret_file_size] = '\0';
    printf("Secret file extension is : %s\n",decInfo->extn_secret_file_decode);
    return e_success;
}

Status decode_secret_file_size(DecodeInfo *decInfo)
{
    char imagebuffer[32];
    fread(imagebuffer,32,1,decInfo->fptr_stego_image);
    decInfo->size_secret_file_decode = decode_size_from_lsb(imagebuffer);
    printf("Secret file size is : %d\n",decInfo->size_secret_file_decode);

    return e_success;
}

Status decode_secret_file_data(DecodeInfo *decInfo)
{
    for(int i=0;i<decInfo->size_secret_file_decode;i++)
    {
        fread(decInfo->image_data,8,1,decInfo->fptr_stego_image);
        char ch = decode_byte_from_lsb(decInfo->image_data);
        fputc(ch,decInfo->fptr_output);
    }

    return e_success;
}

Status close_all_files_decode(DecodeInfo *decInfo)
{
    fclose(decInfo->fptr_stego_image);
    fclose(decInfo->fptr_output);

    return e_success;
}