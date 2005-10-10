// dat2bin.cpp : 定義主控台應用程式的進入點。
//

#include <stdio.h>
#include <string.h>
#include "global.h"
#include "char.h"

int main(int argc, char* argv[])
{
	if( argc < 2 )
		return 1;

	char* prefix = argv[1];
	char filename[ 100 ];
	int i;

#ifndef WIN32
	sprintf( filename, "%s/%s", prefix, CHAR_INDEX_FILE );
#else
	sprintf( filename, "%s\\%s", prefix, CHAR_INDEX_FILE );
#endif
	FILE* fi = fopen( filename, "r" );
	if ( !fi )
		return 1;

	strcat(filename, ".bin");
	FILE* fo = fopen( filename, "wb" );

	if( fo )
	{
		for ( i = 0; i <= PHONE_NUM; i++ )
		{
			uint16 arrPhone;
			int begin;
			fscanf( fi, "%hu %d", &arrPhone, &begin );
			fwrite( &arrPhone, sizeof(uint16), 1, fo );
			fwrite( &begin, sizeof(int), 1, fo );
		}
		fclose( fo );
	}
	fclose( fi );

#ifndef WIN32
	sprintf( filename, "%s/%s", prefix, PHONE_TREE_FILE );
#else
	sprintf( filename, "%s\\%s", prefix, PHONE_TREE_FILE );
#endif
	fi = fopen( filename, "r" );
	if ( !fi )
		return 1;
	strcat(filename, ".bin");
	fo = fopen( filename, "wb" );

	if( fo )
	{
		TreeType tree = {0};
		for ( i = 0; i < TREE_SIZE; i++ ) {
			if ( fscanf( fi, "%hu%d%d%d",
				&tree.phone_id,
				&tree.phrase_id,
				&tree.child_begin,
				&tree.child_end ) != 4 )
				break;
			fwrite( &tree, sizeof(TreeType), 1, fo );
		}
		fclose( fo );
	}
	fclose( fi );


#ifndef WIN32
	sprintf( filename, "%s/%s", prefix, PH_INDEX_FILE );
#else
	sprintf( filename, "%s\\%s", prefix, PH_INDEX_FILE );
#endif
	fi = fopen( filename, "r" );
	if( !fi )
		return 1;
	strcat( filename, ".bin" );
	fo = fopen( filename, "wb" );
	if( fo )
	{
		int i = 0;
		int begin;
		while ( !feof( fi ) )
		{
			fscanf( fi, "%d", &begin );
			fwrite( &begin, sizeof(int), 1, fo );
		}
		fclose(fo);
	}
	fclose( fi );

	return 0;
}

