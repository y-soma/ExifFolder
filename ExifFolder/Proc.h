#pragma once

#include "resource.h"
#include "ImgExif.h"
#include "Dialog.h"
#include <omp.h>


/**************************************************************************
	[Class]
		CProc
	[Details]
		���C��������`�N���X
**************************************************************************/
class CProc
{
private:
	/**************************************************************************
		[Structure]
			FileTag
		[Details]
			�摜�t�@�C��1���̃^�O�����Ǘ�����\����
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
			���������s����
		[Argument1] files
			���[�U�[���h���b�v�����t�@�C��
		[Return]
			���� : 1  ���s : 0  �G���[ : -1
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
			MessageBox(NULL,_T("�������ɃG���[���������܂���"),_T("Fatal Error"),MB_OKCANCEL|MB_ICONERROR);
		}
		return ret;
	}
	
	
private:
	/*-------------------------------------------------------------------------
		[Function]
			Execution_Except()
		[Details]
			�\������O��߂炦�郉�b�s���O�֐�
		[Argument1] mode
			���s���[�h
			( 0:��쐬   1:�ʏ�쐬 )
		[Return]
			���� : 1   ���s : 0   Fatal Error -1
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
			���ۂɏ��������s���Ă���֐�
		[Argument1] files
			���[�U�[���h���b�v�����t�@�C��
		[Return]
			���� : 1  ���s : 0  �G���[ : -1
	-------------------------------------------------------------------------*/
	int ProcExecution(const std::vector<FileInfo>& files)
	{
		if(files.size() < 1) return 0;
		std::vector<ImgTag> RetTags;
		{// �S�Ẳ摜�̃t�H���_�����Ώۂ̃^�O���擾
			// �f�B���N�g�������m�F�p
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

		// �擾�����^�O�𔻒肵�ăt�H���_�������s
		CProgressDlg pdlg(hWnd,(int)RetTags.size());
		pdlg.Show();
		std::vector<TSTR> DirTags(0,_T(""));
		//#pragma omp parallel for
		for(int i=0; i <(int)RetTags.size(); i++)
		{
			if(!i) DirTags.push_back(RetTags[i].tagvalue);
			BOOL find = FALSE;
			for(UINT j=0; j <DirTags.size(); j++)
			{// �^�O�����d�������ꍇ�͓����f�B���N�g�����Ɋi�[
				if(DirTags[j] != RetTags[i].tagvalue) continue;
				DividFolder(RetTags[i]);
				find = TRUE;
			}
			if(!find)
			{// �V�����^�O�����o�Ă����ꍇ�̓f�B���N�g���쐬��A�t�@�C�����ړ�or�R�s�[
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
			�擾�����^�O�̒�����t�H���_�����Ώۂ̃^�O��T�����Ă�
		[Argument1] src
			�擾�����S�Ẵ^�O
		[Argument2] dst
			���ʊi�[
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
			�t�H���_�������s
		[Argument1] src
			�摜�t�@�C��1���̏��
		[Return]
			�ړI�̃^�O�l
	------------------------------------------------------------------------*/
	BOOL DividFolder(const ImgTag& src)
	{
		const TSTR mkdir = src.finfo.dir + _T("\\") + EditFolderName(src);
		// �����t�H���_�����݂���ꍇ�͉����N���Ȃ����߁A�ȉ��̊֐��͕]�����Ȃ�
		CreateDirectory(mkdir.data(),NULL);
		const TSTR newfname = mkdir + _T("\\") + src.finfo.file;
		if(!CopyCheck){
			if(!MoveFile(src.finfo.full.data(),newfname.data()))
				return FALSE;
		}
		else{
			// ��3���� = TRUE  �����t�@�C���������݂����Ƃ��㏑���֎~
			if(!CopyFile(src.finfo.full.data(),newfname.data(),TRUE))
				return FALSE;
		}
		return TRUE;
	}


	/*------------------------------------------------------------------------
		[Function]
			EditFolderName
		[Details]
			�^�O�ɉ����ăt�H���_���𒲐�����
		[Argument1] src
			�摜�t�@�C��1���̏��
		[Return]
			�t�H���_��
	------------------------------------------------------------------------*/
	TSTR EditFolderName(const ImgTag& src)
	{
		TSTR dst = src.tagvalue;
		switch(src.tagid)
		{
		// �I�o����
		case 0x829A:
			dst += _T("(S)");
			break;
		// F�l
		case 0x829D:
			dst = _T("F") + dst;
			break;
		// ISO���x
		case 0x8827:
			dst = _T("ISO") + dst;
			break;
		// �œ_����
		case 0x920A:
			dst += _T("mm");
			break;
		// �� ����
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
			�v���O���X�_�C�A���O��`�N���X
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
			// �E�C���h�E�쐬
			hDlg = DoModeless((HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE));
			hPrg = GetDlgItem(hDlg,IDC_PROGRESS1);
			//�v���O���X�o�[�̐ݒ�           
			SendMessage(hPrg,PBM_SETRANGE,(WPARAM)0,MAKELPARAM(0,PrgMax));
			SendMessage(hPrg,PBM_SETPOS,0,0);
			SendMessage(hPrg,PBM_SETSTEP,1,0);
		};
		~CProgressDlg(void)
		{
		};


	/* �R�s�[�֎~ */
	private:
		CProgressDlg& operator = (const CProgressDlg& obj);


	public:
		/**************************************************************************
			[Function]
				CancelClick
			[Details]
				�E�C���h�E�̃L�����Z���{�^�������󋵂��擾����
			[Return]
				������ : TRUE  �����ĂȂ� : FALSE
		**************************************************************************/
		BOOL CancelClick(void)
		{
			return Cancel;
		}

		/**************************************************************************
			[Function]
				Show
			[Details]
				�E�C���h�E��\��
		**************************************************************************/
		void Show(void)
		{
			// �\��
			ShowWindow(hDlg,SW_SHOW);
			UpdateWindow(hDlg);
		}

		/**************************************************************************
			[Function]
				Close
			[Details]
				�E�C���h�E�����
		**************************************************************************/
		void Close(void)
		{
			EndDialog(hDlg,FALSE);
		}


		/**************************************************************************
			[Function]
				UpdateProgress
			[Details]
				�E�C���h�E�֕\������i�s�󋵂��X�V����
		**************************************************************************/
		void UpdateProgress(void)
		{
			SendMessage(hPrg,PBM_STEPIT,0,0);
			Pos++;
			const int percent = (Pos*100)/PrgMax;
			TSTR title = _T("������ ");
			title += LToStr((long)percent);
			title += _T(" ��");
			SetWindowText(hDlg,title.data());
			UpdateWindow(hDlg);
		}


	/* Over Ride */
	protected:
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
			hHook = SetWindowsHookEx(WH_GETMESSAGE,GetHookProc,NULL,GetCurrentThreadId());
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
			Cancel = TRUE;
		}
		
		
	}; // End CProgressDlg


}; // End CProc

