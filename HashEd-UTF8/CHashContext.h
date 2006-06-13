#ifndef _CHEWING_WIN32__HASHED_
#define _CHEWING_WIN32__HASHED_


#include <vector>
#include "hash.h"


/**
 *  A context that load a hash.dat,
 *  and provide some function to manipulate the context.
 */
class CHashContext
{
public:
    /**
     *  Load specify hash.dat
     *  retval -1   find bad data entry
     */
    int load_hash(const char *file, bool doClear=true);

    /**
     *  Save current context to target file
     */
    bool save_hash(const char *destFile);

    /**
     *  Sort the phrases
     */
    void sort_phrase();

    /**
     *  Delete redundancy phrase
     */
    bool arrange_phrase();

    /**
     *  Fro debug purpose only.
     */
    void list_phrase();

    /**
     *  Find phrase by array index of CHashContext.pool
     */
    HASH_ITEM* get_phrase_by_id(int array_index);

    /**
     *  Remove phrase by HASH_ITEM.item_index
     */
    void del_phrase_by_id(int index);

    /**
     *  Append a phrase into context
     *  You must provide the big5 string and its phoneSeq
     */
    HASH_ITEM* append_phrase(const char *str, uint16 *phoneSeq);

    /**
     *  Find a phrase in exactly match.
     */
    HASH_ITEM* find_phrase_exact(const char *str, uint16 *phoneSeq);

    /**
     *  Find a phrase in partial match.
     */
    HASH_ITEM* find_phrase_partial(const char *str);

    /**
     *  How many phrase in this context?
     */
    int get_phrase_count();

    /**
     *  Connect | launch chewing-server
     */
    void _connect_server(void);

    /**
     *  Retrive lastest phoneSeq info from chewing-server
     */
    int _get_phone_seq_from_server(uint16 *pPhoneSeq);

    /**
     *  Shutdown chewing-server
     */
    void shutdown_server();

    /**
     */
    void release__HASH_ITEM(HASH_ITEM *pItem);

    /**
     */
    void clear();

    /**
     */
    static BOOL isChineseString(const char *str);

    CHashContext();
    ~CHashContext();

private:
    std::vector<HASH_ITEM*> pool;
    int chewing_lifetime;

    HWND serverWnd;
    char filemapName[MAX_PATH];
    char formatstring[ 30 ];
};





#endif