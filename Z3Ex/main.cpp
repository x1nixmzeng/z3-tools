/*
	RealSpace3 Z-Extractor
	WRS
	
	v0.05
		Hardcoded key to reduce number of patches
			-> lpatcher is no longer the best distribution method
			-> key_data only changes last 20 bytes
	v0.04
		source cleanup
		github release
	v0.03
		Added PMANG RZ OBT
		Now able to extract uncompressed files
		Now able to extract comtype 1 files
		The xor function has now been written up
		Fixed the output directory structure
			ie	'testai.xml' (from 'Data/AIDefinition.mrf')
			is now dumped as
				'Data/AIDefinition/testai.xml'
		Fixed the prepName length bug (missed 9 characters from end)
	v0.02
		Fixed dir creation
		Moved files into datadump/ folder
	v0.01
	
	I began wanting to call functions w QuickBMS but that failed
	Instead I decided to hack around the executable in a similar manner to
	how I coded the MEF dumper (where the DLL was added as an import)
*/

#include <windows.h>
#include <stdio.h>
#include "filecmds.h"

#define compiled_ver		"v0.05"
//#define dump_filelist

//#define RAIDERZ_2012_PWE
//#define RAIDERZ_20120213
//#define GUNZ2_20120524
#define RAIDERZ_20120704
//#define RAIDERZ_r34991
//#define RAIDERZ_r35358

// v0.05
const DWORD KEY_LENGTH = 0xCA;

#ifdef RAIDERZ_20120213

	// RaiderZ PMANG (r32096, 13 Feb 2012)
	#define compiled_for	"RaiderZ PMANG"

	#define addrGetValues	0x00674160
	#define addrPartialDec	0x00689790
	
	#define addrXorData		0x00683890
	#define addrUncompress	0x00681E00

#elif defined RAIDERZ_2012_PWE

	// RaiderZ PWE Alpha (r??, 14 Mar 2012)
	
	// On 20 Mar 2012 patch r32732 was applied to the alpha test
	// http://raiderz.perfectworld.com/news/?p=493111
	
	#define compiled_for	"RaiderZ PWE Alpha"

	#define addrGetValues	0x00675EA0
	#define addrPartialDec	0x0068B350
	
	#define addrXorData		0x00685520
	#define addrUncompress	0x00683A40

	unsigned char KEY_DATA[ KEY_LENGTH ] = {
		0x30, 0x81, 0xC7, 0x02, 0x01, 0x00, 0x30, 0x81,
		0xA4, 0x06, 0x07, 0x2A, 0x86, 0x48, 0xCE, 0x3D,
		0x02, 0x01, 0x30, 0x81, 0x98, 0x02, 0x01, 0x01,
		0x30, 0x20, 0x06, 0x07, 0x2A, 0x86, 0x48, 0xCE,
		0x3D, 0x01, 0x01, 0x02, 0x15, 0x00, 0xE9, 0x5E,
		0x4A, 0x5F, 0x73, 0x70, 0x59, 0xDC, 0x60, 0xDF,
		0xC7, 0xAD, 0x95, 0xB3, 0xD8, 0x13, 0x95, 0x15,
		0x62, 0x0F, 0x30, 0x2C, 0x04, 0x14, 0x34, 0x0E,
		0x7B, 0xE2, 0xA2, 0x80, 0xEB, 0x74, 0xE2, 0xBE,
		0x61, 0xBA, 0xDA, 0x74, 0x5D, 0x97, 0xE8, 0xF7,
		0xC3, 0x00, 0x04, 0x14, 0x1E, 0x58, 0x9A, 0x85,
		0x95, 0x42, 0x34, 0x12, 0x13, 0x4F, 0xAA, 0x2D,
		0xBD, 0xEC, 0x95, 0xC8, 0xD8, 0x67, 0x5E, 0x58,
		0x04, 0x29, 0x04, 0xBE, 0xD5, 0xAF, 0x16, 0xEA,
		0x3F, 0x6A, 0x4F, 0x62, 0x93, 0x8C, 0x46, 0x31,
		0xEB, 0x5A, 0xF7, 0xBD, 0xBC, 0xDB, 0xC3, 0x16,
		0x67, 0xCB, 0x47, 0x7A, 0x1A, 0x8E, 0xC3, 0x38,
		0xF9, 0x47, 0x41, 0x66, 0x9C, 0x97, 0x63, 0x16,
		0xDA, 0x63, 0x21, 0x02, 0x15, 0x00, 0xE9, 0x5E,
		0x4A, 0x5F, 0x73, 0x70, 0x59, 0xDC, 0x60, 0xDF,
		0x59, 0x91, 0xD4, 0x50, 0x29, 0x40, 0x9E, 0x60,
		0xFC, 0x09, 0x02, 0x01, 0x01, 0x04, 0x1B, 0x30,
		0x19, 0x02, 0x01, 0x01, 0x04, 0x14, 0x39, 0x79,
		0x0A, 0x0F, 0xE0, 0x7F, 0x5C, 0xD0, 0x67, 0x0A,
		0xEC, 0x57, 0x65, 0x75, 0x6C, 0x5B, 0xEB, 0xC0,
		0x29, 0x8B 
	};

	
#elif defined RAIDERZ_20120704

	// RaiderZ PMANG (r34748, 05 Jul 2012)
	
	// At this point, the client is packed with ASProtect
	
	#define compiled_for	"RaiderZ PMANG OBT"
	
	// values are 00AB7B28 and 000000CA
	#define addrGetValues	0x006B3970
	#define addrPartialDec	0x006C90D0
	
	//#define addrXorData		0x00
	#define addrUncompress	0x006C1600
	
	unsigned char KEY_DATA[ KEY_LENGTH ] = {
		0x30, 0x81, 0xC7, 0x02, 0x01, 0x00, 0x30, 0x81,
		0xA4, 0x06, 0x07, 0x2A, 0x86, 0x48, 0xCE, 0x3D,
		0x02, 0x01, 0x30, 0x81, 0x98, 0x02, 0x01, 0x01,
		0x30, 0x20, 0x06, 0x07, 0x2A, 0x86, 0x48, 0xCE,
		0x3D, 0x01, 0x01, 0x02, 0x15, 0x00, 0xE9, 0x5E,
		0x4A, 0x5F, 0x73, 0x70, 0x59, 0xDC, 0x60, 0xDF,
		0xC7, 0xAD, 0x95, 0xB3, 0xD8, 0x13, 0x95, 0x15,
		0x62, 0x0F, 0x30, 0x2C, 0x04, 0x14, 0x34, 0x0E,
		0x7B, 0xE2, 0xA2, 0x80, 0xEB, 0x74, 0xE2, 0xBE,
		0x61, 0xBA, 0xDA, 0x74, 0x5D, 0x97, 0xE8, 0xF7,
		0xC3, 0x00, 0x04, 0x14, 0x1E, 0x58, 0x9A, 0x85,
		0x95, 0x42, 0x34, 0x12, 0x13, 0x4F, 0xAA, 0x2D,
		0xBD, 0xEC, 0x95, 0xC8, 0xD8, 0x67, 0x5E, 0x58,
		0x04, 0x29, 0x04, 0xBE, 0xD5, 0xAF, 0x16, 0xEA,
		0x3F, 0x6A, 0x4F, 0x62, 0x93, 0x8C, 0x46, 0x31,
		0xEB, 0x5A, 0xF7, 0xBD, 0xBC, 0xDB, 0xC3, 0x16,
		0x67, 0xCB, 0x47, 0x7A, 0x1A, 0x8E, 0xC3, 0x38,
		0xF9, 0x47, 0x41, 0x66, 0x9C, 0x97, 0x63, 0x16,
		0xDA, 0x63, 0x21, 0x02, 0x15, 0x00, 0xE9, 0x5E,
		0x4A, 0x5F, 0x73, 0x70, 0x59, 0xDC, 0x60, 0xDF,
		0x59, 0x91, 0xD4, 0x50, 0x29, 0x40, 0x9E, 0x60,
		0xFC, 0x09, 0x02, 0x01, 0x01, 0x04, 0x1B, 0x30,
		0x19, 0x02, 0x01, 0x01, 0x04, 0x14, 0xC0, 0x07,
		0x74, 0x25, 0x13, 0xAA, 0x00, 0x07, 0x2A, 0xDC,
		0x22, 0x1C, 0xF9, 0xFB, 0x25, 0x8F, 0x46, 0xB0,
		0xA7, 0x96
	};
	
	
#elif defined RAIDERZ_r34991

	// RaiderZ EU closed weekend (r34991, 04 Aug 2012)
	
	// Client is not packed at all
	
	#define compiled_for	"RaiderZ EU CBT (weekend)"
	
	#define addrGetValues	0x006B54F0
	#define addrPartialDec	0x006CACE0
	
	#define addrUncompress	0x006C3220
	
#elif defined RAIDERZ_r35358

	// RaiderZ PWE (r35358, CBT w patch1_4.mpf applied)
	// Protected with ASProtect again
	
	// NOTE: The unpatched client (r35284) uses the same key
	// 
	
	
	#define compiled_for	"RaiderZ r35358"
	
	#define addrGetValues	0x006B62D0
	#define addrPartialDec	0x006CBAA0
	
	#define addrUncompress	0x006C4180

	// v0.05
	unsigned char KEY_DATA[ KEY_LENGTH ] = {
		0x30, 0x81, 0xC7, 0x02, 0x01, 0x00, 0x30, 0x81,
		0xA4, 0x06, 0x07, 0x2A, 0x86, 0x48, 0xCE, 0x3D,
		0x02, 0x01, 0x30, 0x81, 0x98, 0x02, 0x01, 0x01,
		0x30, 0x20, 0x06, 0x07, 0x2A, 0x86, 0x48, 0xCE,
		0x3D, 0x01, 0x01, 0x02, 0x15, 0x00, 0xE9, 0x5E,
		0x4A, 0x5F, 0x73, 0x70, 0x59, 0xDC, 0x60, 0xDF,
		0xC7, 0xAD, 0x95, 0xB3, 0xD8, 0x13, 0x95, 0x15,
		0x62, 0x0F, 0x30, 0x2C, 0x04, 0x14, 0x34, 0x0E,
		0x7B, 0xE2, 0xA2, 0x80, 0xEB, 0x74, 0xE2, 0xBE,
		0x61, 0xBA, 0xDA, 0x74, 0x5D, 0x97, 0xE8, 0xF7,
		0xC3, 0x00, 0x04, 0x14, 0x1E, 0x58, 0x9A, 0x85,
		0x95, 0x42, 0x34, 0x12, 0x13, 0x4F, 0xAA, 0x2D,
		0xBD, 0xEC, 0x95, 0xC8, 0xD8, 0x67, 0x5E, 0x58,
		0x04, 0x29, 0x04, 0xBE, 0xD5, 0xAF, 0x16, 0xEA,
		0x3F, 0x6A, 0x4F, 0x62, 0x93, 0x8C, 0x46, 0x31,
		0xEB, 0x5A, 0xF7, 0xBD, 0xBC, 0xDB, 0xC3, 0x16,
		0x67, 0xCB, 0x47, 0x7A, 0x1A, 0x8E, 0xC3, 0x38,
		0xF9, 0x47, 0x41, 0x66, 0x9C, 0x97, 0x63, 0x16,
		0xDA, 0x63, 0x21, 0x02, 0x15, 0x00, 0xE9, 0x5E,
		0x4A, 0x5F, 0x73, 0x70, 0x59, 0xDC, 0x60, 0xDF,
		0x59, 0x91, 0xD4, 0x50, 0x29, 0x40, 0x9E, 0x60,
		0xFC, 0x09, 0x02, 0x01, 0x01, 0x04, 0x1B, 0x30,
		0x19, 0x02, 0x01, 0x01, 0x04, 0x14, 0x39, 0x79,
		0x0A, 0x0F, 0xE0, 0x7F, 0x5C, 0xD0, 0x67, 0x0A,
		0xEC, 0x57, 0x65, 0x75, 0x6C, 0x5B, 0xEB, 0xC0,
		0x29, 0x8B 
	};
	
#elif defined GUNZ2_20120524

	// GunZ 2 (r13226, 24 May 2012)
	#define compiled_for	"GunZ2 VIP"
	
	#define addrGetValues	0x0049A290
	#define addrPartialDec	0x0054C8C0
	
	#define addrXorData		0x00546420
	#define addrUncompress	0x00544EF0
	
	// v0.05
	unsigned char KEY_DATA[ KEY_LENGTH ] = {
		0x30, 0x81, 0xC7, 0x02, 0x01, 0x00, 0x30, 0x81,
		0xA4, 0x06, 0x07, 0x2A, 0x86, 0x48, 0xCE, 0x3D,
		0x02, 0x01, 0x30, 0x81, 0x98, 0x02, 0x01, 0x01,
		0x30, 0x20, 0x06, 0x07, 0x2A, 0x86, 0x48, 0xCE,
		0x3D, 0x01, 0x01, 0x02, 0x15, 0x00, 0xE9, 0x5E,
		0x4A, 0x5F, 0x73, 0x70, 0x59, 0xDC, 0x60, 0xDF,
		0xC7, 0xAD, 0x95, 0xB3, 0xD8, 0x13, 0x95, 0x15,
		0x62, 0x0F, 0x30, 0x2C, 0x04, 0x14, 0x34, 0x0E,
		0x7B, 0xE2, 0xA2, 0x80, 0xEB, 0x74, 0xE2, 0xBE,
		0x61, 0xBA, 0xDA, 0x74, 0x5D, 0x97, 0xE8, 0xF7,
		0xC3, 0x00, 0x04, 0x14, 0x1E, 0x58, 0x9A, 0x85,
		0x95, 0x42, 0x34, 0x12, 0x13, 0x4F, 0xAA, 0x2D,
		0xBD, 0xEC, 0x95, 0xC8, 0xD8, 0x67, 0x5E, 0x58,
		0x04, 0x29, 0x04, 0xBE, 0xD5, 0xAF, 0x16, 0xEA,
		0x3F, 0x6A, 0x4F, 0x62, 0x93, 0x8C, 0x46, 0x31,
		0xEB, 0x5A, 0xF7, 0xBD, 0xBC, 0xDB, 0xC3, 0x16,
		0x67, 0xCB, 0x47, 0x7A, 0x1A, 0x8E, 0xC3, 0x38,
		0xF9, 0x47, 0x41, 0x66, 0x9C, 0x97, 0x63, 0x16,
		0xDA, 0x63, 0x21, 0x02, 0x15, 0x00, 0xE9, 0x5E,
		0x4A, 0x5F, 0x73, 0x70, 0x59, 0xDC, 0x60, 0xDF,
		0x59, 0x91, 0xD4, 0x50, 0x29, 0x40, 0x9E, 0x60,
		0xFC, 0x09, 0x02, 0x01, 0x01, 0x04, 0x1B, 0x30,
		0x19, 0x02, 0x01, 0x01, 0x04, 0x14, 0x3F, 0xF0,
		0x4B, 0xFF, 0x19, 0xBD, 0xEC, 0x93, 0x6D, 0x33,
		0x27, 0xDE, 0x21, 0x55, 0x21, 0xBF, 0x5C, 0x95,
		0x67, 0x0B 
	};


#else

	#error ** You cannot build WRS__Z3Ex without defining some address **
	#error ** This method relies on 4 function addresses from the target **

#endif

#define CLEAR_MEM(m)	{ delete m; m = NULL; }

//
// #'#'#'#'#'#'#'#'#'#'#'#'#'#'#'#'#'#'#'#'#'#'#'#'#'#'#'#'#'#'#'#'#'#'#'#'#'#'
//

#define KEY_POINTER		(DWORD)(&KEY_DATA)

//
// Returns a key of some sorts
//
typedef bool (__cdecl* fdefGetValues)( DWORD *valA, DWORD *valB );	// 2 args
fdefGetValues GetValues = (fdefGetValues)addrGetValues;

//
// #
//
typedef DWORD (__cdecl* fdefPartialDec)( DWORD key1, DWORD key2, char *srcBuffer, DWORD srcSize, char *dstBuffer, DWORD *dstSize );	// 6 args
fdefPartialDec PartialDec = (fdefPartialDec)addrPartialDec;

//
// XOR function
//
// xor's the data in 32-bit chunks with the key from fileindex record
//
// NOTE: quite a lot of xml data is legiable at this early stage
//
//typedef DWORD (__cdecl* fdefXorData)( char* srcBuffer, DWORD srcSize, DWORD *srcXorKey ); // 3 args
//fdefXorData XorData = (fdefXorData)addrXorData;

//
// Decompression function
//
typedef DWORD (__cdecl* fdefUncompress)( char *srcBuffer, DWORD srcSize, char *outBuffer, DWORD *outSize );  // 4 args
fdefUncompress Uncompress = (fdefUncompress)addrUncompress;

//
//	fileindex.msf structure
//	20-bytes
//	
//	NOTE: FILEINDEX_ENTRY records follows a 1-byte compression flag
//
//		3 known types (RaiderZ only uses type 0)
//		
//		0	Compressed		Data should be fed through XorData() before decompressing
//		1	Compressed++	Data needs unscrambling, which converts it to type 0
//		2	Uncompressed	(GUNZ2 FSB) Raw data - BUT the values have been scrambled (TODO)
//
struct FILEINDEX_ENTRY
{
	DWORD size;		// Uncompressed filesize
	DWORD offset;	// MRF file position
	DWORD zsize;	// Size of compressed data
	DWORD xorkey;	// Used in certain compressed file (when flag == 1)
	short lenMRFN;	// MRF container name length
	short lenName;	// Filename length
};

//
// #'#'#'#'#'#'#'#'#'#'#'#'#'#'#'#'#'#'#'#'#'#'#'#'#'#'#'#'#'#'#'#'#'#'#'#'#'#'
//

void prepName( char *str, DWORD len )
{
	char *pstr;
	DWORD i;
	
	pstr = str;
	i = 0;
	while( i < len )
	{
		if( *str == '/' )
		{
			*str = 0;
			fcCreateDir( pstr );
			*str = '/';
		}
		else
		if(
			!(
				( *str >= 'a' && *str <= 'z' ) ||
				( *str >= 'A' && *str <= 'Z' ) ||
				( *str >= '0' && *str <= '9' ) ||
				( *str == '.' ) ||
				( *str == '-' ) ||
				( *str == 0 )
			)
		)
			*str = '_';
		
		++str;
		++i;
	}
}


// v0.03
char *rs3Name( const FILEINDEX_ENTRY &fileInfo, char *nameBuffer )
{
	char *str;
	const char prefix[] = "datadump/";
	const int len = 9 + fileInfo.lenMRFN - 4 + 1 + fileInfo.lenName;
	
	str = fcAllocate( len +1 );
	
	if( !str ) return NULL;
	
	// base folder
	memcpy( str, prefix, 9 );
	str += 9;
	
	// container folder (data/ ..)
	memcpy(
		str,
		nameBuffer,
		fileInfo.lenMRFN -4 );
	
	str += fileInfo.lenMRFN -4;

	// container seperator
	*str = '/';
	++str;
	
	// filename
	memcpy(
		str,
		nameBuffer+fileInfo.lenMRFN,
		fileInfo.lenName );
	
	str += fileInfo.lenName;
	
	*str = 0;
	str -= len;
	
	prepName( str, len );
	
	return str;
}

void rs3Unscramble( char *srcBuffer, DWORD srcSize, DWORD xorkey )
{
	// Unscramble the 32-bit blocks

	DWORD dBlocks = srcSize >> 2;
	while( dBlocks )
	{
		DWORD tmp;
		tmp = *(DWORD *)srcBuffer; // read 32-bit value
		
		xorkey += tmp;
		tmp ^= xorkey;
		
		*(DWORD *)srcBuffer = tmp; // write 32-bit value
		srcBuffer += 4;
		--dBlocks;
	}
	
	// Unscramble the remaining data (1-3 bytes)

	BYTE bAlign = (BYTE)(srcSize & 3);
	if( bAlign )
	{
		DWORD lastBlock;
		int i;

		lastBlock = 0;
		i = 0;

		// read remaining 8/16/24-bit value
		while( i < bAlign )
		{
			DWORD tmp;
			tmp = *(BYTE *)(srcBuffer + i);
			tmp <<= i << 3;
			lastBlock |= tmp;

			++i;
		}

		xorkey += lastBlock;
		lastBlock ^= xorkey;
		
		// write remaining 8/16/24-bit value
		i = 0;
		while( i < bAlign )
		{
			*(BYTE *)(srcBuffer + i) = (BYTE)lastBlock;
			lastBlock >>= 8;

			++i;
		}
	}
}

void rs3FileExtractor( const FILEINDEX_ENTRY &fileInfo, char *nameBuffer, char fileType )
{
	char misc[0x800];
	HANDLE findex;
	char *buf1;
	char *decFile;
	DWORD zsize2;
	
	// OPEN CONTAINER
	{
		memcpy( misc, nameBuffer, fileInfo.lenMRFN );
		misc[fileInfo.lenMRFN] = 0;
		
		findex = fcOpenRead( misc );
		
		misc[0]=0;
	}
	
	buf1 = fcAllocate( fileInfo.zsize );
	if( buf1 == NULL ) return;
	
	// seek to
	fcSeek( findex, fileInfo.offset );
	
	// read file
	if( fcReadFile( findex, buf1, fileInfo.zsize ) == false )
	{
		fcCloseFile( findex );
		CLEAR_MEM( buf1 );
		return; // bad
	}
	
	// free file handle
	fcCloseFile( findex );
	
	zsize2 = fileInfo.zsize;

	HANDLE hTmp;
	char *fn;
	
	fn = rs3Name( fileInfo, nameBuffer );
	hTmp = fcOpenWrite( fn, true );
	
	if( hTmp == INVALID_HANDLE_VALUE )
	{
		OutputDebugString("WRS__Z3Ex failed to save file");
		CLEAR_MEM( fn );
	}
	else
	{
		misc[0]=0;
		
		if( fileType == 0x00 )
		{
			// compressed 
			
			decFile = fcAllocate( fileInfo.size );
			
			if( !( decFile == NULL ) )
			{
				// v0.03 inline xor func
				rs3Unscramble( buf1, zsize2, fileInfo.xorkey );
				Uncompress( buf1, fileInfo.size, decFile, &zsize2 );
				
				CLEAR_MEM( buf1 );

				fcWriteFile( hTmp, decFile, fileInfo.size );
				
				CLEAR_MEM( decFile );
			
				sprintf(misc, "%s - %i bytes\0", fn, fileInfo.size);
				OutputDebugString( misc );
			}
			else
			{
				CLEAR_MEM( buf1 );
				// write error message
				
				OutputDebugString( "WRS__Z3Ex failed to dump file" );
			}
		}
		else
		if( fileType == 0x1 )
		{
			// compressed, with rsa layer (same as fileindex)
			
			//DWORD key1, key2;
			char *buf2;
			
			buf2 = fcAllocate( fileInfo.zsize );
			
			// todo: check allocation
			
			//GetValues( &key1, &key2 );
			PartialDec( KEY_POINTER, KEY_LENGTH, buf1, fileInfo.zsize, buf2, &zsize2 );
			
			CLEAR_MEM( buf1 );
			
			decFile = fcAllocate( fileInfo.size );
			
			// todo: check allocation
			
			rs3Unscramble( buf2, zsize2, fileInfo.xorkey );
			Uncompress( buf2, fileInfo.size, decFile, &zsize2 );
			
			CLEAR_MEM( buf2 );
			
			fcWriteFile( hTmp, decFile, fileInfo.size );
			
			CLEAR_MEM( decFile );
		
			sprintf(misc, "%s - %i bytes\0", fn, fileInfo.size);
			OutputDebugString( misc );
		
		}
		else
		if( fileType == 0x2 )
		{
			// uncompressed
			
			if( fileInfo.size == fileInfo.zsize && fileInfo.xorkey == 0 )
			{
				fcWriteFile( hTmp, buf1, fileInfo.size );
				
				CLEAR_MEM( buf1 );
				
				sprintf(misc, "%s - %i bytes\0", fn, fileInfo.zsize);
				OutputDebugString( misc );
			}
			else
			{
				OutputDebugString("WRS__Z3Ex failed to extract comtype2");
			}
		}
		else
		{
			OutputDebugString( "WRS__Z3Ex cannot handle this comtype" );
		}
	}
	
	CLEAR_MEM( fn );
	fcCloseFile( hTmp );
}

__declspec( dllexport ) void __stdcall WRS__Z3Ex( void )
{
	HANDLE findex;
	DWORD /*key1, key2, */fsize, usize;
	char *buf1, *buf2;
	const DWORD maxEx = 50000; // 50k
	
	OutputDebugString( "WRS__Z3Ex reading the file index\n" );


	// Read file to memory:	
	findex = fcOpenRead( "fileindex.msf" );
	
	if( findex == INVALID_HANDLE_VALUE )
	{
		OutputDebugString( "WRS__Z3Ex could not open fileindex.msf" );
		return;
	}

	fsize = fcGetSize( findex );
	buf1 = fcAllocate( fsize );
	
	if( buf1 == NULL )
	{
		fcCloseFile( findex );
		return;
	}

	if( fcReadFile( findex, buf1, fsize ) == false )
	{
		fcCloseFile( findex );
		CLEAR_MEM( buf1 );
		return;
	}

	fcCloseFile( findex );
	
	// allocate second buffer
	buf2 = fcAllocate( fsize );
	
	if( buf2 == NULL )
	{
		CLEAR_MEM( buf1 );
		return;
	}
	
	OutputDebugString( "WRS__Z3Ex starting client functions\n" );
	
	
	// Attempt to call client functionsm:
	
	// Get the current values (only need to do this once)
	//GetValues( &key1, &key2 );
	usize = 0;
	
	// this loads rsaenh.dll, lots of crypto debug infoz
	if( PartialDec( KEY_POINTER, KEY_LENGTH, buf1, fsize, buf2, &fsize ) == 0 )
	{
		usize = *(DWORD *) buf2;
		buf2 += sizeof( DWORD );
		fsize -= sizeof( DWORD );
		OutputDebugString( "WRS__Z3Ex got the fileindex size" );
	}
	
	CLEAR_MEM( buf1 );
	
	if( usize != 0 )
	{
		char buf[128];
		char *decFile, *old;
		int max;
		DWORD ret, fcount;
		
		decFile = fcAllocate( usize );
		
		if( !( decFile == NULL ) )
		{
			ret = Uncompress( buf2, fsize, decFile, &usize );
			
			#ifdef dump_filelist
			{
				HANDLE flDump;
				flDump = fcOpenWrite("filelist_dump", false);
				fcWriteFile( flDump, decFile, usize );
				fcCloseFile( flDump );
				
				OutputDebugString( "WRS__Z3Ex dumped the fileindex" );
			}
			#endif

			max = 0;
			old = decFile;
			fcount = 0;
			
			// limit to 1 files
			while( old-decFile < usize && max < maxEx )
			{
				struct FILEINDEX_ENTRY entry;
				char *strPntr;
				char flags;

				flags = *(char *)old;
				old += sizeof( char );
				
				entry = *(struct FILEINDEX_ENTRY *)old;
				old += sizeof( struct FILEINDEX_ENTRY );
				
				strPntr = old;
				
				old += entry.lenMRFN;
				old += entry.lenName;
				
				// debug flags here
				//if( flags == 0x2 )
				{
					rs3FileExtractor( entry, strPntr, flags );
					++max;

				}
				
				++fcount;
			}
			
			OutputDebugString( "WRS__Z3Ex finished parsing fileindex" );
			
			sprintf(buf, "Dumped %i of %i files (limited to %i)\0", max, fcount, maxEx );
			MessageBoxA(0, buf, "WRS__Z3Ex", 0);
			*buf=0;
		}
	}
	
	CLEAR_MEM( buf2 );
}


__declspec( dllexport ) BOOL __stdcall DllMain(HINSTANCE /*hInst*/, DWORD dReason, LPVOID /*lpv*/)
{
	if( dReason == DLL_PROCESS_ATTACH )
		OutputDebugString("WRS__Z3Ex DLL for "compiled_for" "compiled_ver);

	return true;
}
