// Tools : �ėp�֐��S

#pragma once

// Include
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <string>
#include <vector>
#include <math.h>
#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")


// typedef
typedef std::basic_string<TCHAR> TSTR;

/**************************************************************************
	[Structure]
		aRGB
	[Details]
		RGB�̕��ϒl���i�[���邽�߂̍\����
**************************************************************************/
typedef struct A_RGB {
	double R;
	double G;
	double B;
	
	/* Constructor */
	A_RGB()
	{
		R = 0.0;
		G = 0.0;
		B = 0.0;
	}
} aRGB, *PaRGB;


/**************************************************************************
	[Structure]
		FileInfo
	[Details]
		�J�����t�@�C���̃p�X���Ǘ�����\����
**************************************************************************/
typedef struct FILE_INFO{
	// �t���p�X
	TSTR full;
	// �f�B���N�g��
	TSTR dir;
	// �t�@�C����
	TSTR file;
	
	/* Constructor */
	FILE_INFO()
	{
		full.empty();
		dir.empty();
		file.empty();
	}
} FileInfo, *PFileInfo;


// Function Proto Type
TSTR GetFileName(const TCHAR* const fullPath);
TSTR GetDirName(const TCHAR* const fullPath);
TSTR GetModuleDir(void);
TSTR LToStr(const long& val);
TSTR LToStr(const ULONG& val);
TSTR DToStr(const double& val, const int& precision = -1);
double StrToD(const TCHAR* const str);
long StrToL(const TCHAR* const str);
std::vector<TSTR> Split(const TCHAR* const str, const TCHAR* const delim);
std::basic_string<wchar_t> ToUnicode(const char* const str);
std::basic_string<char> ToMBCS(const wchar_t* const str);
TSTR RemoveNull(const TSTR& str);
double RoundEx(const double& src, const int& precision);
int CheckPrecision(const double& src);
BOOL PathIsDirectoryEX(const TCHAR* const path);
TSTR GetMakeFilePath(const TCHAR* const path, const TCHAR* const opfname = _T("log"));

