/*
	launcherBinTxt
		Converts between Z3 .bin launcher files and plaintext
	Written by x1nixmzeng
	
	v0.02
		Repack added
		Now using x1nixmzeng/extractor-base
	v0.01
*/

#include <windows.h>
#include <stdio.h>

#include "..\filecmds.h"

struct buffer
{
	char *data;
	unsigned int length;
};

typedef struct buffer tbuffer;

unsigned char launcher_key[16] = {
    0x5E, 0x58, 0x04, 0x29, 0x04, 0xBE, 0x56, 0x1C,
    0xFA, 0xA4, 0xEA, 0x2F, 0x18, 0x86, 0x48, 0xCE
};

int processLauncherBin( tbuffer *inStream, tbuffer *outStream )
{
	char *pbinData;
	unsigned char xorKey;
	DWORD localOffset;

	if( !( inStream->data ) )
		return 0;

	if( !( outStream->data = fcAllocate( inStream->length -1 ) ) )
		return 0;

	outStream->length = inStream->length -1;

	// Read xor-key value from byte #0
	xorKey = inStream->data[0];
	
	// Data pass 1 ##########################################

	// Go to the last byte in the buffer
	pbinData = ( inStream->data + inStream->length ) -1;
	
	while( pbinData > inStream->data )
	{
		unsigned char tmp;

		tmp = ( *pbinData ) ^ xorKey;
		xorKey -= tmp;
		*pbinData = tmp;
		--pbinData;
	}
	
	// Data pass 2 ##########################################

	// Go to the first the first byte (skips the xor-key)
	pbinData = ( inStream->data ) +1;
	
	localOffset = 0;
	while( localOffset < inStream->length -1 )
	{
		outStream->data[ localOffset ] = pbinData[ localOffset ] ^ launcher_key[ localOffset & 0xF ];
		++localOffset;
	}

	return 1;
}

int processLauncherTxt( tbuffer *inStream, tbuffer *outStream )
{
	char *poutData;
	unsigned char xorKey;
	DWORD localOffset;

	if( !( inStream->data ) )
		return 0;

	if( !( outStream->data = fcAllocate( inStream->length +1 ) ) )
		return 0;

	outStream->length = inStream->length +1;

	// Data pass 1 ##########################################

	poutData = ( outStream->data ) +1;
	localOffset = 0;
	
	while( localOffset < inStream->length )
	{
		poutData[ localOffset ] = inStream->data[ localOffset ] ^ launcher_key[ localOffset & 0xF ];
		++localOffset;
	}

	// Data pass 2 ##########################################

	srand( GetTickCount() );
	xorKey = (unsigned char)(rand() % 255 +1);

	localOffset = 0;

	while( localOffset < inStream->length )
	{
		xorKey += *poutData;
		*poutData ^= xorKey;
		++poutData;
		++localOffset;
	}

	outStream->data[0] = xorKey;

	return 1;
}

typedef int (*processLauncher)( tbuffer *, tbuffer * );

int processFile( HANDLE hand, processLauncher func, char *resfilename  )
{
	tbuffer src, res;

	fcSeek( hand, 0 );
	src.length = fcGetSize( hand );

	if( !( src.data = fcAllocate( src.length ) ) )
	{
		printf("Problems reading file\n");
		return 0;
	}

	if( !( fcReadFile( hand, src.data, src.length ) ) )
	{
		free( src.data );
		printf("Problems reading file\n");
		return 0;
	}

	if( func( &src, &res ) )
	{
		HANDLE handout;
		
		handout = fcOpenWrite( resfilename, 1 );
		fcWriteFile( handout, res.data, res.length );
		
		fcCloseFile( handout );
		
		printf("Saved %s!\n", resfilename);
	}
	else
	{
		printf("Unhandled error with conversion\n");
	}

	free( src.data );
	free( res.data );

	return 1;
}

int main( int argc, char **argv )
{
	processLauncher processLauncherFunc;
	char outname[128];

	printf("Z3 launcher BIN-TXT converter\n");
	printf("Written by WRS\n\n");

	processLauncherFunc = 0;
	outname[0] = 0;
	
	#define usage_switch(argCount,argOption) \
		( ( argc == argCount ) && ( *argv[1] == argOption ) )	
	
	if( usage_switch( 3, 'u' ) )
	{
		printf("Attempting to unpack %s..\n", argv[2] );
		
		sprintf(outname, "%s_unpack.txt", argv[2] );
		processLauncherFunc = &processLauncherBin;
	}
	else
	if( usage_switch( 3, 'r' ) )
	{
		printf("Attempting to repack %s..\n", argv[2] );
		
		sprintf(outname, "%s_repack.bin", argv[2] );
		processLauncherFunc = &processLauncherTxt;
	}
	else
	{
		printf("Usage:\n" \
			"	launcherBinTxt.exe u \"RaiderZ Launcher.bin\"\n" \
			"	launcherBinTxt.exe r \"RaiderZ Launcher.txt\"\n\n");
	}
	
	if( processLauncherFunc )
	{
		HANDLE fhnd;
		
		if( ( fhnd = fcOpenRead( argv[2] ) ) != INVALID_HANDLE_VALUE )
		{
			if( !( processFile( fhnd, processLauncherFunc, outname ) == 1 ) )
				printf("File was not successfully converted.\n");
			
			fcCloseFile( fhnd );
		}
		else
		{
			printf("The file could not be read\n");
		}
	}
	
	return 1;
}
