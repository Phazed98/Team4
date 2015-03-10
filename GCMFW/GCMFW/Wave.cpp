#include "Wave.h"
#include <cell/cell_fs.h>
#include <stdlib.h>

FILE* Wave::file = NULL;
char* Wave::buffer = NULL;
long Wave::filebytesize = 0;
short* Wave::wavetop = NULL;
uint32_t Wave::waveByteSize = 0;

//Load in a Wave File, saves data in to the statics above.
int Wave::readWavfile(const char * filename)
{
	printf(" loading %s\n", filename);

	// Look for file
	file = fopen(filename, "rb");
	if (file != NULL)
	{
		// Load file attributes
		fseek(file, 0, SEEK_END);
		filebytesize = ftell(file);
		if (filebytesize == 0){
			printf("seek error.\n");
			return (-1);
		}
		printf("Filebytesize is %ld\n", filebytesize);
		fseek(file, 0, SEEK_SET);

		// Reserve memory
		buffer = (char *)malloc(filebytesize);
		if (buffer == NULL){
			printf("malloc error.\n");
			return (-1);
		}

		// Copy file into memory
		if (fread(buffer, filebytesize, 1, file) != 1){
			printf(" ## ERR fread fp1 \n");
			fclose(file);
			return (-1);
		}
		printf(" %s %ld bytes read \n", filename, filebytesize);

		// Done with the data , close the file.
		fclose(file);

		//Get the Wav attributes of the loaded file
		const char strData[] = {'d', 'a', 't', 'a'};
		int i = seekWavChunk(buffer, filebytesize, (char *)strData);
		if (i > 0){
			wavetop = (short *)(buffer+i+4);
			waveByteSize = getWavChunkSize(buffer, (unsigned int)i);
			printf(" waveByteSize = %d\n", waveByteSize);
		} else {
			return (-1);
		}
	}
	else{
		printf(" couldn’t find %s\n", filename);
	}

	return 0;
}

// Swap Endinan−ess of an uint32 t
uint32_t Wave::rev32(uint32_t in)
{
	unsigned char *s1, *d1;
	uint32_t out = 0;

	d1 = (unsigned char *)&out;
	s1 = (unsigned char *)&in;

	*d1++ = *(s1 + 3);
	*d1++ = *(s1 + 2);
	*d1++ = *(s1 + 1);
	*d1++ = *s1;

	return (out);
}

//returns the Index of the specified target chunk in a Wav file
int Wave::seekWavChunk
	(char *top, unsigned int bytesize, char *target)
{
	int i;
	char chunk[4];
	uint32_t u32;
	unsigned char *up;
	unsigned int index = 0;
	unsigned int skip = 0;
	char *cp;
	const char strRIFF[] = {'R','I','F','F'};
	const char strWAVE[] = {'W','A','V','E'};

	//Check to see if file begins with ’RIFF’
	for (i = 0; i < 4; i++){
		if (top[index++] != strRIFF[i]){
			return (-1);
		}
	}

	//The next 4 bytes contatin the file size
	//Disgard this as we calculate this ourselves
	up = (unsigned char *)&u32;
	for (i = 0; i < 4; i++){
		*up++ = top[index++];
	}

	// The next 4 bytes should be ’WAVE’
	for (i = 0; i < 4; i++){
		if (top[index++] != strWAVE[i]){
			return (-1);
		}
	}

	while (1){
		//Fill up the 4 byte chunk buffer
		for (i = 0; i < 4; i++){
			chunk[i] = top[index++];
		}

		//The 1st 4 bytes of a chunk is its ID, does it match the target?
		for (i = 0; i < 4; i++){
			if(chunk[i] != target[i]){
				break; //didn’t match, break
			}
		}

		if (i == 4){
			return index; // We found our chunk, return the Index
		}

		if (index >= bytesize){
			return (-1); //reached the end of the file, return error
		}

		cp = (char*)&u32;
		for (i = 0; i < 4; i++){
			*cp++ = top[index++];
		}

		if (index >= bytesize){
			return (-1); //reached the end of the file, return error
		}
#if _ENDIAN==BIG_ENDIAN
		u32 = rev32(u32);
#endif
		index += u32;

		if (index >= bytesize){
			return (-1); //reached the end of the file, return error
		}

		skip++; //increment our skip counter
		if (skip > 16){
			break; //We have skipped too many times, return error
		}
		//Loop round again
	}
	return (-1);
}

//Reads the reported chunk size from the specified wav chunk
uint32_t Wave::getWavChunkSize(char *top, unsigned int offset)
{
	uint32_t u32;
	unsigned char *up = (unsigned char *)&u32;
	int i;

	for (i = 0; i < 4; i++){
		*up++ = top[offset++];
	}
#if ENDIAN==BIG_ENDIAN
	u32 = rev32(u32);
#endif
	return u32;
}
