#include "MainDialog.h"

/**************************************************************************
	[Function]
		int WINAPI _tWinMain()
	[Details]
		Application Entry Point
	[Argument1] hInst
		インスタンスハンドル1
	[Argument2] hPrevInst
		インスタンスハンドル2(=NULL)
	[Argument3] pCmdLine
		コマンドライン制御
	[Argument4] showCmd
		ウインドウ制御
	[Return]
		成功  : 0	失敗 : 1
**************************************************************************/
int WINAPI _tWinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR pCmdLine, int showCmd)
{
	CMainDialog dlg;
	return dlg.DoModal(hInst);
}

