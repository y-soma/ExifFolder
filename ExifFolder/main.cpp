#include "MainDialog.h"

/**************************************************************************
	[Function]
		int WINAPI _tWinMain()
	[Details]
		Application Entry Point
	[Argument1] hInst
		�C���X�^���X�n���h��1
	[Argument2] hPrevInst
		�C���X�^���X�n���h��2(=NULL)
	[Argument3] pCmdLine
		�R�}���h���C������
	[Argument4] showCmd
		�E�C���h�E����
	[Return]
		����  : 0	���s : 1
**************************************************************************/
int WINAPI _tWinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR pCmdLine, int showCmd)
{
	CMainDialog dlg;
	return dlg.DoModal(hInst);
}

