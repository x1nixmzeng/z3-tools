#include "xbuffer.h"
#include <stdio.h>

enum PATCHTYPE
{
	PATCH_ADD = 1,	// Create a new file
	PATCH_REMOVE,	// Remove an existing file
	PATCH_MODIFY,	// Change part of an existing file
	PATCH_INSERT	// Make various changes to an existing file
};

enum INSERTTYPE
{
	INSERT_1 = 1,	// 
	INSERT_2,		// 
	INSERT_3		// 
};

#pragma pack(push, 1)

struct PATCHNODE
{
	unsigned int size;
	unsigned char type;
};

#pragma pack(pop)

bool checkHeader( TStream &pfile )
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

char *getFilename( TStream &pfile )
{
	unsigned int nlen;
	char *data;

	nlen = pfile.ReadUInt();
	data = new char[nlen+1];
	pfile.Read( data, nlen );
	data[nlen] = 0;

	return data;
}

void getMpfInfo( TStream &patch )
{
	unsigned int totlen;
	PATCHNODE pnode;
	char MD5[16];

	totlen = patch.ReadUInt(); // unreliable value

	if( !( checkHeader( patch ) ) )
	{
		printf("Unsupported format\n");
		return;
	}

	patch.Seek( 29, bufo_start );
		
	while( patch.Position() < patch.Size() )
	{
		unsigned int curpos = patch.Position();

		patch.Read( &pnode, sizeof( PATCHNODE ) );

		if( pnode.type == PATCH_ADD )
		{
			unsigned int lencheck = patch.ReadUInt();
			patch.Read( &MD5, 16 );

			char *fname;
			fname = getFilename( patch );

			unsigned int flen;
			flen = patch.ReadUInt();

			printf("Action: Add file\t\t%s (%u bytes)\n", fname, flen );
			delete fname;

			patch.Seek( flen, bufo_skip );
		}
		else
		if( pnode.type == PATCH_REMOVE )
		{
			char *fname;
			fname = getFilename( patch );
			printf("Action: Remove the file\t\t%s\n", fname);
			delete fname;
		}
		else
		if( pnode.type == PATCH_MODIFY )
		{
			unsigned int lencheck = patch.ReadUInt();
			patch.Read( &MD5, 16 );

			char *fname;
			fname = getFilename( patch );
			printf("Action: Patch the file\t\t%s\n", fname );
			delete fname;

			unsigned int len;
			patch.Seek( 4, bufo_skip );
			len = patch.ReadUInt();

			patch.Seek( len, bufo_skip );

		}
		else
		if( pnode.type == PATCH_INSERT )
		{
			unsigned int lencheck = patch.ReadUInt();
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
				else
					printf("Unexpected node %02X\n", mnode.type );

				patch.Seek( mnode.size-sizeof( PATCHNODE ), bufo_skip );
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
}
