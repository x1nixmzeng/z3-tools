/*
	launcherBinTxt
		Converts the Z3 .bin launcher files to plaintext
	Written by x1nixmzeng
	
	v0.01
*/

#include <windows.h>
#include <stdio.h>

#include "..\filecmds.h"

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
	printf("Z3 launcher BIN to TXT converter\n");
	printf("Written by WRS\n\n");
	
	if( !( argc == 2 ) )
	{
		printf("Usage:\n\tlauncherBinTxt.exe <bin file>\n\n");
		return 0;
	}
	else
	{
		HANDLE fHand;
		char *buf, *pName;
		DWORD size;
		
		fHand = fcOpenRead( argv[1] );
		
		if( fHand )
		{
			size = fcGetSize( fHand );
			buf = fcAllocate( size );
			
			if( buf )	fcReadFile( fHand, buf, size );
			
			fcCloseFile( fHand );

			if( buf )	processLauncherBin( buf, size );
			
			pName = argv[1];
			pName += strlen( argv[1] ) -3;
			*(DWORD *)pName = *(DWORD *)"txt\0";
			
			fHand = fcOpenWrite( argv[1], 1 );
			fcWriteFile( fHand, buf+1, size-1 );
			fcCloseFile( fHand );
			
			free( buf );
			
			printf("Saved %s!\n\n", argv[1]);
		}
	}
	
	return 1;
}
