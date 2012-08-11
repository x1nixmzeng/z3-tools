/*
	mpfInfo

	Very simple parser of MAIET Patch Files (.MPF)
	- x1nixmzeng
*/

#include <windows.h>
#include <stdio.h>

#include "mbuffer.h" // (this is a C++ TMemoryStream implementation)

enum PATCHTYPE
{
	PATCH_ADDFILE = 1,
	PATCH_REMOVEFILE,
	PATCH_MODIFYFILE,
	PATCH_MODIFYFILE2
};

#pragma pack(push, 1)

struct PATCHNODE
{
	DWORD size;
	unsigned char type;
};

#pragma pack(pop)

bool checkHeader( mbuffer &pfile )
{
	const char MPATCHHEADER[] = "MAIET PATCH FILE v2.1\0\0\0\0";
	char tmp[25];

	if( pfile.Read( &tmp, 25 ) )
	{
		for( int i(0); i < 25; ++i )
			if( tmp[i] != MPATCHHEADER[i] )
				return false;

		return true;
	}

	return false;
}

char *getFilename( mbuffer &pfile )
{
	DWORD nlen;
	char *data;

	nlen = pfile.ReadUInt();
	data = new char[nlen+1];
	pfile.Read( data, nlen );
	data[nlen] = 0;

	return data;
}

int main( int argc, char **argv )
{
	TMemoryStream patch;

	if( patch.LoadFromFile( "patch1_2.mpf" ) )
	{
		DWORD totlen;
		PATCHNODE pnode;
		char MD5[16];

		totlen = patch.ReadUInt();
		checkHeader( patch );

		patch.Seek( 29, mbo_beginning );
		
		while( patch.Position() < patch.Size() )
		{
			DWORD curpos = patch.Position();

			patch.Read( &pnode, sizeof( PATCHNODE ) );

			if( pnode.type == 1 )
			{
				DWORD lencheck = patch.ReadUInt();
				patch.Read( &MD5, 16 );

				char *fname;
				fname = getFilename( patch );

				printf("Action: Add file\t\t%s\n", fname);
				delete fname;

				DWORD len;
				len = patch.ReadUInt();

				patch.Seek( len, mbo_current );
			}
			else
			if( pnode.type == 2 )
			{
				char *fname;
				fname = getFilename( patch );
				printf("Action: Remove the file\t\t%s\n", fname);
				delete fname;
			}
			else
			if( pnode.type == 3 )
			{
				DWORD lencheck = patch.ReadUInt();
				patch.Read( &MD5, 16 );

				char *fname;
				fname = getFilename( patch );
				printf("Action: Patch the file\t\t%s\n", fname );
				delete fname;

				DWORD len;
				patch.Seek( 4, mbo_current );
				len = patch.ReadUInt();

				patch.Seek( len, mbo_current );

			}
			else
			if( pnode.type == 4 )
			{
				DWORD lencheck = patch.ReadUInt();
				patch.Read( &MD5, 16 );

				char *fname;
				fname = getFilename( patch );
				printf("Action: Patch the file\t\t%s\n", fname );
				delete fname;
				

				PATCHNODE mnode; // mod node

				int types[4] = {0};

				while( pnode.size > patch.Position() - curpos )
				{
					patch.Read( &mnode, sizeof( PATCHNODE ) );

					if( mnode.type > 0 && mnode.type < 5 )
						++types[ mnode.type-1 ];

					patch.Seek( mnode.size-sizeof( PATCHNODE ), mbo_current );
				}

				// Guessed types
				printf("Removals: %i\nAdditions: %i\n3 - %i\n4 = %i\n",
					types[0], types[1], types[2], types[3] );

			}
			else
			{
				printf("Read unknown type %02X (%u)\n", pnode.type, patch.Position());
				break;
			}
		}
		
		patch.Clear();
	}

	return 0;
}
