#include "Tools.h"


// Tools 実装ファイル


/**************************************************************************
	[Function]
		GetFileName()
	[Details]
		フルパスからファイル名を取り出す
	[Argument1] fullPath
		フルパスへのポインタ
	[Return]
		成功 : ファイル名	失敗 : 空文字
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
		フルパスからディレクトリ名を取り出す
	[Argument1] fullPath
		フルパスへのポインタ
	[Return]
		成功 : ディレクトリ名	失敗 : 空文字
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
		実行対象フォルダへのパスを取得する
	[Return]
		実行対象フォルダへのパス(※"C:\\DATA\\TEMP"のように取得したファイルの最後尾に"\\"は付かない)
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
		数値を文字列に変換する
	[Argument1] val
		変換対象
	[Return]
		変換後の文字列
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
		数値を文字列に変換する
	[Argument1] val
		変換対象
	[Return]
		変換後の文字列
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
		少数を文字列に変換する
	[Argument1] val
		変換対象
	[Argument2] precision
		少数点以下の精度(小数点第[precision]位)
	[Return]
		変換後の文字列
**************************************************************************/
TSTR DToStr(const double& val, const int& precision/*= -1*/)
{ 
	TCHAR buf[MAX_PATH] = {0};
	if(precision == -1)
	{// 小数点指定無し
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
		文字列の数値を少数型に変換する
	[Argument1] str
		変換対象
	[Return]
		変換後の浮動小数
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
		文字列の数値を整数型に変換する
	[Argument1] str
		変換対象
	[Return]
		変換結果
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
	[関数]
		CStringW CTools::ToUnicode(const char* const str)
	[説明]
		文字列をUnicodeへ変換する 

	[引数1] str
		変換する文字列

	[戻り値]
		Unicode文字列
*********************************************************************/
std::basic_string<wchar_t> ToUnicode(const char* const str)
{
	try
	{
		// MBCS→Unicode変換
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
	[関数]
		CStringA CTools::ToMBCS(const wchar_t* const str)
	[説明]
		文字列をMBCSへ変換する 

	[引数1] str
		変換する文字列

	[戻り値]
		MBCS文字列
*********************************************************************/
std::basic_string<char> ToMBCS(const wchar_t* const str)
{
	try
	{
		// Unicode->MBCS変換
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
		指定した文字列をデリミタで分割する
	[Argument1] str
		分割文字列
	[Argument2] delim
		デリミタ
	[Return]
		成功すると分割された文字列が配列で返る
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
		文字列からNULL文字を取り除く
	[Argument1] str
		対象
	[Return]
		除外結果
**************************************************************************/
TSTR RemoveNull(const TSTR& str)
{
	TSTR dst = _T("");
	for(UINT i=0; i<str.length(); i++)
	{
		if(str.at(i) == '\0') break;
		dst += str.substr(i,1);
	}

	// 後ろの空白と.を取り除く
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
		double型の数値を指定した精度で丸める
	[Argument1] src
		値
	[Argument2] precision
		まるめ精度(小数点第[precision]位)
	[Return]
		丸め結果
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
		double型の精度をチェックする(少数第何位まで値が存在するか)
	[Argument1] src
		値
	[Return]
		少数第n位かを整数値で返す
**************************************************************************/
int CheckPrecision(const double& src)
{
	int dst = 0;
	{
		while(1)
		{
			const long ltmp = (long)(src*pow(10.0,(double)(dst+1)));
			// ここで代入時に小数点以下が丸められてしまうので上手くいかない
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
		自作型ディレクトリの存在確認
	[Argument1] path
		ディレクトリ
	[Return]
		発見 : TRUE    見つからない : FALSE
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
		指定ディレクトリの中から作成するファイル名の候補を検索して、見つからない場合のみそのファイル名を返す
	[Argument1] path
		ディレクトリのみ
	[Argument2] opfname
		作成ファイル名の先頭名
	[Return]
		成功 : 検索パス    失敗 : empty
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


