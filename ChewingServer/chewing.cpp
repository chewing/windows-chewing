#include <cstring>
#include "Chewingpp.h"

uint16_t Chewing::g_lastPhoneSeq[MAX_PHONE_SEQ_LEN] = {0};

Chewing::Chewing(  bool spaceAsSelection, int keyLayout ) {
    ctx = chewing_new();
    
	chewing_set_KBType( ctx, keyLayout );
	chewing_set_candPerPage( ctx, 9 );
	chewing_set_maxChiSymbolLen( ctx, 20 );
	//chewing_set_addPhraseDirection( ctx, 1 );
	//chewing_set_selKey( ctx, selKey_define, 10 );
	chewing_set_spaceAsSelection( ctx, spaceAsSelection );
}

Chewing::~Chewing()
{
	chewing_Terminate();
	chewing_delete(ctx);
}

void Chewing::SetHsuSelectionKeyType(int type)
{
    if(type > HSU_SELKEY_TYPE2)
        chewing_set_hsuSelKeyType( ctx, HSU_SELKEY_TYPE1 );
    else
		chewing_set_hsuSelKeyType( ctx, type );
}

void Chewing::SetKeyboardLayout(int kb)
{
/*    if (kb==KB_HSU) {
        if(cd->config.hsuSelKeyType == HSU_SELKEY_TYPE2) {
            SelKey("asdfzxcv89");
        } else {
            SelKey("asdfjkl789");
        }
    } else if (kb==KB_DVORAK_HSU) SelKey("aoeuhtn789");
    else SelKey("1234567890");
*/
    chewing_set_KBType(ctx, kb);
}

/*
 * Return i-th selection-key. i starts from 0.
 */
char Chewing::SelKey(int i) {
	char ret;
	int *selkey = chewing_get_selKey( ctx );
	if ( 0 <= i && i < 10 )
		ret = selkey[i];
	else
		ret = 0;
	chewing_free( selkey );
	return ret;
}

void Chewing::SelKey(char *selkey) {
	int sk[10];
	for (int i = 0; i < 10;i++ )
		sk[i] = selkey[i];	
	chewing_set_selKey( ctx, sk, 10 );
}

int Chewing::Space()     { return chewing_handle_Space(ctx); }
int Chewing::Enter() {
	int rt;
	uint16_t *oldSeq = chewing_get_phoneSeq(ctx);
	int seqLen = chewing_get_phoneSeqLen(ctx);
	rt = chewing_handle_Enter(ctx);
	if (chewing_commit_Check(ctx)) {		
		memcpy(g_lastPhoneSeq, oldSeq, seqLen*sizeof(uint16_t));
		g_lastPhoneSeq[seqLen] = 0;
	}
	chewing_free(oldSeq);
	return rt;
}
int Chewing::Delete()    { return chewing_handle_Del(ctx); }
int Chewing::Backspace() { return chewing_handle_Backspace(ctx); }
int Chewing::Tab()       { return chewing_handle_Tab(ctx); }
int Chewing::ShiftLeft() { return chewing_handle_ShiftLeft(ctx); }
int Chewing::ShiftRight() { return chewing_handle_ShiftRight(ctx); }
int Chewing::ShiftSpace() { return chewing_handle_ShiftSpace(ctx); }
int Chewing::Right()     { return chewing_handle_Right(ctx);}
int Chewing::Left()      { return chewing_handle_Left(ctx);}
int Chewing::Up()        { return chewing_handle_Up(ctx);}
int Chewing::Down()      { return chewing_handle_Down(ctx);}
int Chewing::Home()      { return chewing_handle_Home(ctx);}
int Chewing::End()       { return chewing_handle_End(ctx);}
int Chewing::Capslock()  { return chewing_handle_Capslock(ctx);}
int Chewing::DoubleTab() { return chewing_handle_DblTab(ctx);}
int Chewing::Esc()   { return chewing_handle_Esc(ctx); }
int Chewing::CtrlNum(unsigned int code)    { return chewing_handle_CtrlNum(ctx ,(int)code);}
int Chewing::NumPad(unsigned int code)    { return chewing_handle_Numlock(ctx, (int)code);}
int Chewing::Key(unsigned int code) {
    //SetKeyboardLayout(kbLayout);
    return chewing_handle_Default(ctx, (int)code);
}

uint16_t* Chewing::GetLastPhoneSeq() {
	return g_lastPhoneSeq;
}

/*
  CommitReady() returns TRUE if it currnet input state
  is at the "end-of-a-char".
 */
int Chewing::CommitReady() {
  return chewing_commit_Check(ctx);
}

/*
  CommitStr() retrun currnet commit string, regardless
  current input state.

  Alwasy returns a char pointer, caller must free it.
 */

char* Chewing::CommitStr() {
    return chewing_commit_String(ctx);
}

/*
  Always returns a char pointer, caller must free it.
 */
char* Chewing::ZuinStr() {
	return chewing_zuin_String(ctx, NULL);
}

char* Chewing::Buffer() {
	return chewing_buffer_String(ctx);
}

int Chewing::BufferLen() {
	return chewing_buffer_Len(ctx);
}

int Chewing::CursorPos() {
  return (int)chewing_cursor_Current(ctx);
}

int Chewing::KeystrokeIgnore() {
  return chewing_keystroke_CheckIgnore(ctx);
}

int Chewing::ChineseMode() {
  return (chewing_get_ChiEngMode(ctx) == CHINESE_MODE);
}

int Chewing::Candidate() {
	return chewing_cand_TotalPage(ctx);
}

int Chewing::ChoicePerPage() {
	return chewing_cand_ChoicePerPage(ctx);
}

int Chewing::TotalChoice() {
	return chewing_cand_TotalChoice(ctx);
}

int Chewing::TotalPage() {
	return chewing_cand_TotalPage(ctx);
}

int Chewing::CurrentPage() {
	return chewing_cand_CurrentPage(ctx);
}

/*
 * Return the i-th candidate wchar (WCH_SIZE bytes), i starts from 0.
 * User MUST free() the return value of this sub-routine,
 * even if it's a false value.
 */
char* Chewing::Selection(int i) {
	/* FIXME: This can be more efficient */
  int j = 0;
  chewing_cand_Enumerate(ctx);
  for (j = 0; chewing_cand_hasNext(ctx) && j < i; j++)
	  chewing_free(chewing_cand_String(ctx));
  return chewing_cand_String(ctx);
}

// A debugging purpose Dumping routing. Output current inputed Zuin
// and Commit string.
void Chewing::Dump() {
#if 0
  fprintf(stderr,"nCommitStr = %d\n",co->nCommitStr);
  
  fprintf(stderr,"zuin: ");

  int i;
  for(i=0;i<ZUIN_SIZE;i++) 
    if(co->zuinBuf[i].s)
      fprintf(stderr,"%s",co->zuinBuf[i].s);
  fprintf(stderr,"\n");
  
  fprintf(stderr,"commit: ");
  for(i=0;i<co->nCommitStr;i++) 
    fprintf(stderr,"%s",co->commitStr[i].s);
  fprintf(stderr,"\n");
  fprintf(stderr,"-----\n");
#endif
}

void Chewing::SetFullShape(bool full)
{
	chewing_set_ShapeMode(ctx, full ? FULLSHAPE_MODE : HALFSHAPE_MODE );
}

bool Chewing::GetFullShape(void)
{
	return chewing_get_ShapeMode(ctx) == FULLSHAPE_MODE;
}

void Chewing::SetAdvanceAfterSelection(int bDo)
{
    chewing_set_autoShiftCur(ctx, bDo);
}

void Chewing::SetEasySymbolInput(int bSet)
{
    chewing_set_easySymbolInput(ctx, bSet);
}

bool Chewing::LoadDataFiles(const char *dataDir, const char *userDir)
{
	chewing_Init(dataDir, userDir);
	return true; 
}

void Chewing::SetSpaceAsSelection(bool spaceAsSelection)
{
	chewing_set_spaceAsSelection(ctx, spaceAsSelection);
}

void Chewing::SetEscCleanAllBuf( bool escCleanAllBuf ) {
	chewing_set_escCleanAllBuf(ctx, escCleanAllBuf);
}

int Chewing::ShowMsgLen(void)
{
	return chewing_aux_Length(ctx);
}

char* Chewing::ShowMsg(void)
{
	return chewing_aux_String(ctx);
}

void Chewing::SetAddPhraseForward(bool add_forward)
{
	chewing_set_addPhraseDirection(ctx, add_forward);
}

bool Chewing::GetAddPhraseForward(void)
{
	return !!chewing_get_addPhraseDirection(ctx);
}

void Chewing::SetCandPerPage(int len)
{
	chewing_set_candPerPage(ctx, len);
}

// Return the length of interval array
int Chewing::IntervalLen()
{
	int i;
	chewing_interval_Enumerate(ctx);
	for (i = 0; chewing_interval_hasNext(ctx); i++)
		chewing_interval_Get(ctx, NULL);
	return i*2;
}

//	Return interval array
//	caller must free return value
unsigned char* Chewing::IntervalArray()
{
	unsigned char* interval = (unsigned char*)calloc( IntervalLen(), sizeof( unsigned char ) );
	IntervalType it;
	chewing_interval_Enumerate(ctx);
	for( int i = 0; chewing_interval_hasNext(ctx); ++i ) {
		chewing_interval_Get(ctx, &it);
		interval[i * 2] = it.from;
		interval[i * 2 + 1] = it.to;
	}
	return interval;
}

void TerminateSymbolTable();
void TerminateEasySymbolTable();
int InitSymbolTable( const char *prefix );
int InitEasySymbolInput( const char *prefix );

void Chewing::ReloadSymbolTable(const char* dataDir, const char* userDir )
{
	chewing_Terminate();
	chewing_Init(dataDir, userDir);
}
