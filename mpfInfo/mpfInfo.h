/*
	Modified after refreshing my C++  ;)
	x1nixmzeng
*/

#include "xbuffer.h"
#include <stdio.h>

enum PATCHTYPE
{
	PATCH_ADD = 1,			// Create a new file
	PATCH_REMOVE,			// Remove an existing file
	PATCH_REPLACE,			// Replace an existing file
	PATCH_MODIFY			// Make various changes to an existing file
};

enum PATCHMODIFYTYPE
{
	PATCHMOD_REMOVE = 1,	// Remove data from file (TODO)
	PATCHMOD_INSERT,		// Insert data to file (TODO)
	PATCHMOD_COPY,			// Copy data from another file (TODO)
	PATCHMOD_4				// ??
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

void printMD5( char *destBuf, char *hash )
{
	destBuf[0] = 0;
	for( int i(0); i<16; ++i, ++hash )
		sprintf(destBuf, "%s%02x", destBuf, (*hash & 0xFF) );
	destBuf[32]=0;
}

bool readPatchAdd( TStream &patch, const PATCHNODE &pnode )
{
	unsigned int verifyLen;
	char MD5[16]; char MD5str[33];
	char *fileName;
	// *********
	unsigned int fileSize;
	// *********

	verifyLen = patch.ReadUInt();
	patch.Read( &MD5, 16 );
	fileName = getFilename( patch );
	fileSize = patch.ReadUInt();

	printMD5( MD5str, MD5 );

	printf( "-- Add file\t%s\n", fileName );
	printf( " MD5\t\t%s\n", MD5str );
	printf( " Size\t\t%u\n\n", fileSize );

	patch.Seek( fileSize, bufo_skip );
	delete fileName;

	return true;
}

bool readPatchRemove( TStream &patch, const PATCHNODE &pnode )
{
	char *fileName;

	fileName = getFilename( patch );

	printf("-- Remove file\t%s\n\n", fileName );

	delete fileName;

	return true;
}

bool readPatchReplace( TStream &patch, const PATCHNODE &pnode )
{
	unsigned int verifyLen;
	char MD5[16]; char MD5str[33];
	char *fileName;
	// *********
	unsigned int oldFileSize;
	unsigned int newFileSize;
	// *********

	verifyLen = patch.ReadUInt();
	patch.Read( &MD5, 16 );
	fileName = getFilename( patch );
	oldFileSize = patch.ReadUInt();
	newFileSize = patch.ReadUInt();

	printMD5( MD5str, MD5 );

	printf( "-- Replace file\t%s\n", fileName );
	printf( " MD5\t\t%s\n", MD5str );
	printf( " Old Size\t%u\n", oldFileSize );
	printf( " Size\t\t%u\n\n", newFileSize );

	patch.Seek( newFileSize, bufo_skip );
	delete fileName;

	return true;
}

bool readPatchModify( TStream &patch, const PATCHNODE &pnode )
{
	unsigned int verifyLen;
	char MD5[16]; char MD5str[33];
	char *fileName;
	// *********
	unsigned int beganPos;
	// *********

	// Get the starting position of the patch node header
	beganPos = ( patch.Position() - sizeof( PATCHNODE ) );

	verifyLen = patch.ReadUInt();
	patch.Read( &MD5, 16 );
	fileName = getFilename( patch );

	printMD5( MD5str, MD5 );

	printf( "-- Modify file\t%s\n", fileName );
	printf( " MD5\t\t%s\n", MD5str );
	delete fileName;

	// Followed by a list of things to patch !

	PATCHNODE mnode;

	printf(" POSITION %u\n", patch.Position() );

	int types[4] = {0};

	while( pnode.size > patch.Position() - beganPos )
	{
		patch.Read( &mnode, sizeof( PATCHNODE ) );

		if( mnode.type >= PATCHMOD_REMOVE && mnode.type <= PATCHMOD_4 )
		{
			if( ( mnode.type == PATCHMOD_REMOVE ) && ( mnode.size == ( sizeof( PATCHNODE ) + 8 ) ) )
			{
				unsigned int rmOffset, rmSize;
				rmOffset	= patch.ReadUInt();
				rmSize		= patch.ReadUInt();
				printf(" > Remove data from %u to %u (%u bytes)\n", rmOffset, ( rmOffset + rmSize ), rmSize );
			}
			else
			if( mnode.type == PATCHMOD_INSERT )
			{
				unsigned int a,b,c;
				a			= patch.ReadUInt(); // Offset ?
				b			= patch.ReadUInt(); // Unpacked size?
				c			= patch.ReadUInt(); // Stored size?
				
				printf(" > Insert data %u %u %u\n", a, b, c ); // placeholder

				// assert( mnode.size == c + 12 + sizeof( PATCHNODE ) ) -- or similar

				patch.Seek( c, bufo_skip );
			}
			else
			if( mnode.type == PATCHMOD_COPY )
			{
				unsigned int a,b,c;
				char *srcName;

				a			= patch.ReadUInt(); // 
				b			= patch.ReadUInt(); // 
				c			= patch.ReadUInt(); // 
				
				srcName		= getFilename( patch );
				
				printf(" > Copy data from %s - %u %u %u\n", srcName, a, b, c ); // placeholder

				delete srcName;
			}
			else
			{
				printf(" > Unhandled type (%02X)\n", mnode.type);
				patch.Seek( mnode.size-sizeof( PATCHNODE ), bufo_skip );
			}

			++types[ mnode.type-1 ];
		}
		else
		{
			printf("Unexpected node %02X\n", mnode.type );
			return false;
		}
	}

	// Not needed (handled above)
	//printf(" TYPE1\t%u\n", types[0]);
	//printf(" TYPE2\t%u\n", types[1]);
	//printf(" TYPE3\t%u\n", types[2]);
	//printf(" TYPE4\t%u\n\n", types[3]);

	return true;
}

void getMpfInfo( TStream &patch )
{
	unsigned int totlen;
	PATCHNODE pnode;

	totlen = patch.ReadUInt(); // unreliable value

	if( !( checkHeader( patch ) ) )
	{
		printf("Unsupported format\n");
		return;
	}

	//patch.Seek( 29, bufo_start );

	// Shorthand function calls:
	typedef bool( *patchFunc )( TStream&, const PATCHNODE & );
	patchFunc patchTypes[4] = 
	{
		readPatchAdd,
		readPatchRemove,
		readPatchReplace,
		readPatchModify
	};
		
	while( patch.Position() < patch.Size() )
	{
		unsigned int curpos = patch.Position();

		patch.Read( &pnode, sizeof( PATCHNODE ) );

		if( pnode.type >= PATCH_ADD && pnode.type <= PATCH_MODIFY )
		{
			// Call relevant function
			if( !( patchTypes[ pnode.type-1 ]( patch, pnode ) ) )
				break;
		}
		else
		{
			printf("WARNING Unknown type %02X (%u)\n", pnode.type, patch.Position());
			break;
		}
	}
}
