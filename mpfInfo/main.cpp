/*
	mpfInfo

	Very simple parser of MAIET Patch Files (.MPF)
	- x1nixmzeng

	August 12th
	> Added a bit more C++ wizardary
	> Few tweaks to compile on linux
*/

#include <stdio.h>

#include "mbuffer.h" // a TMemoryStream implementation
#include "fbuffer.h" // a TFileStream implementation

#include "mpfInfo.h"

unsigned int getSizeOfFile( const char *filename )
{
	unsigned int fsize( 0 );
	TFileStream strm( filename );

	if( !( strm.isOpen() ) )
		return( fsize ); // of 0

	fsize = strm.Size();
	strm.Close();

	return( fsize );
}

int main( int argc, char **argv )
{
	bool valid( false );
	
	if( argc == 2 )
	{
		unsigned int basicCheck = getSizeOfFile( argv[1] );

		if( basicCheck > 0 )
		{
			TStream *patchFile;

			if( basicCheck > ( 1 << 23 ) )
			{
				printf("WARNING: Large file! Reading from TFileStream..\n");
				patchFile = new TFileStream( argv[1] );
				valid = dynamic_cast<TFileStream *>( patchFile )->isOpen();
			}
			else
			{
				patchFile = new TMemoryStream;
				valid = dynamic_cast<TMemoryStream *>( patchFile )->LoadFromFile( argv[1] );
			}

			if( valid )
				getMpfInfo( *patchFile );

			patchFile->Close();
			delete patchFile;
		}
	}
	
	if( !( valid ) )
		printf("Usage:\nmpfInfo <patchfile.mpf>\n\n");

	return 0;
}
