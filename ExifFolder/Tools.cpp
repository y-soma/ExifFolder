#include "Tools.h"


// Tools �����t�@�C��


/**************************************************************************
	[Function]
		GetFileName()
	[Details]
		�t���p�X����t�@�C���������o��
	[Argument1] fullPath
		�t���p�X�ւ̃|�C���^
	[Return]
		���� : �t�@�C����	���s : �󕶎�
**************************************************************************/
TSTR GetFileName(const TCHAR* const fullPath)
{ 
	
	TSTR dst = _T("");
	{
		const TSTR fpath = fullPath;
		int find = 0;	
		if((find=(int)fpath.rfind('\\')) > 0 )
		{ 
			for(UINT i=find+1; i<fpath.length(); i++)
			{
				dst += fpath.substr(i,1);
			}
		}
	}
	return dst;
}


/**************************************************************************
	[Function]
		GetDirName()
	[Details]
		�t���p�X����f�B���N�g���������o��
	[Argument1] fullPath
		�t���p�X�ւ̃|�C���^
	[Return]
		���� : �f�B���N�g����	���s : �󕶎�
**************************************************************************/
TSTR GetDirName(const TCHAR* const fullPath)
{ 
	TSTR dst = _T("");
	{
		const TSTR fpath = fullPath;
		int find = 0;	
		if((find=(int)fpath.rfind('\\')) > 0 )
		{ 
			for(int i=0; i<find; i++)
			{
				dst += fpath.substr(i,1);
			}
		}
	}
	return dst;
}


/**************************************************************************
	[Function]
		GetModuleDir
	[Details]
		���s�Ώۃt�H���_�ւ̃p�X���擾����
	[Return]
		���s�Ώۃt�H���_�ւ̃p�X(��"C:\\DATA\\TEMP"�̂悤�Ɏ擾�����t�@�C���̍Ō����"\\"�͕t���Ȃ�)
**************************************************************************/
TSTR GetModuleDir(void)
{
	TCHAR str[MAX_PATH];
	GetModuleFileName(NULL,str,MAX_PATH);
	return GetDirName(str);
}


/**************************************************************************
	[Function]
		LToStr()
	[Details]
		���l�𕶎���ɕϊ�����
	[Argument1] val
		�ϊ��Ώ�
	[Return]
		�ϊ���̕�����
**************************************************************************/
TSTR LToStr(const long& val)
{ 
	TCHAR buf[MAX_PATH*0xFF] = {0};
	wsprintf(buf,_T("%d"),val);
	return buf;
}

/**************************************************************************
	[Function]
		LToStr()
	[Details]
		���l�𕶎���ɕϊ�����
	[Argument1] val
		�ϊ��Ώ�
	[Return]
		�ϊ���̕�����
**************************************************************************/
TSTR LToStr(const ULONG& val)
{ 
	TCHAR buf[MAX_PATH*0xFF] = {0};
	wsprintf(buf,_T("%d"),val);
	return buf;
}


/**************************************************************************
	[Function]
		DToStr()
	[Details]
		�����𕶎���ɕϊ�����
	[Argument1] val
		�ϊ��Ώ�
	[Argument2] precision
		�����_�ȉ��̐��x(�����_��[precision]��)
	[Return]
		�ϊ���̕�����
**************************************************************************/
TSTR DToStr(const double& val, const int& precision/*= -1*/)
{ 
	TCHAR buf[MAX_PATH] = {0};
	if(precision == -1)
	{// �����_�w�薳��
		_stprintf_s(buf, _T("%f"), val);
	}
	else if(precision > -1)
	{
		const TSTR pre = _T("%.")+LToStr((long)precision)+_T("f");
		_stprintf_s(buf, pre.data(), val);
	}
	return buf;
}


/**************************************************************************
	[Function]
		StrToD()
	[Details]
		������̐��l�������^�ɕϊ�����
	[Argument1] str
		�ϊ��Ώ�
	[Return]
		�ϊ���̕�������
**************************************************************************/
double StrToD(const TCHAR* const str)
{ 
	#ifndef UNICODE
		return atof(str);
	#else
		return _wtof(str);
	#endif
}


/**************************************************************************
	[Function]
		StrToL()
	[Details]
		������̐��l�𐮐��^�ɕϊ�����
	[Argument1] str
		�ϊ��Ώ�
	[Return]
		�ϊ�����
**************************************************************************/
long StrToL(const TCHAR* const str)
{ 
	#ifndef UNICODE
		return atol(str);
	#else
		return _wtol(str);
	#endif
}


/*********************************************************************
	[�֐�]
		CStringW CTools::ToUnicode(const char* const str)
	[����]
		�������Unicode�֕ϊ����� 

	[����1] str
		�ϊ����镶����

	[�߂�l]
		Unicode������
*********************************************************************/
std::basic_string<wchar_t> ToUnicode(const char* const str)
{
	try
	{
		// MBCS��Unicode�ϊ�
		const int len = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
		std::vector<wchar_t> buffer(len + 2, 0);
		if( MultiByteToWideChar(CP_ACP, 0, str, -1, &buffer[0], static_cast<int>(buffer.size())) != len )
		{
			return L"";
		}
		buffer[len] = L'\0';
		return &buffer[0];
	}
	catch(...)
	{
		return L"";
	}
}
//CStringW CTools::ToUnicode(const CStringA& str)
//{
//	return ToUnicode(static_cast<const char*>(str));
//}


/*********************************************************************
	[�֐�]
		CStringA CTools::ToMBCS(const wchar_t* const str)
	[����]
		�������MBCS�֕ϊ����� 

	[����1] str
		�ϊ����镶����

	[�߂�l]
		MBCS������
*********************************************************************/
std::basic_string<char> ToMBCS(const wchar_t* const str)
{
	try
	{
		// Unicode->MBCS�ϊ�
		const int len = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
		std::vector<char>buffer(len + 2, 0);
		if( WideCharToMultiByte(CP_ACP, 0, str, -1, &buffer[0], static_cast<int>(buffer.size()), NULL, NULL) != len )
			return false;
		buffer[len] = '\0';
		return &buffer[0];
	}
	catch(...)
	{
		return "";
	}
}
//CStringA CTools::ToMBCS(const CStringW& str)
//{
//	return ToMBCS(static_cast<const wchar_t*>(str));
//}


/**************************************************************************
	[Function]
		Split()
	[Details]
		�w�肵����������f���~�^�ŕ�������
	[Argument1] str
		����������
	[Argument2] delim
		�f���~�^
	[Return]
		��������ƕ������ꂽ�����񂪔z��ŕԂ�
**************************************************************************/
std::vector<TSTR> Split(const TCHAR* const str, const TCHAR* const delim)
{
	std::vector<TSTR> items(0,_T(""));
	TSTR src = str;
	std::size_t dlm_idx = -1;
	if(src.npos == (dlm_idx = src.find_first_of(delim)))
		items.push_back(src.substr(0,dlm_idx));
	while(src.npos != (dlm_idx = src.find_first_of(delim)))
	{
		if(src.npos == src.find_first_not_of(delim)) break;
		items.push_back(src.substr(0,dlm_idx));
		dlm_idx++;
		src = src.erase(0,dlm_idx);
		if(src.npos == src.find_first_of(delim) && _T("") != src.data())
		{
			items.push_back(src);
			break;
		}
	}
	return items;
}


/**************************************************************************
	[Function]
		RemoveNull()
	[Details]
		�����񂩂�NULL��������菜��
	[Argument1] str
		�Ώ�
	[Return]
		���O����
**************************************************************************/
TSTR RemoveNull(const TSTR& str)
{
	TSTR dst = _T("");
	for(UINT i=0; i<str.length(); i++)
	{
		if(str.at(i) == '\0') break;
		dst += str.substr(i,1);
	}

	// ���̋󔒂�.����菜��
	int length = 0;
	for(int i=(int)dst.length()-1; i>-1; i--)
	{
		if(dst.at(i) != ' ' && dst.at(i) != '.'){
			length = i;
			break;
		}
	}
	dst = dst.substr(0,length+1);
	return dst;
}


/**************************************************************************
	[Function]
		RoundEx
	[Details]
		double�^�̐��l���w�肵�����x�Ŋۂ߂�
	[Argument1] src
		�l
	[Argument2] precision
		�܂�ߐ��x(�����_��[precision]��)
	[Return]
		�ۂߌ���
**************************************************************************/
double RoundEx(const double& src, const int& precision)
{
	const double adj = 0.5 * pow(10.0,(double)(precision*-1));
	const double mag = pow(10.0,(double)precision);
	double dst = src + adj;
	dst *= mag;
	return floor(dst)/mag;
}


/**************************************************************************
	[Function]
		CheckPrecision
	[Details]
		double�^�̐��x���`�F�b�N����(�����扽�ʂ܂Œl�����݂��邩)
	[Argument1] src
		�l
	[Return]
		������n�ʂ��𐮐��l�ŕԂ�
**************************************************************************/
int CheckPrecision(const double& src)
{
	int dst = 0;
	{
		while(1)
		{
			const long ltmp = (long)(src*pow(10.0,(double)(dst+1)));
			// �����ő�����ɏ����_�ȉ����ۂ߂��Ă��܂��̂ŏ�肭�����Ȃ�
			const double srctmp = src*pow(10.0,(double)(dst+1));
			const double check = srctmp-(double)ltmp;
			if(check == 0)
				break;
			else if(check > 0)
				dst++;
			if(dst > 50) break;
		}
	}
	return dst;
}


#pragma warning (disable:4996)
/**************************************************************************
	[Function]
		PathIsDirectoryEX()
	[Details]
		����^�f�B���N�g���̑��݊m�F
	[Argument1] path
		�f�B���N�g��
	[Return]
		���� : TRUE    ������Ȃ� : FALSE
**************************************************************************/
BOOL PathIsDirectoryEX(const TCHAR* const path)
{
	BOOL ret = FALSE;
	{
		const TSTR filePath = GetMakeFilePath(path);
		FILE* fp = NULL;	
	#ifndef UNICODE
		fp = fopen(filePath.data(),"w,ccs=UNICODE");
	#else
		fp = _wfopen(filePath.data(),L"w,ccs=UNICODE");
	#endif
		if(!fp) return ret;
		fclose(fp); ret = TRUE;
		if(!DeleteFile(filePath.data())) ret = FALSE;
	}
	return ret;
}


/**************************************************************************
	[Function]
		GetMakeFilePath()
	[Details]
		�w��f�B���N�g���̒�����쐬����t�@�C�����̌����������āA������Ȃ��ꍇ�݂̂��̃t�@�C������Ԃ�
	[Argument1] path
		�f�B���N�g���̂�
	[Argument2] opfname
		�쐬�t�@�C�����̐擪��
	[Return]
		���� : �����p�X    ���s : empty
**************************************************************************/
TSTR GetMakeFilePath(const TCHAR* const path, const TCHAR* const opfname/*=_T("log")*/)
{
	TSTR dst = _T("");
	{
		const TSTR Path = path;
		const TSTR OpFName = opfname;
		long i  = 1;
		while(1)
		{
			const TSTR filePathTemp = Path + _T("\\") + OpFName + LToStr(i) + _T(".txt");
			if(PathFileExists(filePathTemp.data())){
				i++; continue;
			}
			else{
				dst = filePathTemp; break;
			}
			if(i > 0x1FF) return dst;
		}
	}
	return dst;
}


