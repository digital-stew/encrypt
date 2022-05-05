#include <stdio.h>
#include <string.h>
//size of buffer in bytes
int enc_buffer_size = 100000;
//max keysize 1gb
char key[1000000000];

char buffer[100000];
char enc_file_name[500];
int main(int argc, char *argv[])
{

//check for file to encrypt/de
if (argv[2] == NULL){
		printf("no file to encrypt");
		return 1;
	}

// open key file
	FILE *fp;
	fp = fopen(argv[1], "rb");

	if (fp == NULL)
	{
		printf("no key file");
		return 1;
	}
	//get key size	
	fseek(fp, 0, SEEK_END);
	int key_size = ftell(fp);
	
	//test if key is too small
	if (key_size < 3000){
		printf("keyfile too small. >3kb");
		return 1;	
	}
	//test if key too large
	if (key_size > 1000000000){
		printf("key file too large. <1gb");
		return 1;
	}
	
	//read key into buffer "key"
	fseek(fp, 0, SEEK_SET);
	fread(key, key_size, 1, fp);
	fclose(fp);

	
//test if input file has .enc 
	FILE *fp2;
	//put filename into a string
	strcpy(enc_file_name, argv[2] );
	//test if filename has a ".enc" and add or remove as nes
    if (strcasestr(enc_file_name, ".enc" ))
		enc_file_name[strlen(enc_file_name)-4] = '\0';
    else
		strcat(enc_file_name,".enc");

//try to open new file for writing 
	fp2 = fopen(enc_file_name, "wb");
	if (fp2 == NULL)
	{
		printf("cant write new file");
		return 1;
	}
//try to open input file for reading
	fp = fopen(argv[2], "r+b");
	if (fp == NULL)
	{
		printf("no input file");
		return 1;
	}
	//get input file size
	fseek(fp, 0, SEEK_END);
	long int infile_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	long int infile_pos = 0;
	
	int key_pos = 0;
//xor input file with key file byte by byte in {enc_buffer_size} chunks
	while (infile_size > 0){
		if (infile_size > enc_buffer_size){
			fread(buffer, enc_buffer_size, 1, fp);
			//do the enc
			for (int i=0;i<enc_buffer_size;i++){
				buffer[i] = buffer[i] ^ key[key_pos];
				key_pos++;
				if (key_pos > key_size) key_pos = 0;
			}
			fwrite(buffer, enc_buffer_size, 1, fp2);
			infile_size = infile_size - enc_buffer_size;
		}else{
			fread(buffer, infile_size, 1, fp);
			//do the enc but smaller than {enc_buffer_size}
			for (int i=0;i<infile_size;i++){
				buffer[i] = buffer[i] ^ key[key_pos];
				key_pos++;
				if (key_pos > key_size) key_pos = 0;
			}			
			fwrite(buffer, infile_size, 1, fp2);
			infile_size = infile_size - infile_size;			
		}

	}

	fclose(fp);
	fclose(fp2);

	return 0;
}
