
// create a file handle
// 
HANDLE fcOpenRead( const char *strFilename )
{
	return(
		CreateFileA(
			strFilename,
			GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL
		)
	);
}

HANDLE fcOpenWrite( const char *strFilename, bool force )
{
	return(
		CreateFileA(
			strFilename,
			GENERIC_WRITE,
			FILE_SHARE_READ,
			NULL,
			force ? CREATE_ALWAYS : CREATE_NEW,
			FILE_ATTRIBUTE_NORMAL,
			NULL
		)
	);
}

// seek to offset
//
void fcSeek( HANDLE fcFile, DWORD offset )
{
	SetFilePointer( fcFile, offset, NULL, FILE_BEGIN );
}

// get file size
//
DWORD fcGetSize( HANDLE fcFile )
{
	return(
		GetFileSize( fcFile, NULL )
	);
}

// close the file handle
//
void fcCloseFile( HANDLE fcFile )
{
	CloseHandle( fcFile );
}

// read to buffer
//
bool fcReadFile( HANDLE fcFile, char *outBuf, DWORD fcSize )
{
	DWORD bRead;
	return(
		ReadFile(
			fcFile,
			outBuf,
			fcSize,
			&bRead,
			NULL
		)
	);
}

bool fcWriteFile( HANDLE fcFile, char *inBuf, DWORD fcSize )
{
	DWORD bWritten;
	
	WriteFile(
		fcFile,
		inBuf,
		fcSize,
		&bWritten,
		NULL
	);
	
	return( bWritten == fcSize );
}

// allocate space
//
char *fcAllocate( DWORD fcSize )
{
	return(
		(char *)malloc( fcSize )
	);
}

// create dir
//
bool fcCreateDir( char *str )
{
	return(
		CreateDirectory(
			str,
			NULL
		)
	);
}