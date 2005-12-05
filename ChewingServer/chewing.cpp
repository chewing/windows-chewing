#include <string.h>
#include <winsock.h>
#include "Chewingpp.h"
#include ".\chewingpp.h"

Chewing::Chewing(  bool spaceAsSelection, int keyLayout ) {
    cf = (ChewingConf *) calloc( 1, sizeof( ChewingConf ) );
    cd = (ChewingData *) calloc( 1, sizeof( ChewingData ) );
    co = (ChewingOutput *) calloc( 1, sizeof( ChewingOutput ) );
    kbLayout = keyLayout;

    cf->inp_cname = "Chewing";
    cf->inp_ename = "Chewing";
    cf->kb_type   = kbLayout;

    InitChewing(cd,cf);

    config.selectAreaLen = 40;
    config.maxChiSymbolLen = 20;
	config.bSpaceAsSelection = spaceAsSelection;

    SetKeyboardLayout(kbLayout);
}

void Chewing::SetHsuSelectionKeyType(int type)
{
    if(type > HSU_SELKEY_TYPE2)
        cd->config.hsuSelKeyType = HSU_SELKEY_TYPE1;
    else
        cd->config.hsuSelKeyType = type;
}

void Chewing::SetKeyboardLayout(int kb)
{
    kbLayout = kb;
/*    if (kb==KB_HSU) {
        if(cd->config.hsuSelKeyType == HSU_SELKEY_TYPE2) {
            SelKey("asdfzxcv89");
        } else {
            SelKey("asdfjkl789");
        }
    } else if (kb==KB_DVORAK_HSU) SelKey("aoeuhtn789");
    else SelKey("1234567890");
*/
    cd->zuinData.kbtype=kb;
    SetConfig(cd, &config);
}

/*
 * Return i-th selection-key. i starts from 0.
 */
char Chewing::SelKey(int i) {
  if(0 <= i && i < 10)
    return config.selKey[i];
  return 0;
}

void Chewing::SelKey(char *selkey) {
  for (int i = 0; i < 10;i++ ) config.selKey[ i ] = selkey[i];
  SetConfig( cd, &config );
}

int Chewing::Space()     { return OnKeySpace((void*)cd,co); }
int Chewing::Enter()     { return OnKeyEnter((void*)cd,co); }
int Chewing::Delete()    { return OnKeyDel((void*)cd,co); }
int Chewing::Backspace() { return OnKeyBackspace((void*)cd,co); }
int Chewing::Tab()       { return OnKeyTab((void*)cd,co); }
int Chewing::ShiftLeft() { return OnKeyShiftLeft((void*)cd,co); }
int Chewing::ShiftRight() { return OnKeyShiftRight((void*)cd,co); }
int Chewing::ShiftSpace() { return OnKeyShiftSpace((void*)cd,co); }
int Chewing::Right()     { return OnKeyRight((void*)cd, co);}
int Chewing::Left()      { return OnKeyLeft((void*)cd, co);}
int Chewing::Up()        { return OnKeyUp((void*)cd, co);}
int Chewing::Down()      { return OnKeyDown((void*)cd, co);}
int Chewing::Home()      { return OnKeyHome((void*)cd, co);}
int Chewing::End()       { return OnKeyEnd((void*)cd, co);}
int Chewing::Capslock()  { return OnKeyCapslock((void*)cd, co);}
int Chewing::DoubleTab() { return OnKeyDblTab((void*)cd, co);}
int Chewing::Esc()   { return OnKeyEsc((void*)cd,co); }
int Chewing::CtrlOption(unsigned int code) { return OnKeyCtrlOption((void*)cd,(int)code, co);}
int Chewing::CtrlNum(unsigned int code)    { return OnKeyCtrlNum((void*)cd, (int)code,co);}
int Chewing::NumPad(unsigned int code)    { return OnKeyNumlock((void*)cd, (int)code,co);}
int Chewing::Key(unsigned int code) {
    SetKeyboardLayout(kbLayout);
    return OnKeyDefault((void*)cd,(int)code,co);
}

/*
  CommitReady() returns TRUE if it currnet input state
  is at the "end-of-a-char".
 */
int Chewing::CommitReady() {
  return (co->keystrokeRtn & KEYSTROKE_COMMIT);
}

/*
  CommitStr() retrun currnet commit string, regardless
  current input state.

  Alwasy returns a char pointer, caller must free it.
 */

char* Chewing::CommitStr() {
    return CommitStr(0,co->nCommitStr-1);
}

char* Chewing::CommitStr(int from) {
    return CommitStr(from,co->nCommitStr-1);
}

char* Chewing::CommitStr(int from, int to) {
  char *s = (char*) calloc (1 + co->nCommitStr, sizeof(char) * MAX_UTF8_SIZE);
  if(from >= 0 && to < co->nCommitStr ) {
    for(int i=from; i <= to ; i++)
      strcat(s,(char*)(co->commitStr[i].s));
  }
  return s;
}

/*
  Always returns a char pointer, caller must free it.
 */
char* Chewing::ZuinStr() {
  char *s;
  s = (char*) calloc ( 1+ ZUIN_SIZE ,sizeof(char) * WCH_SIZE );
  for(int i = 0; i < ZUIN_SIZE ; i++)
    if(co->zuinBuf[i].s) strcat(s,(char*)(co->zuinBuf[i].s));
  return s;
}

int Chewing::BufferLen() {
  return co->chiSymbolBufLen;
}

char* Chewing::Buffer() { return Buffer(0,BufferLen() - 1); }
char* Chewing::Buffer(int from) {
  return Buffer(from, BufferLen() - 1);
}
char* Chewing::Buffer(int from,int to) {
  char *s;
  s = (char*)calloc( 1+ BufferLen(), sizeof(char) * MAX_UTF8_SIZE);
  if(from >= 0 && to < BufferLen() ) {
    for(int i = from; i <= to; i++) {
      strcat(s,(char*)(co->chiSymbolBuf[i].s));
    }
  }
  return s;
}

int Chewing::CursorPos() {
  return co->chiSymbolCursor;
}

int Chewing::PointStart() {
  return co->PointStart;
}

int Chewing::PointEnd() {
  return co->PointEnd;
}

int Chewing::KeystrokeRtn() {
  return co->keystrokeRtn;
}

int Chewing::KeystrokeIgnore() {
  return (co->keystrokeRtn & KEYSTROKE_IGNORE);
}

int Chewing::ChineseMode() {
  return (cd->bChiSym == CHINESE_MODE);
}

int Chewing::Candidate() {
	return (co->pci ? co->pci->nPage : 0);
}

int Chewing::ChoicePerPage() {
	return (co->pci ? co->pci->nChoicePerPage : 0);
}

int Chewing::TotalChoice() {
	return (co->pci ? co->pci->nTotalChoice : 0);
}

int Chewing::TotalPage() {
	return (co->pci ? co->pci->nPage : 0);
}

int Chewing::CurrentPage() {
	return (co->pci ? co->pci->pageNo : -1);
}

/*
 * Return the i-th candidate wchar (WCH_SIZE bytes), i starts from 0.
 * User MUST free() the return value of this sub-routine,
 * even if it's a false value.
 */
char* Chewing::Selection(int i) {
  char *s;
  int no = co->pci->pageNo * ChoicePerPage() + i;

  if(i >=0 && no < co->pci->nTotalChoice)
    s = strdup(co->pci->totalChoiceStr[no]);
  else
    s = strdup("");

  return s;
}

// A debugging purpose Dumping routing. Output current inputed Zuin
// and Commit string.
void Chewing::Dump() {
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
}

void Chewing::SetFullShape(bool full)
{
	::SetShapeMode( cd, full ? FULLSHAPE_MODE : HALFSHAPE_MODE );
}

bool Chewing::GetFullShape(void)
{
	return !!cd->bFullShape;
}

void Chewing::SetAdvanceAfterSelection(int bDo)
{
    cd->bAutoShiftCur = bDo;
}

bool Chewing::LoadDataFiles(const char *dataDir, const char *hashDir)
{
    ReadTree( (char*)dataDir );
    InitChar( (char*)dataDir );
    InitDict( (char*)dataDir );
    ReadHash( (char*)hashDir );
	return true; 
}

void Chewing::SetSpaceAsSelection(bool spaceAsSelection)
{
	config.bSpaceAsSelection = spaceAsSelection;
    SetConfig(cd, &config);
}

int Chewing::ShowMsgLen(void)
{
	return co->bShowMsg ? co->showMsgLen : 0;
}

char* Chewing::ShowMsg(void)
{
	char *msg = (char*) calloc (1 + co->showMsgLen, sizeof(char) * MAX_UTF8_SIZE);
	for(int i = 0; i < co->showMsgLen ; ++i )
		strcat(msg, (char*)(co->showMsg[i].s));
	return msg;
}

void Chewing::SetAddPhraseForward(bool add_forward)
{
	cd->config.bAddPhraseForward = add_forward;
}

bool Chewing::GetAddPhraseForward(void)
{
	return !!cd->config.bAddPhraseForward;
}

void Chewing::SetSelAreaLen(int len)
{
	cd->config.selectAreaLen = len;
}
