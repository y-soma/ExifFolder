#pragma once

// CBaseDialog : 定義

#include "Tools.h"
#include <algorithm>
#include <shlobj.h>

// disable
#pragma warning(disable:4311)
#pragma warning(disable:4312)
#pragma warning(disable:4244)


/**************************************************************************
	[Class]
		CBaseDlg
	[Details]
		ダイアログ基底クラス
**************************************************************************/
class CBaseDlg
{

/* Constructor / Destructor ※継承なしでは作れない */
protected:
	CBaseDlg(const int& GetID)
		:DlgID(GetID),hDlg(NULL)
	{
	};
	virtual ~CBaseDlg(void)
	{
	};

/* コピー禁止 */
private:
	CBaseDlg(CBaseDlg& obj){};
	CBaseDlg& operator = (const CBaseDlg& obj);

/* Member */
protected:
	// ウインドウハンドル
	HWND hDlg;
	// リソースID
	int DlgID;
	// フックハンドル
	HHOOK hHook;

	
/* Protected Member Function */
protected:
	/*------------------------------------------------------------------------
		[Function]
			MsgProc
		[Details]
			ダイアログボックスのコールバック関数
		[Argument1] hDlg
			ダイアログのウインドウハンドル
		[Argument2] uMsg
			取得メッセージ
		[Argument3] wParam
			メッセージの最初のパラメータ
		[Argument4] lParam
			メッセージの2番目のパラメータ
		[Return]
			メッセージ処理の結果が返ります。
			戻り値の意味は、送信されたメッセージによって異なります。
			初期化時を除き、Falseが返ります
	------------------------------------------------------------------------*/
	LRESULT CALLBACK MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch(uMsg)
		{
			case WM_INITDIALOG:
			{
				InitCommonControls();
				MoveCenter(hWnd);
				return HANDLE_WM_INITDIALOG(hWnd,wParam,lParam,OnInitDialog);
			}
			case WM_KEYDOWN:
				switch(LOWORD(wParam))
				{
				case VK_ESCAPE:
					if(MessageBox(NULL,_T("終了しますか？"),_T("Escキーが押されました"),MB_OKCANCEL|MB_ICONQUESTION) == IDOK)
					{
						PostQuitMessage(0);
						return FALSE;
					}
					break;
				}
				break;
			case WM_COMMAND:
				return CommandProc(wParam);
			case WM_DROPFILES:
				OnDropFile(wParam);
				break;
			case WM_CLOSE:
				EndDialog(hWnd,FALSE);
				return FALSE;
			default:
				break;
		}
		return FALSE;
	}


	/*------------------------------------------------------------------------
		[Function]
			GetHookProc
		[Details]
			メッセージを横取りするフックプロシージャ
		[Argument2] nCode
			コード
		[Argument3] wParam
			メッセージの最初のパラメータ
		[Argument4] lParam
			メッセージの2番目のパラメータ
		[Return]
			メッセージ処理の結果が返ります。
			戻り値の意味は、送信されたメッセージによって異なります。
			初期化時を除き、Falseが返ります
	------------------------------------------------------------------------*/
	static LRESULT FAR PASCAL GetHookProc(int nCode, WPARAM wParam, LPARAM lParam)
	{
		//SetClassLong(hDlg,GCW_ATOM,0xAFF);
		CBaseDlg* ptr = reinterpret_cast<CBaseDlg*>(lParam);
		if(!ptr) return -1;
		LPMSG lpMsg = (LPMSG)lParam;
		if(nCode >= 0 && PM_REMOVE == wParam)
		{
			// Don't translate non-input events.
			if((lpMsg->message >= WM_KEYFIRST && lpMsg->message <= WM_KEYLAST))
			{
				if(IsDialogMessage(ptr->hDlg,lpMsg))
				{
					// The value returned from this hookproc is ignored, 
					// and it cannot be used to tell Windows the message has been handled.
					// To avoid further processing, convert the message to WM_NULL 
					// before returning.
					lpMsg->message = WM_NULL;
					lpMsg->lParam  = 0;
					lpMsg->wParam  = 0;
				}
			}
		}
		return CallNextHookEx(ptr->hHook,nCode,wParam,lParam);
	}

	
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
		EndDialog(hDlg,FALSE);
	}
	
	
	/*------------------------------------------------------------------------
		[Function]
			OnDropFile
		[Details]
			ファイルドラッグ＆ドロップイベント処理
	------------------------------------------------------------------------*/
	virtual void OnDropFile(const WPARAM wParam)
	{
	}


	/*------------------------------------------------------------------------
		[Function]
			MoveCenter
		[Details]
			ダイアログを画面の中心に移動させる : 親がないので何もしないと左上に配置される
		[Argument1] hwnd
			ウインドウハンドル
	------------------------------------------------------------------------*/
	virtual void MoveCenter(const HWND& hWnd)
	{
		int w = 0, h = 0, wpos = 0, hpos = 0;
		{// ダイアログの縦横サイズ
			RECT rc;
			GetWindowRect(hWnd, &rc);
			w = rc.right - rc.left;
			h = rc.bottom - rc.top;
		}
		{// スクリーンサイズから計算
			const int wFull = GetSystemMetrics(SM_CXSCREEN);
			const int hFull = GetSystemMetrics(SM_CYSCREEN);
			wpos = static_cast<int>((wFull-w)*0.5);
			hpos = static_cast<int>((hFull-h)*0.5);
		}
		MoveWindow(hWnd,wpos,hpos,w,h,FALSE);
	}


	/*------------------------------------------------------------------------
		[Function]
			CommandProc
		[Details]
			コマンド系メッセージ処理
		[Argument1] wParam
			コマンド系メッセージ
		[Return]
			メッセージ処理の結果が返ります。
			戻り値の意味は、送信されたメッセージによって異なります。
	------------------------------------------------------------------------*/
	LRESULT CommandProc(const WPARAM wParam)
	{
		switch(LOWORD(wParam))
		{
		// キャンセルボタン
		case IDCANCEL:
			OnCancel();
			break;
		}
		return FALSE;
	}


	/*------------------------------------------------------------------------
		[Function]
			GetWindowTxtEx
		[Details]
			コントロールに表示されているテキストを取得する
		[Argument1] gWnd
			指定ウインドウハンドル
		[Return]
			表示されているテキストが文字列で返る
	------------------------------------------------------------------------*/
	TSTR GetWindowTxtEx(const HWND& hWnd)
	{
		TSTR dst = _T("");
		{
			TCHAR str[MAX_PATH*0xFF];
			GetWindowText(hWnd,str,MAX_PATH*0xFF);
			dst = str;
		}
		return dst;
	}


}; // End CBaseDlg




/**************************************************************************
	[Class]
		CModalDlg
	[Details]
		モーダルダイアログクラス
**************************************************************************/
class CModalDlg : public CBaseDlg
{

/* Constructor / Destructor ※継承なしでは作れない */
protected:
	CModalDlg(const int& GetID)
		:CBaseDlg(GetID)
	{
	};
	virtual ~CModalDlg(void)
	{
	};

/* コピー禁止 */
private:
	CModalDlg& operator = (const CModalDlg& obj);


/* Public Member Function */
public:
	/**************************************************************************
		[Function]
			DoModal
		[Details]
			モーダルダイアログ作成
		[Argument1] hInstance
			インスタンスハンドル
		[Return]
			Falseを返します
	**************************************************************************/
	int DoModal(const HINSTANCE hInstance)
	{
		// LPARAMへthisポインタを関連付けするため、DialogBoxParamを使用 ( ※最後の引数へthisポインタを指定する )
		return (int)DialogBoxParam(hInstance,MAKEINTRESOURCE(DlgID),(HWND)NULL,(DLGPROC)DlgProc,reinterpret_cast<LPARAM>(this));
	}


/* CALLBACK Function */
private:
	/*------------------------------------------------------------------------
		[Function]
			DlgProc
		[Details]
			ダイアログボックスのコールバック関数
		[Argument1] hDlg
			ダイアログのウインドウハンドル
		[Argument2] uMsg
			取得メッセージ
		[Argument3] wParam
			メッセージの最初のパラメータ
		[Argument4] lParam
			メッセージの2番目のパラメータ
		[Return]
			メッセージ処理の結果が返ります。
			戻り値の意味は、送信されたメッセージによって異なります。
			初期化時を除き、Falseが返ります
	------------------------------------------------------------------------*/
	static LRESULT CALLBACK DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		static const TCHAR PropName[] = _T("ModalDlgProp");
		switch(uMsg)
		{
			case WM_INITDIALOG:
			{// 初期化
				// 既存のコントロールからthisポインタ取得
				CModalDlg* ptr = reinterpret_cast<CModalDlg*>(lParam);
				if(!ptr) return -1;
				// プロパティにWindowクラスのポインタを設定する
				if(!::SetProp(hWnd, PropName,reinterpret_cast<HANDLE>(ptr))) return -1;
				// ウィンドウハンドルセット
				ptr->hDlg = hWnd;
				// メッセージ処理関数コール
				return ptr->MsgProc(hWnd,uMsg,wParam,lParam);
			}
			case WM_CLOSE:
			{// 終了
				CModalDlg* ptr = reinterpret_cast<CModalDlg*>(::GetProp(hWnd,PropName));
				LRESULT ret = 0;
				// メッセージ処理関数コール
				if(ptr != NULL){
					ret = ptr->MsgProc(hWnd,uMsg,wParam,lParam);
					ptr->hDlg = NULL;
				}
				else{
					ret = ::DefWindowProc(hWnd,uMsg,wParam,lParam);
				}
				// 設定したプロパティのデータを削除する
				::RemoveProp(hWnd, PropName);
				return ret;
			}
			default:
			{// その他
				CModalDlg* ptr = reinterpret_cast<CModalDlg*>(::GetProp(hWnd,PropName));
				if(ptr != NULL)
					return ptr->MsgProc(hWnd,uMsg,wParam,lParam);
				else
					return ::DefWindowProc(hWnd,uMsg,wParam,lParam);
			}
		}
		return FALSE;
	}

}; // End CModalDlg




/**************************************************************************
	[Class]
		CModelessDlg
	[Details]
		モードレスダイアログクラス
**************************************************************************/
class CModelessDlg : public CBaseDlg
{

/* Constructor / Destructor ※継承なしでは作れない */
protected:
	CModelessDlg(const HWND& hWnd, const int& GetID)
		:CBaseDlg(GetID),OwnerWnd(hWnd)
	{
	};
	virtual ~CModelessDlg(void)
	{
	};

/* コピー禁止 */
private:
	CModelessDlg& operator = (const CModelessDlg& obj);

protected:
	// オーナーウインドウハンドル
	HWND OwnerWnd;

/* Public Member Function */
public:
	/**************************************************************************
		[Function]
			DoModeless
		[Details]
			モードレスダイアログ作成
		[Argument1] hInstance
			インスタンスハンドル
		[Return]
			ウインドウハンドル
	**************************************************************************/
	HWND DoModeless(const HINSTANCE hInstance)
	{
		return CreateDialogParam(hInstance,MAKEINTRESOURCE(DlgID),OwnerWnd,(DLGPROC)DlgProc,reinterpret_cast<LPARAM>(this));
	}


/* CALLBACK Function */
private:
	/*------------------------------------------------------------------------
		[Function]
			DlgProc
		[Details]
			ダイアログボックスのコールバック関数
		[Argument1] hDlg
			ダイアログのウインドウハンドル
		[Argument2] uMsg
			取得メッセージ
		[Argument3] wParam
			メッセージの最初のパラメータ
		[Argument4] lParam
			メッセージの2番目のパラメータ
		[Return]
			メッセージ処理の結果が返ります。
			戻り値の意味は、送信されたメッセージによって異なります。
			初期化時を除き、Falseが返ります
	------------------------------------------------------------------------*/
	static LRESULT CALLBACK DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		static const TCHAR PropName[] = _T("ModelessDlgProp");
		switch(uMsg)
		{
			case WM_INITDIALOG:
			{// 初期化
				// 既存のコントロールからthisポインタ取得
				CModelessDlg* ptr = reinterpret_cast<CModelessDlg*>(lParam);
				if(!ptr) return -1;
				// プロパティにWindowクラスのポインタを設定する
				if(!::SetProp(hWnd, PropName,reinterpret_cast<HANDLE>(ptr))) return -1;
				// ウィンドウハンドルセット
				ptr->hDlg = hWnd;
				// メッセージ処理関数コール
				return ptr->MsgProc(hWnd,uMsg,wParam,lParam);
			}
			case WM_CLOSE:
			{// 終了
				CModelessDlg* ptr = reinterpret_cast<CModelessDlg*>(::GetProp(hWnd,PropName));
				LRESULT ret = 0;
				// メッセージ処理関数コール
				if(ptr != NULL){
					ret = ptr->MsgProc(hWnd,uMsg,wParam,lParam);
					ptr->hDlg = NULL;
				}
				else{
					ret = ::DefWindowProc(hWnd,uMsg,wParam,lParam);
				}
				// 設定したプロパティのデータを削除する
				::RemoveProp(hWnd, PropName);
				return ret;
			}
			default:
			{// その他
				CModelessDlg* ptr = reinterpret_cast<CModelessDlg*>(::GetProp(hWnd,PropName));
				if(ptr != NULL)
					return ptr->MsgProc(hWnd,uMsg,wParam,lParam);
				else
					return ::DefWindowProc(hWnd,uMsg,wParam,lParam);
			}
		}
		return FALSE;
	}


protected:
	/*------------------------------------------------------------------------
		[Function]
			MoveCenter
		[Details]
			ダイアログをオーナーウインドウの中心に移動させる
		[Argument1] hwnd
			ウインドウハンドル
	------------------------------------------------------------------------*/
	virtual void MoveCenter(const HWND& hWnd)
	{
		if(!OwnerWnd) return CBaseDlg::MoveCenter(hWnd);	
		int w = 0, h = 0, wpos = 0, hpos = 0;
		{// ダイアログの縦横サイズ
			RECT rc;
			GetWindowRect(hWnd,&rc);
			w = rc.right - rc.left;
			h = rc.bottom - rc.top;
		}
		{// オーナーウインドウの中央の座標から計算
			RECT rc;
			GetWindowRect(OwnerWnd,&rc);
			const int oc_wpos = rc.left + ((rc.right-rc.left)/2);
			const int oc_hpos = rc.top + ((rc.bottom-rc.top)/2);
			const int wpos_t = oc_wpos - (w/2);
			const int hpos_t = oc_hpos - (h/2);
			wpos = wpos_t<0? 0:wpos_t;
			hpos = hpos_t<0? 0:hpos_t;
		}
		MoveWindow(hWnd,wpos,hpos,w,h,FALSE);
	}


}; // End CModalDlg


