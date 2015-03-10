#pragma once
#include <stdio.h>
#include <vector>

class Wave
{
public:
	//File on disk
	static FILE* file;

	// File data in memeory
	static char* buffer;

	// reported FileSize from filesystem
	static long filebytesize;

	//Index of the wave data chunk in the file
	static short* wavetop;

	//reported size of the data chunk
	static uint32_t waveByteSize;

	//Load in a Wave File, saves data in to the statics above.
	static int readWavfile(const char * filename);

	// Swap Endinan−ess of an uint32 t
	static uint32_t rev32(uint32_t in);

	// returns the Index of the specified target chunk in a Wav file
	static int seekWavChunk(
		char *top, // The Wav file
		unsigned int bytesize, // the size of the Wav File
		char *target // Which Section of the Wav fiel to load
		);

	//Reads the reported chunk size from the specified wav chunk
	static uint32_t getWavChunkSize(
		char *top, //The Wave File
		unsigned int offset //Index of chunk to get filesize for
		);
};