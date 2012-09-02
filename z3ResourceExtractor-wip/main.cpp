/*
	z3ResourceExtractor-wip
	x1nixmzeng

	Run-length decoding function

	Thanks to skyflox (XeNTaX)
*/

#include "tables.h"

#include <Windows.h>
#include "filecmds.h"

#include <stdio.h>

#define IGNORE_USAGE_OPTION

/*
	Extracts the filesize from header bytes

	Each header byte
		.. has the left-most bit marked as 1
		.. contains 7-bits of data which needs reconstructing

	NOTE: Only a basic shift overflow check
*/

struct codedSizeByte
{
	unsigned char data	: 7;
	bool notLastData	: 1;
};

bool decodeSize( unsigned char *data, unsigned int &value, unsigned int &length )
{
	codedSizeByte sizeByte;
	unsigned int bitSHL;

	value = 0;	// Reset returned size value
	length = 0;	// Length (in bytes) of size data

	bitSHL = 0;	// Reset number of bits to shift left

	do
	{
		// Check shift value (indicates invalid starting position)
		if( bitSHL > 32 ) return false;

		sizeByte = *(codedSizeByte *)data;
		value |= ( static_cast<unsigned int>( sizeByte.data ) << bitSHL );

		bitSHL += 7;	// Bits of data per 'codedSizeByte'
		++data;			// Move along buffer
	}
	while( sizeByte.notLastData );

	// Calculate the actual length of data
	length = bitSHL /= 7;

	return true;
}

bool decodeInstruction( unsigned char *data, unsigned int &count, unsigned char *buff, unsigned int &bufCount )
{
	unsigned char cmdMarker;
	unsigned int instruction, instructionExSize, buf32;

	count = 0;

	// Read the command marker from the buffer
	cmdMarker = *(unsigned char *)(data+count);
	++count;
	
	// Lookup the instruction
	instruction = z3RleInstructions[ cmdMarker ];

	// Length of additional bytes (5-bits of instruction)
	instructionExSize = instruction >> 11;

	if( instructionExSize > 4 )
		return false;

	// Read these additional bytes (only 4 are supported)
	buf32 = 0;
	for( unsigned int i(0 ); i < instructionExSize; ++i, ++count )
	{
		// Read another byte into buf32
		unsigned int tmp = data[ count ];
		tmp <<= ( i * 8 );
		buf32 |= tmp;
	}

	// Check command marker for method
	if( cmdMarker & 3 )
	{
		// Using the destination buffer as the source (copying existing data)

		unsigned int srcOffset, msgLength;

		// Data can be up to 2,047 bytes from current position
		srcOffset = ( instruction & 0x700 ) + buf32;
		msgLength = ( instruction & 0xFF );

		// Copy data from existing buffer
		for(unsigned int i = 0; i < msgLength; ++i )
			*(buff+bufCount+i) = *(buff+bufCount-(srcOffset - (i % srcOffset)));

		bufCount += msgLength;
	}
	else
	{
		// Using the source buffer as the source (inserting new data)

		unsigned int msgLength;

		msgLength = ( instruction & 0xFF ) + buf32;

		// Copy data from buffer
		memcpy( buff+bufCount, data+count, msgLength );
		count += msgLength;
		
		bufCount += msgLength;
	}

	return true;
}

int main( int argc, char** argv )
{
	printf(
		"z3ResourceExtractor-wip\n" \
		"Reversing the decompression method\n\n"
	);
	
	#ifdef IGNORE_USAGE_OPTION
	if( true )
	#else
	if( argc == 2 )
	#endif
	{
		#ifdef IGNORE_USAGE_OPTION
		HANDLE fcHandle = filecmds::fcOpenRead( "fileindex_dec_stage1.dat" );
		#else
		HANDLE fcHandle = filecmds::fcOpenRead( argv[1] );
		#endif

		if( !( fcHandle == INVALID_HANDLE_VALUE ) )
		{
			char *dBuffer;
			unsigned int dBufferLen;

			dBufferLen = filecmds::fcGetSize( fcHandle );

			dBuffer = filecmds::fcAllocate( dBufferLen );
			filecmds::fcReadFile( fcHandle, dBuffer, dBufferLen );
			filecmds::fcCloseFile( fcHandle );


			unsigned char *pdata;

			pdata = (unsigned char *)dBuffer;

			unsigned int a,b;

			if( decodeSize( pdata, a, b ) )
			{
				printf( "GOT SIZE: %u (%u bytes)\n", a, b );
			}

			pdata += b;

			unsigned char *buffer = new unsigned char[a];
			unsigned int bufOffset = 0;

			printf("Uncompressed: %u bytes\n", a );

			unsigned int i = b;

			// #jit streaming
			fcHandle = filecmds::fcOpenWrite("dump.dat", true);

			while( i < dBufferLen )
			{
				unsigned int oldi = bufOffset;


				printf("Next instruction: %02X - ", *pdata );

				decodeInstruction( pdata, b, buffer, bufOffset );
		
				filecmds::fcWriteFile( fcHandle, (char *)buffer+oldi, bufOffset-oldi );

				pdata += b;
				i += b;
				printf("%u bytes!  (%u/%u)\n", b, i, dBufferLen );
			}

			delete dBuffer;

			filecmds::fcCloseFile( fcHandle );


			delete buffer;
		}

		printf("Done!\n\n");
	}
	else
		printf("Usage: %%s <half_dumped_filelist.msf>\n\n");
	
	return 0;
}