// HashEdit.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string.h>
#include <algorithm>
#include <tchar.h>

#include <assert.h>

#include "hash.h"
extern "C"
{
	#include "chewing-utf8-util.h"
}
#include "chewingserver.h"
#include "chashcontext.h"



#define		chewingServerClassName		_T("ChewingServer")



void CHashContext::release__HASH_ITEM(HASH_ITEM *pItem)
{
    free(pItem->data.phoneSeq);
    free(pItem->data.wordSeq);
    free(pItem);
}

void CHashContext::clear()
{
    std::vector<HASH_ITEM*>::iterator iter;
    
    iter = pool.begin();
    while ( iter!=pool.end() )
    {
        release__HASH_ITEM((HASH_ITEM*) *iter);
        ++iter;
    }
    pool.clear();
}

int CHashContext::load_hash(const char *file, bool doClear)
{
    FILE *fHashFile;
    HASH_ITEM *hitem;
    char strBuf[256];
    int i, lop;
    bool badItem, badStr;

    if ( doClear )
        clear();

    fHashFile = fopen(file, "r");
    if ( fHashFile==NULL )
        return  false;

    if ( fscanf(fHashFile, "%d", &chewing_lifetime) != 1 )
    {
        fclose(fHashFile);
        return  false;
    }

    badStr = false;
    while   ( 1 )
    {
        badItem = false;
        if ( fscanf(fHashFile, "%s", strBuf)!=1 )
            break;

		if ( CHashContext::isChineseString(strBuf)==FALSE )
        {
            badStr = true;
            fseek(fHashFile, FIELD_SIZE-strlen(strBuf)-1, SEEK_CUR);
            continue;
        }

        hitem = (HASH_ITEM*) calloc(1, sizeof(HASH_ITEM));

		int word_len = strlen( strBuf );
		hitem->data.wordSeq = (char*)calloc( word_len + 1, sizeof(char) );
		strcpy( hitem->data.wordSeq, strBuf );

		/* read phoneSeq */
		int len = ueStrLen(hitem->data.wordSeq);
		hitem->data.phoneSeq = (uint16*)calloc( len + 1, sizeof(uint16) );
		for ( i = 0; i < len; i++ )
			if ( fscanf( fHashFile, "%hu", &( hitem->data.phoneSeq[ i ] ) ) != 1 )
			{
                badItem = true;
                goto _handle_load_err;			
			}
		hitem->data.phoneSeq[ len ] = 0;

		/* read userfreq & recentTime */
		if ( fscanf( fHashFile, "%d %d %d %d", 
			&(hitem->data.userfreq), 
			&(hitem->data.recentTime),
			&(hitem->data.maxfreq), 
			&(hitem->data.origfreq) ) != 4 )
		{
            badItem = true;
            goto _handle_load_err;
		}

        pool.push_back(hitem);
        hitem->item_index = pool.size();
    }

_handle_load_err:
    fclose(fHashFile);
    if ( badItem ) {
        release__HASH_ITEM(hitem);
        return  0;
    }
    return  (badStr==false)?1 :-1;
}

static int _PhoneSeqTheSame(const uint16 p1[], const uint16 p2[])
{
	int i;

	for ( i = 0; ( p1[ i ] != 0 && p2[ i ] != 0 ); i++ ) {
		if ( p1[ i ] != p2[ i ] )
			return (int)p1[ i ]-(int)p2[ i ];
	}
	if ( p1[ i ] != p2[ i ] )
        return  (int)p1[ i ]-(int)p2[ i ];
	return 0;
}

static bool _Comp(HASH_ITEM *p1, HASH_ITEM *p2)
{
    int iCmp = strcmp(p1->data.wordSeq, p2->data.wordSeq);
    if ( iCmp!=0 )
    {
        return  (iCmp<0)?true :false;
    }
    return  (_PhoneSeqTheSame(p1->data.phoneSeq, p2->data.phoneSeq)<0)?true :false;
}

void CHashContext::sort_phrase()
{
    //  sort
    std::stable_sort(pool.begin( ), pool.end( ), _Comp);
    //  re-index items
    std::vector<HASH_ITEM*>::iterator iter;
    int id = 0;
    iter = pool.begin();
    while ( iter!=pool.end() )
    {
        ((HASH_ITEM*) *iter)->item_index = id;
        ++iter;
        ++id;
    };
}

bool CHashContext::arrange_phrase()
{
    std::vector<HASH_ITEM*>::iterator iter;
    HASH_ITEM *pItem, *pPivot;
    bool bDup = true;
    
    iter = pool.begin();
    pPivot = NULL;
    while ( iter!=pool.end() )
    {
        pItem = (HASH_ITEM*) *iter;
        if ( pPivot!=NULL )
        {
            if ( (strcmp(pPivot->data.wordSeq, pItem->data.wordSeq)!=0 ||
                 _PhoneSeqTheSame(pPivot->data.phoneSeq, pItem->data.phoneSeq)!=0) )
            { }
            else
            {   /* duplicated item */
                bDup = false;
                release__HASH_ITEM(pItem);
                iter = pool.erase(iter);
                continue;
            }
        }
        pPivot = pItem;
        ++iter;
    }
    return  bDup;
}

//  for debug purpose
void CHashContext::list_phrase()
{
    FILE *foFile;
    std::vector<HASH_ITEM*>::iterator iter;
    HASH_ITEM *pItem;
    uint16 *pui;
    
    foFile = fopen("lista.txt", "w+");
    iter = pool.begin();
    while ( iter!=pool.end() )
    {
        pItem = (HASH_ITEM*) *iter;

        fprintf(foFile, "%s,%d,%d,%d,%d>", 
            pItem->data.wordSeq,
            pItem->data.userfreq, 
            pItem->data.recentTime,
            pItem->data.maxfreq, 
            pItem->data.origfreq);

        pui = pItem->data.phoneSeq;
        while ( *pui!=0 )
        {
            fprintf(foFile, ",%d", *pui);
            ++pui;
        }
        fprintf(foFile, "\n");

        ++iter;
    }
    fclose(foFile);
}


void HashItem2String( char *str, HASH_ITEM *pItem )
{
	int i, len;
	char buf[ FIELD_SIZE ];

	sprintf( str, "%s ", pItem->data.wordSeq );
	len = ueStrLen(pItem->data.wordSeq);
	for ( i = 0; i < len; i++ ) {
		sprintf( buf, "%hu ", pItem->data.phoneSeq[ i ] );
		strcat( str, buf );
	}
	sprintf( 
		buf, "%d %d %d %d",
		pItem->data.userfreq, pItem->data.recentTime, 
		pItem->data.maxfreq, pItem->data.origfreq );
	strcat( str, buf );
}



bool CHashContext::save_hash(const char *destFile)
{
	FILE *outfile;
    std::vector<HASH_ITEM*>::iterator iter;
	char str[FIELD_SIZE+1];

	outfile = fopen(destFile, "w+");

	/* update "lifetime" */
	sprintf(formatstring, "%%-%ds", FIELD_SIZE ); 
	fseek( outfile, 0, SEEK_SET );
	sprintf( str, "%d", chewing_lifetime );
	fprintf( outfile, formatstring, str );

    iter = pool.begin();
    while ( iter!=pool.end() )
    {
    	HashItem2String(str, *iter);
    	fprintf(outfile, formatstring, str);
        ++iter;
    };
	fclose( outfile );
    return  true;
}

HASH_ITEM* CHashContext::get_phrase_by_id(int index)
{
    if ( index>=pool.size() || index<0 )
    {
        return  NULL;
    }

    return  pool[index];
}

void CHashContext::del_phrase_by_id(int index)
{
    if ( index>=pool.size() || index<0 )
    {
        return;
    }
    //  search the item
    int hi=pool.size()-1, lo=0 , p, v;
    while ( hi>=lo )
    {
        p = (hi+lo)/2;
        v = pool[p]->item_index-index;
        
        if ( v>0 )
            hi = p-1;
        else if ( v<0 )
            lo = p+1;
        else
        {
            release__HASH_ITEM(pool[p]);
            pool.erase(pool.begin()+p);
            break;
        }
    };
}

HASH_ITEM* CHashContext::append_phrase(const char *str, uint16 *phoneSeq)
{
    //  existing?

    //  new
    HASH_ITEM *hitem = (HASH_ITEM*) calloc(1, sizeof(HASH_ITEM));
    hitem->data.wordSeq = (char*) malloc(strlen(str)+1);
    hitem->data.phoneSeq = (uint16*) calloc(MAX_PHONE_SEQ_LEN+1, sizeof(uint16));
    strcpy(hitem->data.wordSeq, str);
    for ( int lop=0; *phoneSeq!=0; ++lop, ++phoneSeq )
    {
        hitem->data.phoneSeq[lop] = *phoneSeq;
    }
    pool.push_back(hitem);
    hitem->item_index = pool.size();
    sort_phrase();

    return	hitem;
}

HASH_ITEM* CHashContext::find_phrase_exact(const char *str, uint16 *phoneSeq)
{
	return	NULL;
}

HASH_ITEM* CHashContext::find_phrase_partial(const char *str)
{
	return	NULL;
}

int CHashContext::get_phrase_count()
{
    return  pool.size();
}

void CHashContext::_connect_server(void)
{
	serverWnd = FindWindow( chewingServerClassName, NULL );
	if( ! serverWnd )
	{
		LPCTSTR evtname = _T("Local\\ChewingServerEvent");
		DWORD osVersion = GetVersion();
 		DWORD major = (DWORD)(LOBYTE(LOWORD(osVersion)));
		DWORD minor =  (DWORD)(HIBYTE(LOWORD(osVersion)));
		if( osVersion >= 0x80000000 || major <= 4 )	// Windows 9x or Windows NT 4
			evtname += 6;	// remove prfix "Local\\"

		HANDLE evt = CreateEvent( NULL, TRUE, FALSE, evtname );
		TCHAR server_path[MAX_PATH];
		GetSystemDirectory( server_path, MAX_PATH );
		_tcscat( server_path, _T("\\IME\\Chewing\\ChewingServer.exe") );
		ShellExecute( NULL, "open", server_path, NULL, NULL, SW_HIDE );
		WaitForSingleObject( evt, 10000 );
		CloseHandle(evt);
		serverWnd = FindWindow( chewingServerClassName, NULL );
	}
	GetWindowText( serverWnd, filemapName, sizeof(filemapName) );
}

int CHashContext::_get_phone_seq_from_server(uint16 *pPhoneSeq)
{
    int phone_cnt = SendMessage(serverWnd, ChewingServer::cmdLastPhoneSeq, 0, 0 );
	if( phone_cnt>0 && phone_cnt<=MAX_PHONE_SEQ_LEN )
	{
	    HANDLE sharedMem = OpenFileMapping( FILE_MAP_ALL_ACCESS, FALSE, filemapName);
        if ( sharedMem==NULL )
        {
            sharedMem = INVALID_HANDLE_VALUE; 
            return  0;
        }

		uint16 *pword = (uint16*)MapViewOfFile( sharedMem, FILE_MAP_READ, 0, 0, CHEWINGSERVER_BUF_SIZE );
		if( pword!=NULL )
		{
            memcpy(pPhoneSeq, pword, sizeof(uint16)*phone_cnt);
            pPhoneSeq[phone_cnt] = 0;
			UnmapViewOfFile(pword);
		}
        else
        {
            phone_cnt = 0;
        }
    	CloseHandle(sharedMem);
        return  phone_cnt;
	}
    return  0;
}

void CHashContext::shutdown_server()
{
    SendMessage(serverWnd, WM_DESTROY, 0, 0 );
    do {
        Sleep(10);
    }   while ( SendMessage(serverWnd, 
                            ChewingServer::cmdEcho, 0, 0)!=0 );
}

#if 0
int main(int argc, char* argv[])
{
    CHashContext context;

    context.load_hash( HASH_FILE );
    context.sort_phrase();
    context.arrange_phrase();
    context.list_phrase();
    context.save_hash("xhash.dat");

    printf(context.get_phrase_by_id(343)->data.wordSeq);

	return 0;
}
#endif

BOOL CHashContext::isChineseString(char *str)
{
	while ( *str != NULL )	{
		int len = ueBytesFromChar( (unsigned char)*str );
		if ( len <= 1 )	{
			return	FALSE;
		}
		str += len;
	};
	return	TRUE;
}

CHashContext::CHashContext()
{
    pool.clear();
    chewing_lifetime = 0;
    serverWnd = NULL;
    memset(filemapName, 0, sizeof(filemapName));
	sprintf(formatstring, "%%-%ds", FIELD_SIZE ); 
}

CHashContext::~CHashContext()
{
    std::vector<HASH_ITEM*>::iterator iter;
    
    iter = pool.begin();
    while ( iter!=pool.end() )
    {
        release__HASH_ITEM(*iter);
        ++iter;
    }
}


