#pragma once

#include "resource.h"
#include "ImgExif.h"
#include "Dialog.h"
#include <omp.h>


/**************************************************************************
	[Class]
		CProc
	[Details]
		メイン処理定義クラス
**************************************************************************/
class CProc
{
private:
	/**************************************************************************
		[Structure]
			FileTag
		[Details]
			画像ファイル1つ分のタグ情報を管理する構造体
	**************************************************************************/
	typedef struct IMG_TAG {
		FileInfo finfo;
		UINT tagid;
		TSTR tagvalue;
		IMG_TAG() {/* Constructor */
			tagid = 0;
			tagvalue.clear();
		}
	} ImgTag, *PImgTag;

private:
	HWND hWnd;
	BOOL CopyCheck;
	TSTR SelectTag;
	std::map<ULONG,TSTR> StdTag;

public:
	CProc(const HWND& GethWnd, const BOOL& GetCopyCheck, const TSTR& GetSelectTag)
		:hWnd(GethWnd),CopyCheck(GetCopyCheck),SelectTag(GetSelectTag)
	{
		CImgExif::InitStdTag(StdTag);
	};
	
	~CProc(void)
	{
	};

	
public:
	/**************************************************************************
		[Function]
			Execution
		[Details]
			処理を実行する
		[Argument1] files
			ユーザーがドロップしたファイル
		[Return]
			成功 : 1  失敗 : 0  エラー : -1
	**************************************************************************/
	int Execution(const std::vector<FileInfo>& files)
	{	
		int ret = 0;
		try
		{
			ret = Execution_Except(files);
		}
		catch(...)
		{
			ret = -1;
			MessageBox(NULL,_T("処理中にエラーが発生しました"),_T("Fatal Error"),MB_OKCANCEL|MB_ICONERROR);
		}
		return ret;
	}
	
	
private:
	/*-------------------------------------------------------------------------
		[Function]
			Execution_Except()
		[Details]
			構造化例外を捕らえるラッピング関数
		[Argument1] mode
			実行モード
			( 0:空作成   1:通常作成 )
		[Return]
			成功 : 1   失敗 : 0   Fatal Error -1
	-------------------------------------------------------------------------*/
	int Execution_Except(const std::vector<FileInfo>& files)
	{
		int ret = 0;
		__try
		{
			ret = ProcExecution(files);
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
		}
		return ret;
	}


	/*-------------------------------------------------------------------------
		[Function]
			ProcExecution
		[Details]
			実際に処理を実行している関数
		[Argument1] files
			ユーザーがドロップしたファイル
		[Return]
			成功 : 1  失敗 : 0  エラー : -1
	-------------------------------------------------------------------------*/
	int ProcExecution(const std::vector<FileInfo>& files)
	{
		if(files.size() < 1) return 0;
		std::vector<ImgTag> RetTags;
		{// 全ての画像のフォルダ分け対象のタグを取得
			// ディレクトリ整合確認用
			TSTR dir = _T("");
			for(UINT i=0; i <files.size(); i++)
			{
				CImgExif Exifinfo(files[i].full);
				if(Exifinfo.GetResult() == 1)
				{
					if(dir != _T(""))
					{
						if(dir != files[i].dir)
							return 0;
					}
					else
						dir = files[i].dir;
					ImgTag buf; buf.finfo = files[i];
					GetTagValue(Exifinfo.GetData(),buf);
					RetTags.push_back(buf);
				}
			}
		}

		// 取得したタグを判定してフォルダ分け実行
		CProgressDlg pdlg(hWnd,(int)RetTags.size());
		pdlg.Show();
		std::vector<TSTR> DirTags(0,_T(""));
		//#pragma omp parallel for
		for(int i=0; i <(int)RetTags.size(); i++)
		{
			if(!i) DirTags.push_back(RetTags[i].tagvalue);
			BOOL find = FALSE;
			for(UINT j=0; j <DirTags.size(); j++)
			{// タグ名が重複した場合は同じディレクトリ内に格納
				if(DirTags[j] != RetTags[i].tagvalue) continue;
				DividFolder(RetTags[i]);
				find = TRUE;
			}
			if(!find)
			{// 新しいタグ名が出てきた場合はディレクトリ作成後、ファイルを移動orコピー
				DirTags.push_back(RetTags[i].tagvalue);
				DividFolder(RetTags[i]);
			}
			if(!pdlg.CancelClick()) pdlg.UpdateProgress(); else break;
		}
		pdlg.Close();
		return 1;
	}


	/*------------------------------------------------------------------------
		[Function]
			GetTagValue
		[Details]
			取得したタグの中からフォルダ分け対象のタグを探し当てる
		[Argument1] src
			取得した全てのタグ
		[Argument2] dst
			結果格納
	------------------------------------------------------------------------*/
	void GetTagValue(const std::vector<CImgExif::ExifTag>& src, ImgTag& dst)
	{
		for(UINT i=0; i<src.size(); i++)
		{
			BOOL find = FALSE;
			if(src[i].Content == SelectTag)
			{
				switch(src[i].Tag)
				{
				case 0x0101:
				case 0x0102:
				case 0x0104:
				case 0x0203:
					if(src[i].IFD == CImgExif::HardWareIFD){
						dst.tagid = src[i].Tag;
						dst.tagvalue = src[i].Value;
						find = TRUE;
					}
					break;
				default:
					dst.tagid = src[i].Tag;
					dst.tagvalue = src[i].Value;
					find = TRUE;
					break;
				
				}
			}
			if(find) break;
		}
	}

	
	/*------------------------------------------------------------------------
		[Function]
			DividFolder
		[Details]
			フォルダ分け実行
		[Argument1] src
			画像ファイル1つ分の情報
		[Return]
			目的のタグ値
	------------------------------------------------------------------------*/
	BOOL DividFolder(const ImgTag& src)
	{
		const TSTR mkdir = src.finfo.dir + _T("\\") + EditFolderName(src);
		// 同じフォルダが存在する場合は何も起きないため、以下の関数は評価しない
		CreateDirectory(mkdir.data(),NULL);
		const TSTR newfname = mkdir + _T("\\") + src.finfo.file;
		if(!CopyCheck){
			if(!MoveFile(src.finfo.full.data(),newfname.data()))
				return FALSE;
		}
		else{
			// 第3引数 = TRUE  同じファイル名が存在したとき上書き禁止
			if(!CopyFile(src.finfo.full.data(),newfname.data(),TRUE))
				return FALSE;
		}
		return TRUE;
	}


	/*------------------------------------------------------------------------
		[Function]
			EditFolderName
		[Details]
			タグに応じてフォルダ名を調整する
		[Argument1] src
			画像ファイル1つ分の情報
		[Return]
			フォルダ名
	------------------------------------------------------------------------*/
	TSTR EditFolderName(const ImgTag& src)
	{
		TSTR dst = src.tagvalue;
		switch(src.tagid)
		{
		// 露出時間
		case 0x829A:
			dst += _T("(S)");
			break;
		// F値
		case 0x829D:
			dst = _T("F") + dst;
			break;
		// ISO感度
		case 0x8827:
			dst = _T("ISO") + dst;
			break;
		// 焦点距離
		case 0x920A:
			dst += _T("mm");
			break;
		// 幅 高さ
		case 0xA002: case 0xA003:
			dst += _T("Pixel");
			break;
		// MC Serial.No
		case 0x0102:
			dst = _T("MC_No.") + dst;
			break;
		}
		return dst;
	}



	/**************************************************************************
		[Class]
			CProgressDlg
		[Details]
			プログレスダイアログ定義クラス
	**************************************************************************/
	class CProgressDlg : public CModelessDlg
	{
	private:
		HWND hPrg;
		BOOL Cancel;
		int PrgMax, Pos;

	public:
		CProgressDlg(const HWND& hWnd, const int& MaxCnt)
			:CModelessDlg(hWnd,IDD_PROGRESS_DIALOG),PrgMax(MaxCnt)
		{
			Cancel = FALSE;
			Pos = 0;
			// ウインドウ作成
			hDlg = DoModeless((HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE));
			hPrg = GetDlgItem(hDlg,IDC_PROGRESS1);
			//プログレスバーの設定           
			SendMessage(hPrg,PBM_SETRANGE,(WPARAM)0,MAKELPARAM(0,PrgMax));
			SendMessage(hPrg,PBM_SETPOS,0,0);
			SendMessage(hPrg,PBM_SETSTEP,1,0);
		};
		~CProgressDlg(void)
		{
		};


	/* コピー禁止 */
	private:
		CProgressDlg& operator = (const CProgressDlg& obj);


	public:
		/**************************************************************************
			[Function]
				CancelClick
			[Details]
				ウインドウのキャンセルボタン投下状況を取得する
			[Return]
				押した : TRUE  押してない : FALSE
		**************************************************************************/
		BOOL CancelClick(void)
		{
			return Cancel;
		}

		/**************************************************************************
			[Function]
				Show
			[Details]
				ウインドウを表示
		**************************************************************************/
		void Show(void)
		{
			// 表示
			ShowWindow(hDlg,SW_SHOW);
			UpdateWindow(hDlg);
		}

		/**************************************************************************
			[Function]
				Close
			[Details]
				ウインドウを閉じる
		**************************************************************************/
		void Close(void)
		{
			EndDialog(hDlg,FALSE);
		}


		/**************************************************************************
			[Function]
				UpdateProgress
			[Details]
				ウインドウへ表示する進行状況を更新する
		**************************************************************************/
		void UpdateProgress(void)
		{
			SendMessage(hPrg,PBM_STEPIT,0,0);
			Pos++;
			const int percent = (Pos*100)/PrgMax;
			TSTR title = _T("処理中 ");
			title += LToStr((long)percent);
			title += _T(" ％");
			SetWindowText(hDlg,title.data());
			UpdateWindow(hDlg);
		}


	/* Over Ride */
	protected:
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
			hHook = SetWindowsHookEx(WH_GETMESSAGE,GetHookProc,NULL,GetCurrentThreadId());
			return TRUE;
		}

	
		/*------------------------------------------------------------------------
			[Function]
				OnCancel
			[Details]
				キャンセルイベント処理
		------------------------------------------------------------------------*/
		virtual void OnCancel(void)
		{
			Cancel = TRUE;
		}
		
		
	}; // End CProgressDlg


}; // End CProc

