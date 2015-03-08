#pragma once

// CMainDialog : 定義

#include "Proc.h"
#include <algorithm>
#include <shlobj.h>

// Define
#define THIS_VERSION	_T("1.0.0.3")
#define COPYRIGHT		_T("Copyright (C) 2011 OLYMPUS IMAGING CORP. All Rights Reserved.")


/**************************************************************************
	[Class]
		CMainDialog
	[Details]
		メインダイアログ
**************************************************************************/
class CMainDialog : public CModalDlg
{

/* Constructor / Destructor */
public:
	CMainDialog(void)
		:CModalDlg(IDD_MAIN_DIALOG)
	{
		// ロケールを日本語に設定
		_tsetlocale(LC_ALL, _T("Japanese"));
	};
	~CMainDialog(void)
	{
	};

/* Member */
protected:
	WNDPROC DefStaticProc;
private:
	// GUIコントロールのウインドウハンドル
	HWND hCombo, hCheck;


/* Private Member Function */
	/*------------------------------------------------------------------------
		[Function]
			OnInitDialog
		[Details]
			ダイアログ初期化
		[Return]
			TRUEを返す
	------------------------------------------------------------------------*/
	virtual BOOL OnInitDialog(const HWND hWnd, const HWND hwndFocus, const LPARAM lParam)
	{
		TSTR title = _T("Exifタグフォルダ分け  Version ");
		title += THIS_VERSION;
		SetWindowText(hDlg,title.data());
		// 既存のコントロールからハンドルを取得
		hCombo = GetDlgItem(hWnd,IDC_TAGLIST_COMBO);
		hCheck = GetDlgItem(hWnd,IDC_COPY_CHECK);
		// コンボボックス初期化
		return InitTaglistCombo();
	}


	/*------------------------------------------------------------------------
		[Function]
			InitTaglistCombo
		[Details]
			コンボボックス初期化
		[Argument1] hDlg
			ウインドウハンドル
		[Return]
			成功：TRUE  失敗：FALSE
	------------------------------------------------------------------------*/
	BOOL InitTaglistCombo(void)
	{
		std::map<ULONG,TSTR> StdTag;
		CImgExif::InitStdTag(StdTag);
		const ULONG TagID[] =
		{// ここで選択対象にするタグIDを任意に登録する
			0x010F,
			0x0110,
			0xA002,
			0xA003,
			0x829A,
			0x829D,
			0x8827,
			0x9204,
			0x920A,
			0x0102,
			0x0104,
			0x0203,
		};
		const TSTR xmlpath = GetModuleDir() + _T("\\tagsetJ.xml");
		CXml pXml(xmlpath.data());
		if(!pXml.IsXmlLoad()){
			MessageBox(NULL,_T("""tagsetJ.xml""の読み込みに失敗しました"),_T("Error"), MB_OK | MB_ICONERROR);
			return FALSE;
		}
		/*{// xmlから取得した情報で更新する
			std::vector<TSTR> str(0,_T(""));
			pXml.GetAttribute(str,_T("entry"),_T("name"));
			std::vector<CXml::EntryAttribute> eAttribute;
			pXml.GetEntryAttribute(eAttribute);
			for(int i=0; i<str.size(); i++)
			{
				SendMessage(hCombo,CB_INSERTSTRING,i,(LPARAM)str[i].data());
			}
		}*/
		for(int i=0; i<sizeof(TagID)/sizeof(TagID[0]); i++)
		{
			SendMessage(hCombo,CB_INSERTSTRING,i,(LPARAM)StdTag[TagID[i]].data());
		}
		// 一番目のアイテムを選択
		SendMessage(hCombo, CB_SETCURSEL,0,0);
		return TRUE;
	}


	/*------------------------------------------------------------------------
		[Function]
			Execution
		[Details]
			処理を実行する
		[Argument1] files
			ユーザーがドロップしたファイル
		[Return]
			成功 : 1  失敗 : 0  エラー : -1
	------------------------------------------------------------------------*/
	int Execution(const std::vector<FileInfo>& files)
	{
		if(files.size() < 1) return 0;
		const BOOL CCheck = (BOOL)SendMessage(hCheck,BM_GETCHECK,0,NULL);
		const TSTR SelectTag = GetWindowTxtEx(hCombo);
		CProc proc(hDlg,CCheck,SelectTag);
		return proc.Execution(files);
	}


	/*------------------------------------------------------------------------
		[Function]
			OnDropFile
		[Details]
			ファイルドラッグ＆ドロップイベント処理
		[Return]
			FALSEを返す
	------------------------------------------------------------------------*/
	virtual void OnDropFile(const WPARAM wParam)
	{
		std::vector<FileInfo> files;
		if(ReadDropfile(files,wParam) != 1) return;
		Execution(files);
	}


	/*------------------------------------------------------------------------
		[Function]
			ReadDropfile
		[Details]
			ドラッグ＆ドロップファイル読み込み
		[Argument1] hWnd
			ウインドウハンドル
		[Argument2] wParam
			パラメータ
		[Return]
			成功 : 1  失敗 : 0  エラー : -1
	------------------------------------------------------------------------*/
	int ReadDropfile(std::vector<FileInfo>& dst,const WPARAM wParam)
	{
		try
		{
			TCHAR FileName[MAX_PATH * 0xFF] = _T("");
			HDROP hDrop = (HDROP)wParam;

			int ic = 0;
			{// ドロップファイル数を取得
				ic = DragQueryFile(hDrop,0xFFFFFFFF,FileName,256);
				if(ic < 1){
					MessageBox(NULL,_T("ファイルをドロップできませんでした"),_T("Error"), MB_OK | MB_ICONERROR);
					return 0;
				}
			}
			
			{// 取得
				POINT pDrop;
				DragQueryPoint(hDrop,&pDrop);
				for(int i=0; i < ic; i++)
				{// ファイル数分
					const int StrLength = DragQueryFile(hDrop,i,FileName,MAX_PATH);
					FileName[StrLength] = '\0';
					FileInfo buf;
					{
						const TCHAR* dir = FileName;
						buf.full = dir;
						buf.dir = GetDirName(dir);
						buf.file = GetFileName(dir);
					}
					dst.push_back(buf);
				}
				DragFinish(hDrop);
			}
			SortFileData(dst);
			return 1;
		}
		catch(...)
		{
			MessageBox(NULL,_T("ファイルをドロップ中にエラーが発生しました"),_T("Error"), MB_OK | MB_ICONERROR);
			return -1;
		}
	}

	
	/*------------------------------------------------------------------------
		[Function]
			SortFileData
		[Details]
			ファイル情報を昇順にソートする
		[Argument1] src
			ソートするデータへの参照
		[Remarks]
			構造体のvector配列はstd::sortの対象外だったのでメンバに分解してソートする
	------------------------------------------------------------------------*/
	void SortFileData(std::vector<FileInfo>& src)
	{
		std::vector<FileInfo> Buf;
		{// ソートデータを取得
			std::vector<TSTR> sorttmp(0,_T(""));
			{// 比較用の構造体のメンバだけ取り出してソート
				for(UINT i=0; i<src.size(); i++){
					sorttmp.push_back(src[i].full);
				}
				std::sort(sorttmp.begin(), sorttmp.end());
			}
			for(UINT i=0; i<sorttmp.size(); i++)
			{// 上記でソートされたデータを利用して本ソートする
				for(UINT j=0; j<src.size(); j++)
				{
					if(sorttmp[i] == src[j].full)
					{
						Buf.push_back(src[j]);
						break;
					}
				}
			}
		}
		// コピー
		src = Buf;
	}

}; // CMainDialog

