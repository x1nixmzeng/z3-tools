/*
	z3ResourceExtractor-wip
	x1nixmzeng

	** Still very early reversing work **

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
	unsigned char data : 7;
	bool inSequence : 1;
};

bool decodeSizeData( unsigned char *data, unsigned int &value, unsigned int &length )
{
	codedSizeByte sizeByte;
	unsigned int bitSHL;

	value = 0;	// Reset returned size value
	length = 0;	// Length (in bytes) of size data

	bitSHL = 0;	// 

	do
	{
		// Check shift value (indicates invalid starting position)
		if( bitSHL > 32 ) return false;

		sizeByte = *(codedSizeByte *)data;
		value |= ( static_cast<unsigned int>( sizeByte.data ) << bitSHL );

		bitSHL += 7;	// Bits of data per 'codedSizeByte'
		++data;			// Move along buffer
	}
	while( sizeByte.inSequence );

	length = bitSHL /= 7;

	return true;
}


unsigned int decodeSize( unsigned char *data, unsigned int &count )
{
	unsigned char tmpByte;
	unsigned int sizeValue;

	sizeValue = 0;
	count = 0;

	do
	{
		if( count > 32 )
			return( static_cast<unsigned int>( -1 ) );

		tmpByte = *data;
		sizeValue |= ( ( tmpByte & 0x7F ) << count );

		count += 7;
		++data;
	}
	while( tmpByte >> 7 );

	count /= 7;
	return sizeValue;
}

union controlCmd
{
	unsigned short rawData;

	struct
	{
		unsigned char mask : 5;
		unsigned char cmd  : 3;

		unsigned char len;
	} directData;
};

/*
	
*/
void decodeInstruction( unsigned char *data, unsigned int &count, unsigned char *buff, unsigned int &bufCount )
{
	unsigned char tmpByte;
	unsigned int ecx,edx,edi;

	// NOTE: not checking if there are at least 5 bytes left in the data stream!!

	// Read control byte
	tmpByte = data[0];	// MOV BL,BYTE PTR DS:[EAX]
	count = 1;			// INC EAX
						// MOVZX EDX,BL
						// MOVZX ECX,WORD PTR DS:[EDX*2+<table2>]
	ecx = table2_new[ tmpByte ];
/*
	ecx:0000 0000 0000 0000
		^^^^ ^				number of masking bytes
		      ^^^			related to commands
			      ^^^^ ^^^^	length/size of data
*/
	edx = ecx >> 11;	// SHR EDX,0B
	
	controlCmd test;
	test.rawData = ecx;

	if( test.directData.mask )
	{
		unsigned int ebp;
		ebp = *(unsigned int *)(data+1);

	}


	// added check on mask
//	if( edx > 0 )
	{
		// assert edx < 5 ??

		edi = table1[ edx ];// MOV EDI,DWORD PTR DS:[EDX*4+<table1>]

		unsigned int ebp;

		// Read possible byte stream (4-bytes)
		ebp = *(unsigned int *)(data+count);

		count += edx;		// ADD EAX,EDX
		edi &= ebp;			// AND EDI,EBP

		// todo: finish ebp stuff
	}

	edx = ecx & 0xFF;	// MOVZX EDX,CL

	if( tmpByte & 3 )	// TEST BL,3
	{
		// process command (but no more data)

		// 0x700 is the lower 3 bits of the HIBYTE in the word
		ecx &= 0x700;
		ecx += edi;

		// edx and ecx contain the values here
		
		unsigned int srcOffset, dstLength;

		srcOffset = ecx;
		dstLength = edx;

		// hacky means of avoiding about 5 memcpy clones
		for(unsigned int i = 0; i < dstLength; ++i )
		{
			// copying from existing data
			*(buff+bufCount+i) = *(buff+bufCount-(srcOffset - (i % srcOffset)));
		}

		//memcpy( buff+bufCount, (buff+bufCount-ecx), edx );

		//count += edx;

		printf("| %08X %08X |", edx, ecx);
		bufCount += edx; // test!!
	}
	else
	{
		// write out data (todo: offsetting, etc)

		edx += edi;

		printf(" data %u bytes ", edx );

		// test
		memcpy( buff+bufCount, (data+count), edx );
		bufCount += edx;

		count += edx;
	}

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
			unsigned int lastLength, datalen;

			pdata = (unsigned char *)dBuffer;

			unsigned int a,b;

			if( decodeSizeData( pdata, a, b ) )
			{
				printf( "GOT SIZE: %u (%u bytes)\n", a, b );
			}

			// Read and skip header
			datalen = decodeSize( pdata, lastLength );
			pdata += lastLength;

			unsigned char *buffer = new unsigned char[a];
			unsigned int bufOffset = 0;

			printf("Uncompressed: %u bytes\n", datalen );

			unsigned int i = lastLength;

			// #jit streaming
			fcHandle = filecmds::fcOpenWrite("dump.dat", true);

			while( i < dBufferLen )
			{
				unsigned int oldi = bufOffset;


				printf("Next instruction: %02X - ", *pdata );

				decodeInstruction( pdata, lastLength, buffer, bufOffset );
		
				filecmds::fcWriteFile( fcHandle, (char *)buffer+oldi, bufOffset-oldi );

				pdata += lastLength;
				i += lastLength;
				printf("%u bytes!  (%u/%u)\n", lastLength, i, dBufferLen );
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