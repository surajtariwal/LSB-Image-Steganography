#include<stdio.h>
#include<string.h>
#include"types.h"
#include"encode.h"

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    //check if second arguement is ending with ".bmp" or not,if it is ending then store in src_image_fname
    if(strstr(argv[2],".bmp")!=NULL)
    {
        printf(".bmp is present\n");
        encInfo -> src_image_fname = argv[2];
    }
    else
    {
        printf(".bmp is not present\n");
        return e_failure;
    }

    //check if third arguement is ending with ".txt" or not, if it is ending then store it in secret_fname
    if(strstr(argv[3],".txt")!=NULL)
    {
        printf(".txt is present\n");
        encInfo -> secret_fname = argv[3];
    }
    else
    {
        printf(".txt is not present\n");
        return e_failure;
    }

    //check if fourth arguement is ending with ".bmp" or not(output file), if it is ending then store it in stego_image_fname 
    if(strstr(argv[4],".bmp")!=NULL)
    {
        printf("stego.bmp is present\n");
        encInfo -> stego_image_fname = argv[4];
    }
    else
    {
        //if it is not ending then store th file name as stego.bmp in stego_image_fname
        encInfo -> stego_image_fname = "stego.bmp";
    }

    if(argv[2] == NULL || argv[3] == NULL || argv[4] == NULL)
    {
        printf("Error : Missing arguements for encoding\n");
        printf("Pass correct arguements\n");
        printf("./a.out -e beautiful.bmp secret.txt --> for encoding\n");
        printf("./a.out -d stego.bmp --> for decoding\n");
    }
    

    return e_success;
}
//If reading and validating the encode arguements is successfull then perform encoding operation 
Status do_encoding(EncodeInfo *encInfo)
{
    if(open_files(encInfo)==e_success)
    {
        printf("All the files are opened successfully\n");
    }
    else
    {
        printf("File are not opened\n");
        return e_failure;
    }

    if(check_capacity(encInfo)==e_success)
    {
        printf("Check capacity is successfull\n");
    }
    else
    {
        printf("Check capacity is un-successfull\n");
        return e_failure;
    }

    if(copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image)==e_success)
    {
        printf("Header copied successfull\n");
    }
    else
    {
        printf("Header copy un-successful\n");
        return e_failure;
    }
    
    if(encode_size_magic_string(encInfo)==e_success)
    {
        printf("Size of magic string is encoded successfully\n");
    }
    else
    {
        printf("Size of magic string is not encoded\n");
        return e_failure;
    }
    
    if(encode_magic_string(encInfo)==e_success)
    {
        printf("Magic string encoded successfully\n");
    }
    else
    {
        printf("Magic string encoding un-successfull\n");
        return e_failure;
    }

    if(encode_secret_file_ext_size(encInfo->extn_secret_file_size,encInfo)==e_success)
    {
        printf("Secret file extension size is encoded successfull\n");
    }
    else
    {
        printf("Secret file extension size is not encoded successfull\n");
        return e_failure;
    }

    if(encode_secret_file_extn(encInfo)==e_success)
    {
        printf("Secret file extension is encoded successffuly\n");
    }
    else
    {
        printf("Secret file extension is not encoded successfully\n");
        return e_failure;
    }

    if(encode_secret_file_size(encInfo->size_secret_file,encInfo)==e_success)
    {
        printf("Secret file size is encoded\n");
    }
    else
    {
        printf("Secret file size is not encoded\n");
        return e_failure;
    }

    if(encode_secret_file_data(encInfo)==e_success)
    {
        printf("Secret file data is encoded\n");
    }
    else
    {
        printf("Secret file data is not encoded\n");
        return e_failure;
    }

    if(copy_remaining_img_data(encInfo)==e_success)
    {
        printf("Remaining image data is copied successfully\n");
    }
    else
    {
        printf("Remaining image data is not copied successfully\n");
        return e_failure;
    }

    if(close_all_files(encInfo)==e_success)
    {
        printf("All file are closed\n");
    }
    else
    {
        printf("Files are not closed\n");
        return e_failure;
    }
    return e_success;
}
Status open_files(EncodeInfo *encInfo)
{
    //check if files are succesfully opened or not.
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname,"r");
    if(encInfo->fptr_src_image==NULL)
    {
        perror("fopen");
        fprintf(stderr,"Source file is not present\n");
        return e_failure;
    }

    encInfo->fptr_secret = fopen(encInfo->secret_fname,"r");
    if(encInfo->fptr_secret==NULL)
    {
        perror("fopen");
        fprintf(stderr,"Secret file is not present\n");
        return e_failure;
    }

    char *dot = strchr(encInfo->secret_fname,'.');
    if(dot != NULL)
    {
        strcpy(encInfo->extn_secret_file, dot);
        encInfo->extn_secret_file_size = strlen(dot);
    }
    else
    {
        printf("No extension found in secret file name\n");
        return e_failure;
    }
    encInfo->fptr_stego_image=fopen(encInfo->stego_image_fname,"w");

    return e_success;
}
Status check_capacity(EncodeInfo *encInfo)
{
    //size of src file
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    //size of secret file
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);

    printf("Enter the magic string : ");
    scanf(" %[^\n]",encInfo->magic);

    if(encInfo->image_capacity > (strlen(encInfo->magic)*8 + 32 + 32 + 32 + encInfo->size_secret_file * 8))
    {
        return e_success;
    }
    else
    {
        printf("Image capacity failed\n");
        return e_failure;
    }
    return e_success;
}
uint get_file_size(FILE *fptr)
{
    long cur_size = ftell(fptr);
    fseek(fptr,0,SEEK_END);
    long size = ftell(fptr);
    fseek(fptr,cur_size,SEEK_SET);
    return (uint)size;
}
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width,height;
    //fptr is pointing to 18th position
    //RGB Data start at the 18th position(bitmap width)
    fseek(fptr_image,18,SEEK_SET);
    //fptr is shifted to 22nd position
    fread(&width,4,1,fptr_image);
    // printf("width = %d\n",width);
    //fptr is shifted to 26th position
    fread(&height,4,1,fptr_image);
    // printf("height = %d\n",height);

    //return height * width * no of pixels(3 bytes for RGB image)
    return (width*height*3);
}
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    //since fptr is pointing to 26 th position so rewind fptr
    rewind(fptr_src_image);
    
    char buffer[54];
    fread(buffer,54,1,fptr_src_image);
    fwrite(buffer,54,1,fptr_dest_image);
    //54 header bytes are copied into buffer and written in stego.bmp image
    return e_success;
}
Status encode_size_magic_string(EncodeInfo *encInfo)
{
    char imagebuffer[32];
    int len = strlen(encInfo->magic);

    fread(imagebuffer,32,1,encInfo->fptr_src_image);
    encode_size_to_lsb(len,imagebuffer);
    fwrite(imagebuffer,32,1,encInfo->fptr_stego_image);

    printf("Size of magic string is encoded successfully is : %d\n",len);

    return e_success;
}
Status encode_magic_string(EncodeInfo *encInfo)
{
    if(encode_data_to_image(encInfo->magic, strlen(encInfo->magic), encInfo)==e_success)
    {
        printf("Magic string is encoded\n");
    }
    else
    {
        printf("Magic string is not encoded\n");
        return e_failure;
    }

    return e_success;
}
Status encode_data_to_image(char *data, int size, EncodeInfo *encInfo)
{
    for(int i=0;i<size;i++)
    {
        fread(encInfo->image_data,8,1,encInfo->fptr_src_image);
        encode_byte_to_lsb(data[i],encInfo->image_data);        
        fwrite(encInfo->image_data,8,1,encInfo->fptr_stego_image);
    }             
    return e_success;                                  
}
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for(int i=0;i<8;i++)
    {
        image_buffer[i] = (image_buffer[i] & 0xFE) | ((data>>i) & 1);
    }
    return e_success;
}
Status encode_secret_file_ext_size(int size, EncodeInfo *encInfo)
{
    char imagebuffer[32];    
    fread(imagebuffer,32,1,encInfo->fptr_src_image);
    encode_size_to_lsb(size,imagebuffer);
    fwrite(imagebuffer,32,1,encInfo->fptr_stego_image);
    return e_success;
}
Status encode_size_to_lsb(long data, char *image_buffer)
{
    for(int i=0;i<32;i++)
    {
        image_buffer[i] = (image_buffer[i] & 0xFE) | ((data >> i) & 1);
    }

    return e_success;
}
Status encode_secret_file_extn(EncodeInfo *encInfo)
{
    for(int i=0; i<encInfo -> extn_secret_file_size ;i++)
    {
        fread(encInfo->image_data,8,1,encInfo->fptr_src_image);
        encode_byte_to_lsb(encInfo -> extn_secret_file[i], encInfo->image_data);        
        fwrite(encInfo->image_data,8,1,encInfo->fptr_stego_image);
    }             
    return e_success; 
}
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char imagebuffer[32];
    
    fread(imagebuffer,32,1,encInfo->fptr_src_image);
    encode_size_to_lsb(file_size,imagebuffer);
    fwrite(imagebuffer,32,1,encInfo->fptr_stego_image);
    
    return e_success;
}
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    rewind(encInfo->fptr_secret);
    fread(encInfo->secret_data,encInfo->size_secret_file,1,encInfo->fptr_secret);
    for(int i=0;i<encInfo->size_secret_file;i++)
    {
        fread(encInfo->image_data,8,1,encInfo->fptr_src_image);
        encode_byte_to_lsb(encInfo->secret_data[i],encInfo->image_data);
        fwrite(encInfo->image_data,8,1,encInfo->fptr_stego_image);
    }
    
    return e_success;
}
Status copy_remaining_img_data(EncodeInfo *encInfo)
{
    int ch;
    while((ch=fgetc(encInfo->fptr_src_image))!=EOF)
    {
        fputc(ch,encInfo->fptr_stego_image); 
    }
    
    return e_success;
}
Status close_all_files(EncodeInfo *encInfo)
{
    fclose(encInfo->fptr_src_image);
    fclose(encInfo->fptr_secret);
    fclose(encInfo->fptr_stego_image);

    return e_success;
}