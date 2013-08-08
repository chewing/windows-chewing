/* @(#)Chewingpp.h
 */

#ifndef _CHEWINGPP_H
#define _CHEWINGPP_H 1

#include "chewing.h"
#include <cstdint>
#include <cstdlib>
#pragma comment(lib, "chewing")

#ifdef	MAX_UTF8_SIZE
#define	MAX_CHAR_SIZE	(MAX_UTF8_SIZE)
#else
#define	MAX_CHAR_SIZE	(2)
#endif

#if !defined(MAX_PHONE_SEQ_LEN)
#define MAX_PHONE_SEQ_LEN 50
#endif

class Chewing
{
  ChewingContext *ctx;

  static uint16_t g_lastPhoneSeq[MAX_PHONE_SEQ_LEN];

 public:
  static bool LoadDataFiles( const char* dataDir, const char* userDir );
  static void ReloadSymbolTable( const char* datadir, const char* userDir );
  static uint16_t* GetLastPhoneSeq( void );

  Chewing( bool spaceAsSelection = true, int keyLayout=0);
  ~Chewing();

  void SetKeyboardLayout(int kb);
  void SetHsuSelectionKeyType(int type);

  // Debugging-only retuine.
  void Dump();

  int Space();
  int Esc();
  int Enter();
  int Delete();
  int Backspace();
  int Tab();
  int ShiftLeft();
  int ShiftRight();
  int ShiftSpace();
  int Right();
  int Left();
  int Up();
  int Down();
  int Home();
  int End();
  int Capslock();
  int Key(unsigned int code);
  int CtrlNum(unsigned int code);
  int NumPad(unsigned int code);
  int DoubleTab();

  // Return the i-th selection key, i >= 0.
  char SelKey(int i);
  void SelKey(char* selkey);

  char* ZuinStr();
  char* CommitStr();
  uint16_t* PhoneSeq();
  int   CommitReady();

  char* Buffer();
  int   BufferLen();

  int CursorPos();
  int KeystrokeIgnore();

  int ChineseMode();

  // CandidateWindow-related routines
  int Candidate();
  int ChoicePerPage();
  int TotalChoice();
  int TotalPage();
  int CurrentPage();

  // Return the i-th selection wchar, i >= 0.
  char* Selection(int i);
  void SetFullShape(bool full);
  bool GetFullShape(void);
  void SetSpaceAsSelection(bool spaceAsSelection);
  void SetAdvanceAfterSelection(int bDo);
  void SetEasySymbolInput(int bSet);
  int ShowMsgLen(void);
  char* ShowMsg(void);
  void SetAddPhraseForward(bool add_forward);
  bool GetAddPhraseForward(void);
  void SetCandPerPage(int len);
  void SetEscCleanAllBuf( bool escCleanAllBuf );

  int IntervalLen();
  unsigned char* IntervalArray();
};

#endif /* _CHEWING_H */

