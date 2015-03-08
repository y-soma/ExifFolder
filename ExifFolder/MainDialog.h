#pragma once

// CMainDialog : ��`

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
		���C���_�C�A���O
**************************************************************************/
class CMainDialog : public CModalDlg
{

/* Constructor / Destructor */
public:
	CMainDialog(void)
		:CModalDlg(IDD_MAIN_DIALOG)
	{
		// ���P�[������{��ɐݒ�
		_tsetlocale(LC_ALL, _T("Japanese"));
	};
	~CMainDialog(void)
	{
	};

/* Member */
protected:
	WNDPROC DefStaticProc;
private:
	// GUI�R���g���[���̃E�C���h�E�n���h��
	HWND hCombo, hCheck;


/* Private Member Function */
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
		TSTR title = _T("Exif�^�O�t�H���_����  Version ");
		title += THIS_VERSION;
		SetWindowText(hDlg,title.data());
		// �����̃R���g���[������n���h�����擾
		hCombo = GetDlgItem(hWnd,IDC_TAGLIST_COMBO);
		hCheck = GetDlgItem(hWnd,IDC_COPY_CHECK);
		// �R���{�{�b�N�X������
		return InitTaglistCombo();
	}


	/*------------------------------------------------------------------------
		[Function]
			InitTaglistCombo
		[Details]
			�R���{�{�b�N�X������
		[Argument1] hDlg
			�E�C���h�E�n���h��
		[Return]
			�����FTRUE  ���s�FFALSE
	------------------------------------------------------------------------*/
	BOOL InitTaglistCombo(void)
	{
		std::map<ULONG,TSTR> StdTag;
		CImgExif::InitStdTag(StdTag);
		const ULONG TagID[] =
		{// �����őI��Ώۂɂ���^�OID��C�ӂɓo�^����
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
			MessageBox(NULL,_T("""tagsetJ.xml""�̓ǂݍ��݂Ɏ��s���܂���"),_T("Error"), MB_OK | MB_ICONERROR);
			return FALSE;
		}
		/*{// xml����擾�������ōX�V����
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
		// ��Ԗڂ̃A�C�e����I��
		SendMessage(hCombo, CB_SETCURSEL,0,0);
		return TRUE;
	}


	/*------------------------------------------------------------------------
		[Function]
			Execution
		[Details]
			���������s����
		[Argument1] files
			���[�U�[���h���b�v�����t�@�C��
		[Return]
			���� : 1  ���s : 0  �G���[ : -1
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
			�t�@�C���h���b�O���h���b�v�C�x���g����
		[Return]
			FALSE��Ԃ�
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
			�h���b�O���h���b�v�t�@�C���ǂݍ���
		[Argument1] hWnd
			�E�C���h�E�n���h��
		[Argument2] wParam
			�p�����[�^
		[Return]
			���� : 1  ���s : 0  �G���[ : -1
	------------------------------------------------------------------------*/
	int ReadDropfile(std::vector<FileInfo>& dst,const WPARAM wParam)
	{
		try
		{
			TCHAR FileName[MAX_PATH * 0xFF] = _T("");
			HDROP hDrop = (HDROP)wParam;

			int ic = 0;
			{// �h���b�v�t�@�C�������擾
				ic = DragQueryFile(hDrop,0xFFFFFFFF,FileName,256);
				if(ic < 1){
					MessageBox(NULL,_T("�t�@�C�����h���b�v�ł��܂���ł���"),_T("Error"), MB_OK | MB_ICONERROR);
					return 0;
				}
			}
			
			{// �擾
				POINT pDrop;
				DragQueryPoint(hDrop,&pDrop);
				for(int i=0; i < ic; i++)
				{// �t�@�C������
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
			MessageBox(NULL,_T("�t�@�C�����h���b�v���ɃG���[���������܂���"),_T("Error"), MB_OK | MB_ICONERROR);
			return -1;
		}
	}

	
	/*------------------------------------------------------------------------
		[Function]
			SortFileData
		[Details]
			�t�@�C�����������Ƀ\�[�g����
		[Argument1] src
			�\�[�g����f�[�^�ւ̎Q��
		[Remarks]
			�\���̂�vector�z���std::sort�̑ΏۊO�������̂Ń����o�ɕ������ă\�[�g����
	------------------------------------------------------------------------*/
	void SortFileData(std::vector<FileInfo>& src)
	{
		std::vector<FileInfo> Buf;
		{// �\�[�g�f�[�^���擾
			std::vector<TSTR> sorttmp(0,_T(""));
			{// ��r�p�̍\���̂̃����o�������o���ă\�[�g
				for(UINT i=0; i<src.size(); i++){
					sorttmp.push_back(src[i].full);
				}
				std::sort(sorttmp.begin(), sorttmp.end());
			}
			for(UINT i=0; i<sorttmp.size(); i++)
			{// ��L�Ń\�[�g���ꂽ�f�[�^�𗘗p���Ė{�\�[�g����
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
		// �R�s�[
		src = Buf;
	}

}; // CMainDialog

