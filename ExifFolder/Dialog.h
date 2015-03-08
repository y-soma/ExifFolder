#pragma once

// CBaseDialog : ��`

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
		�_�C�A���O���N���X
**************************************************************************/
class CBaseDlg
{

/* Constructor / Destructor ���p���Ȃ��ł͍��Ȃ� */
protected:
	CBaseDlg(const int& GetID)
		:DlgID(GetID),hDlg(NULL)
	{
	};
	virtual ~CBaseDlg(void)
	{
	};

/* �R�s�[�֎~ */
private:
	CBaseDlg(CBaseDlg& obj){};
	CBaseDlg& operator = (const CBaseDlg& obj);

/* Member */
protected:
	// �E�C���h�E�n���h��
	HWND hDlg;
	// ���\�[�XID
	int DlgID;
	// �t�b�N�n���h��
	HHOOK hHook;

	
/* Protected Member Function */
protected:
	/*------------------------------------------------------------------------
		[Function]
			MsgProc
		[Details]
			�_�C�A���O�{�b�N�X�̃R�[���o�b�N�֐�
		[Argument1] hDlg
			�_�C�A���O�̃E�C���h�E�n���h��
		[Argument2] uMsg
			�擾���b�Z�[�W
		[Argument3] wParam
			���b�Z�[�W�̍ŏ��̃p�����[�^
		[Argument4] lParam
			���b�Z�[�W��2�Ԗڂ̃p�����[�^
		[Return]
			���b�Z�[�W�����̌��ʂ��Ԃ�܂��B
			�߂�l�̈Ӗ��́A���M���ꂽ���b�Z�[�W�ɂ���ĈقȂ�܂��B
			���������������AFalse���Ԃ�܂�
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
					if(MessageBox(NULL,_T("�I�����܂����H"),_T("Esc�L�[��������܂���"),MB_OKCANCEL|MB_ICONQUESTION) == IDOK)
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
			���b�Z�[�W������肷��t�b�N�v���V�[�W��
		[Argument2] nCode
			�R�[�h
		[Argument3] wParam
			���b�Z�[�W�̍ŏ��̃p�����[�^
		[Argument4] lParam
			���b�Z�[�W��2�Ԗڂ̃p�����[�^
		[Return]
			���b�Z�[�W�����̌��ʂ��Ԃ�܂��B
			�߂�l�̈Ӗ��́A���M���ꂽ���b�Z�[�W�ɂ���ĈقȂ�܂��B
			���������������AFalse���Ԃ�܂�
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
			�_�C�A���O������
		[Return]
			TRUE��Ԃ�
	------------------------------------------------------------------------*/
	virtual BOOL OnInitDialog(const HWND hWnd, const HWND hwndFocus, const LPARAM lParam)
	{
		return TRUE;
	}

	
	/*------------------------------------------------------------------------
		[Function]
			OnCancel
		[Details]
			�L�����Z���C�x���g����
	------------------------------------------------------------------------*/
	virtual void OnCancel(void)
	{
		EndDialog(hDlg,FALSE);
	}
	
	
	/*------------------------------------------------------------------------
		[Function]
			OnDropFile
		[Details]
			�t�@�C���h���b�O���h���b�v�C�x���g����
	------------------------------------------------------------------------*/
	virtual void OnDropFile(const WPARAM wParam)
	{
	}


	/*------------------------------------------------------------------------
		[Function]
			MoveCenter
		[Details]
			�_�C�A���O����ʂ̒��S�Ɉړ������� : �e���Ȃ��̂ŉ������Ȃ��ƍ���ɔz�u�����
		[Argument1] hwnd
			�E�C���h�E�n���h��
	------------------------------------------------------------------------*/
	virtual void MoveCenter(const HWND& hWnd)
	{
		int w = 0, h = 0, wpos = 0, hpos = 0;
		{// �_�C�A���O�̏c���T�C�Y
			RECT rc;
			GetWindowRect(hWnd, &rc);
			w = rc.right - rc.left;
			h = rc.bottom - rc.top;
		}
		{// �X�N���[���T�C�Y����v�Z
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
			�R�}���h�n���b�Z�[�W����
		[Argument1] wParam
			�R�}���h�n���b�Z�[�W
		[Return]
			���b�Z�[�W�����̌��ʂ��Ԃ�܂��B
			�߂�l�̈Ӗ��́A���M���ꂽ���b�Z�[�W�ɂ���ĈقȂ�܂��B
	------------------------------------------------------------------------*/
	LRESULT CommandProc(const WPARAM wParam)
	{
		switch(LOWORD(wParam))
		{
		// �L�����Z���{�^��
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
			�R���g���[���ɕ\������Ă���e�L�X�g���擾����
		[Argument1] gWnd
			�w��E�C���h�E�n���h��
		[Return]
			�\������Ă���e�L�X�g��������ŕԂ�
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
		���[�_���_�C�A���O�N���X
**************************************************************************/
class CModalDlg : public CBaseDlg
{

/* Constructor / Destructor ���p���Ȃ��ł͍��Ȃ� */
protected:
	CModalDlg(const int& GetID)
		:CBaseDlg(GetID)
	{
	};
	virtual ~CModalDlg(void)
	{
	};

/* �R�s�[�֎~ */
private:
	CModalDlg& operator = (const CModalDlg& obj);


/* Public Member Function */
public:
	/**************************************************************************
		[Function]
			DoModal
		[Details]
			���[�_���_�C�A���O�쐬
		[Argument1] hInstance
			�C���X�^���X�n���h��
		[Return]
			False��Ԃ��܂�
	**************************************************************************/
	int DoModal(const HINSTANCE hInstance)
	{
		// LPARAM��this�|�C���^���֘A�t�����邽�߁ADialogBoxParam���g�p ( ���Ō�̈�����this�|�C���^���w�肷�� )
		return (int)DialogBoxParam(hInstance,MAKEINTRESOURCE(DlgID),(HWND)NULL,(DLGPROC)DlgProc,reinterpret_cast<LPARAM>(this));
	}


/* CALLBACK Function */
private:
	/*------------------------------------------------------------------------
		[Function]
			DlgProc
		[Details]
			�_�C�A���O�{�b�N�X�̃R�[���o�b�N�֐�
		[Argument1] hDlg
			�_�C�A���O�̃E�C���h�E�n���h��
		[Argument2] uMsg
			�擾���b�Z�[�W
		[Argument3] wParam
			���b�Z�[�W�̍ŏ��̃p�����[�^
		[Argument4] lParam
			���b�Z�[�W��2�Ԗڂ̃p�����[�^
		[Return]
			���b�Z�[�W�����̌��ʂ��Ԃ�܂��B
			�߂�l�̈Ӗ��́A���M���ꂽ���b�Z�[�W�ɂ���ĈقȂ�܂��B
			���������������AFalse���Ԃ�܂�
	------------------------------------------------------------------------*/
	static LRESULT CALLBACK DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		static const TCHAR PropName[] = _T("ModalDlgProp");
		switch(uMsg)
		{
			case WM_INITDIALOG:
			{// ������
				// �����̃R���g���[������this�|�C���^�擾
				CModalDlg* ptr = reinterpret_cast<CModalDlg*>(lParam);
				if(!ptr) return -1;
				// �v���p�e�B��Window�N���X�̃|�C���^��ݒ肷��
				if(!::SetProp(hWnd, PropName,reinterpret_cast<HANDLE>(ptr))) return -1;
				// �E�B���h�E�n���h���Z�b�g
				ptr->hDlg = hWnd;
				// ���b�Z�[�W�����֐��R�[��
				return ptr->MsgProc(hWnd,uMsg,wParam,lParam);
			}
			case WM_CLOSE:
			{// �I��
				CModalDlg* ptr = reinterpret_cast<CModalDlg*>(::GetProp(hWnd,PropName));
				LRESULT ret = 0;
				// ���b�Z�[�W�����֐��R�[��
				if(ptr != NULL){
					ret = ptr->MsgProc(hWnd,uMsg,wParam,lParam);
					ptr->hDlg = NULL;
				}
				else{
					ret = ::DefWindowProc(hWnd,uMsg,wParam,lParam);
				}
				// �ݒ肵���v���p�e�B�̃f�[�^���폜����
				::RemoveProp(hWnd, PropName);
				return ret;
			}
			default:
			{// ���̑�
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
		���[�h���X�_�C�A���O�N���X
**************************************************************************/
class CModelessDlg : public CBaseDlg
{

/* Constructor / Destructor ���p���Ȃ��ł͍��Ȃ� */
protected:
	CModelessDlg(const HWND& hWnd, const int& GetID)
		:CBaseDlg(GetID),OwnerWnd(hWnd)
	{
	};
	virtual ~CModelessDlg(void)
	{
	};

/* �R�s�[�֎~ */
private:
	CModelessDlg& operator = (const CModelessDlg& obj);

protected:
	// �I�[�i�[�E�C���h�E�n���h��
	HWND OwnerWnd;

/* Public Member Function */
public:
	/**************************************************************************
		[Function]
			DoModeless
		[Details]
			���[�h���X�_�C�A���O�쐬
		[Argument1] hInstance
			�C���X�^���X�n���h��
		[Return]
			�E�C���h�E�n���h��
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
			�_�C�A���O�{�b�N�X�̃R�[���o�b�N�֐�
		[Argument1] hDlg
			�_�C�A���O�̃E�C���h�E�n���h��
		[Argument2] uMsg
			�擾���b�Z�[�W
		[Argument3] wParam
			���b�Z�[�W�̍ŏ��̃p�����[�^
		[Argument4] lParam
			���b�Z�[�W��2�Ԗڂ̃p�����[�^
		[Return]
			���b�Z�[�W�����̌��ʂ��Ԃ�܂��B
			�߂�l�̈Ӗ��́A���M���ꂽ���b�Z�[�W�ɂ���ĈقȂ�܂��B
			���������������AFalse���Ԃ�܂�
	------------------------------------------------------------------------*/
	static LRESULT CALLBACK DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		static const TCHAR PropName[] = _T("ModelessDlgProp");
		switch(uMsg)
		{
			case WM_INITDIALOG:
			{// ������
				// �����̃R���g���[������this�|�C���^�擾
				CModelessDlg* ptr = reinterpret_cast<CModelessDlg*>(lParam);
				if(!ptr) return -1;
				// �v���p�e�B��Window�N���X�̃|�C���^��ݒ肷��
				if(!::SetProp(hWnd, PropName,reinterpret_cast<HANDLE>(ptr))) return -1;
				// �E�B���h�E�n���h���Z�b�g
				ptr->hDlg = hWnd;
				// ���b�Z�[�W�����֐��R�[��
				return ptr->MsgProc(hWnd,uMsg,wParam,lParam);
			}
			case WM_CLOSE:
			{// �I��
				CModelessDlg* ptr = reinterpret_cast<CModelessDlg*>(::GetProp(hWnd,PropName));
				LRESULT ret = 0;
				// ���b�Z�[�W�����֐��R�[��
				if(ptr != NULL){
					ret = ptr->MsgProc(hWnd,uMsg,wParam,lParam);
					ptr->hDlg = NULL;
				}
				else{
					ret = ::DefWindowProc(hWnd,uMsg,wParam,lParam);
				}
				// �ݒ肵���v���p�e�B�̃f�[�^���폜����
				::RemoveProp(hWnd, PropName);
				return ret;
			}
			default:
			{// ���̑�
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
			�_�C�A���O���I�[�i�[�E�C���h�E�̒��S�Ɉړ�������
		[Argument1] hwnd
			�E�C���h�E�n���h��
	------------------------------------------------------------------------*/
	virtual void MoveCenter(const HWND& hWnd)
	{
		if(!OwnerWnd) return CBaseDlg::MoveCenter(hWnd);	
		int w = 0, h = 0, wpos = 0, hpos = 0;
		{// �_�C�A���O�̏c���T�C�Y
			RECT rc;
			GetWindowRect(hWnd,&rc);
			w = rc.right - rc.left;
			h = rc.bottom - rc.top;
		}
		{// �I�[�i�[�E�C���h�E�̒����̍��W����v�Z
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


