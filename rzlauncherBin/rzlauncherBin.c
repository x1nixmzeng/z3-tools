#include <windows.h>
#include <stdio.h>

#include "filecmds.h"

unsigned char launcher_key[16] = {
    0x5E, 0x58, 0x04, 0x29, 0x04, 0xBE, 0x56, 0x1C,
    0xFA, 0xA4, 0xEA, 0x2F, 0x18, 0x86, 0x48, 0xCE
};

void processLauncherBin( char *binData, DWORD binSize )
{
	char *pData;
	unsigned char xorKey, tmp;
	DWORD ecx;
	
	xorKey = binData[0];
	
	// pass 1
	pData = (binData + binSize -1);
	
	while( pData > binData )
	{
		tmp = ( *pData ) ^ xorKey;
		xorKey -= tmp;
		*pData = tmp;
		--pData;
	}
	
	// pass 2
	pData = binData +1;
	
	ecx = 0;
	while( ecx < binSize -1 )
	{
		*pData ^= launcher_key[ ecx & 0xF ];
		++pData;
		++ecx;
	}
}

int main( int argc, char **argv )
{
	printf("RaiderZ Launcher BIN to TXT\n");
	printf("Written by WRS\n\n");
	
	if( !( argc == 2 ) )
	{
		printf("Usage:\n\trzlauncherBin.exe \"RaiderZ Launcher.bin\"\n\n");
		return 0;
	}
	else
	{
		HANDLE fHand;
		char *buf;
		DWORD size;
		
		fHand = fcOpenRead( argv[1] );
		
		if( fHand )
		{
			size = fcGetSize( fHand );
			buf = fcAllocate( size );
			
			if( buf )	fcReadFile( fHand, buf, size );
			
			fcCloseFile( fHand );

			if( buf )	processLauncherBin( buf, size );
			
			fHand = fcOpenWrite( "binout.txt", 1 );
			fcWriteFile( fHand, buf+1, size-1 );
			fcCloseFile( fHand );
			
			free( buf );
			
			printf("Saved binout.txt!\n\n");
		}
	}
	
	return 1;
}
