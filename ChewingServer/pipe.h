/**
 * pipe.c
 *
 * Copyright (c) 2007
 *	libchewing.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file.
 */

#ifndef _LIBCHEWING_WIN32_PIPE_IMPLEMENT_
#define _LIBCHEWING_WIN32_PIPE_IMPLEMENT_


#define	PIPE_MAX_BUFFER_SZ	4096
#define PIPE_NAME_PREFIX	"LIBCHEWING_WIN32_PIPE_"

typedef struct {
	void *handle;
	char *message;
	char *response;
} ChewingMsgPacket;





ChewingMsgPacket* FetchClientRequest(char *pipename);

int ReplyClientRequest(ChewingMsgPacket *pkt);

int DestroyPacket(ChewingMsgPacket *pkt);



#endif
