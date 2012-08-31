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
unsigned int decodeSize( unsigned char *data, unsigned int &count )
{
	unsigned char tmpByte;
	unsigned int sizeValue;

	sizeValue = 0;
	count = 0;

	do
	{
		if( count * 7 > 32 )
			return( static_cast<unsigned int>( -1 ) );

		tmpByte = data[ count ];
		sizeValue |= ( ( tmpByte & 0x7F ) << ( count * 7 ) );
		++count;
	}
	while( tmpByte >> 7 );

	return sizeValue;
}

/*
	
*/
void decodeInstruction( unsigned char *data, unsigned int &count )
{
	unsigned char tmpByte;
	unsigned int ecx,edx,edi;

	// NOTE: not checking if there are at least 5 bytes left in the data stream!!



	// Read control byte
	tmpByte = data[0];	// MOV BL,BYTE PTR DS:[EAX]
	count = 1;			// INC EAX
						// MOVZX EDX,BL
						// MOVZX ECX,WORD PTR DS:[EDX*2+<table2>]
	ecx = *(unsigned short *)( table2 + ( tmpByte * sizeof( short ) ) );
/*
	ecx:0000 0000 0000 0000
		^^^^ ^				number of masking bytes
		      ^^^			related to commands
			      ^^^^ ^^^^	length/size of data
*/
	edx = ecx >> 11;	// SHR EDX,0B
	
	// added check on mask
	if( edx > 0 )
	{
		// assert edx < 5 ??

		edi = table1[ edx ];// MOV EDI,DWORD PTR DS:[EDX*4+<table1>]

		unsigned int ebp;

		// Read possible byte stream (4-bytes)
		ebp = *(unsigned int *)(data+count);

		count += edx;		// ADD EAX,EDX
		ebp &= edi;			// AND EDI,EBP
		
		// todo: finish ebp stuff
	}

	edx = ecx & 0xFF;	// MOVZX EDX,CL

	if( tmpByte & 3 )	// TEST BL,3
	{
		// process command (but no more data)

		// 0x700 is the lower 3 bits of the HIBYTE in the word
		ecx &= 0x700;
		ecx += edi;
	}
	else
	{
		// write out data (todo: offsetting, etc)

		printf(" data %u bytes ", edx );
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

			// Read and skip header
			datalen = decodeSize( pdata, lastLength );
			pdata += lastLength;

			printf("Uncompressed: %u bytes\n", datalen );

			unsigned int i = lastLength;

			while( i < dBufferLen )
			{
				printf("Next instruction: %02X - ", *pdata );
				decodeInstruction( pdata, lastLength );
		
				pdata += lastLength;
				i += lastLength;
				printf("%u bytes!  (%u/%u)\n", lastLength, i, dBufferLen );
			}

			delete dBuffer;
		}

		printf("Done!\n\n");
	}
	else
		printf("Usage: %%s <half_dumped_filelist.msf>\n\n");
	
	return 0;
}