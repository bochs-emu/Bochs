#ifndef BX_ENH_DBG_DEF_H
#define BX_ENH_DBG_DEF_H

#include "win32dialog.h"

void MoveLists();
void SetStatusText(int column, char *buf); // should it be here ?
void MakeListsGray();
void DispMessage(char *msg, char *title);
void InsertListRow(char *ColumnText[], int ColumnCount, int listnum, int LineCount, int grouping);
void StartListUpdate(int listnum);
void EndListUpdate(int listnum);
void RedrawColumns(int listnum);
void Invalidate(int i);
void TakeInputFocus();
bx_bool ShowAskDialog();
bx_bool NewFont();
void GrayMenuItem(int flag, int CmdIndex);
void ShowMemData(bx_bool initting);
void SetMenuCheckmark (int flag, int CmdIndex);
void ClearInputWindow();
void VSizeChange();
void ToggleWSchecks(int newWS, int oldWS);
void SetOutWinTxt();
void ShowFW();
void GetInputEntry(char *buf);
void SelectHistory(int UpDown);

void HideTree();
void FillPTree();

int GetASMTopIdx();
void ScrollASM(int pixels);

void GetLIText(int listnum, int itemnum, int column, char *buf);
void SetLIState(int listnum, int itemnum, bx_bool Select);
int GetNextSelectedLI(int listnum, int StartPt);

bx_bool OSInit();
void SpecialInit();
void dbgOnToggle(bx_bool on);

#define CHK_CMD_MODEB   CMD_MODEB
#define CHK_CMD_ONECPU  CMD_ONECPU
#define CHK_CMD_UCASE   CMD_UCASE
#define CHK_CMD_IOWIN   CMD_IOWIN
#define CHK_CMD_SBTN    CMD_SBTN
#define CHK_CMD_MHEX    CMD_MHEX
#define CHK_CMD_MASCII  CMD_MASCII
#define CHK_CMD_LEND    CMD_LEND
#define CHK_CMD_IGNSA   CMD_IGNSA
#define CHK_CMD_IGNNT   CMD_IGNNT
#define CHK_CMD_RCLR    CMD_RCLR
#define CHK_CMD_EREG    CMD_EREG
//#define CHK_CMD_SREG  S_REG       HIHI -- don't need these?
//#define CHK_CMD_SYSR  SYS_R
//#define CHK_CMD_CREG  C_REG
//#define CHK_CMD_FPUR  FPU_R
//#define CHK_CMD_XMMR  XMM_R
//#define CHK_CMD_DREG  D_REG
//#define CHK_CMD_TREG  T_REG

#endif

