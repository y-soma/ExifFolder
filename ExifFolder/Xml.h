#pragma once

#include "Tools.h"

#import <msxml4.dll>


/*/////////////////////////////////////////////////////////////////////////
	[Class]
		CXml
	[Purpose]
		Xml取扱いクラス
/////////////////////////////////////////////////////////////////////////*/
class CXml
{
public:
	CXml(void)
	:m_bXmlLoadFlg(FALSE)
	{
		setlocale(LC_ALL,"Japanese");
		Init();
	};

	CXml(const TCHAR* const xmlpath)
	{
		Init();
		LoadXml(xmlpath);
	};

	~CXml(void)
	{
		// ※なくても大丈夫そう
		// COMライブラリの終了処理
		//::CoUninitialize();
	};

public:
	/**************************************************************************
		[Structure]
			EntryAttribute
		[Purpose]
			"entry"フィールド上のアトリビュートを管理する構造体
	**************************************************************************/
	typedef struct tagENTRYATTRIBUTE
	{
		// 要素名
		TSTR name;
		// 値
		long value;
		// Constructor
		tagENTRYATTRIBUTE():name(_T("")),value(0){}
	} EntryAttribute;

// メンバ変数
private:
	// Xml操作ドキュメント
	MSXML2::IXMLDOMDocument2Ptr m_pDoc;
	// Xmlファイルを正常に読み込んだかどうかのフラグ
	BOOL m_bXmlLoadFlg;


// メンバ関数
public:
	/**************************************************************************
		[Function]
			LoadXml
		[Purpose]
			Xmlファイル読み込み
		[Parameter1] path
			ファイルパス
		[Returns]
			成功：TRUE  失敗：FALSE
	**************************************************************************/
	inline VARIANT_BOOL LoadXml(const TCHAR* const path)
	{
		const VARIANT_BOOL ret = m_pDoc->load(path);
		m_bXmlLoadFlg = ret==VARIANT_TRUE? TRUE:FALSE;
		return ret;
	}


	/**************************************************************************
		[Function]
			IsXmlLoad
		[Purpose]
			Xml読み込みフラグの取得
		[Returns]
			フラグのメンバ変数の参照を返す
	**************************************************************************/
	inline const BOOL& IsXmlLoad(void) const { return m_bXmlLoadFlg; };


	/**************************************************************************
		[Function]
			GetAttribute
		[Purpose]
			Xmlファイル中の指定した要素名のアトリビュートを取得する
		[Parameter1] dst
			結果格納
		[Parameter2] pMainElements
			メイン要素名
		[Parameter3] pAttributeName
			アトリビュート名
		[Parameter4] pSubElements
			サブ要素名がある場合は指定する
		[Returns]
			成功：TRUE  失敗：FALSE
	**************************************************************************/
	BOOL GetAttribute(std::vector<TSTR>& dst, const TCHAR* const pMainElements, const TCHAR* const pAttributeName, const TCHAR* const pSubElements = NULL)
	{
		try
		{
			MSXML2::IXMLDOMNodeListPtr pnList = m_pDoc->getElementsByTagName(pMainElements);
			if(!pnList) return FALSE;
			const int size = pnList->Getlength();
			for(int i=0; i<size; i++)
			{// 指定した要素下の全てのアトリビュートを検索
				MSXML2::IXMLDOMElementPtr peName = pnList->Getitem(i);
				MSXML2::IXMLDOMElementPtr pElement = NULL;
				if(!peName) return FALSE;
				if(pSubElements != NULL)
					pElement = peName->getElementsByTagName(pSubElements)->Getitem(0);
				else
					pElement = peName;
				if(!pElement) return FALSE;
				_variant_t pAttribute = pElement->getAttribute(pAttributeName);
				dst.push_back(BstrToString(pAttribute.bstrVal));
			}
			return TRUE;
		}
		catch(...)
		{
			return FALSE;
		}
	}


	/**************************************************************************
		[Function]
			GetEntryAttribute
		[Purpose]
			Xmlファイル中の"entry"要素内の"name","value"のアトリビュートを取得する
		[Parameter1] dst
			結果格納
		[Returns]
			成功：TRUE  失敗：FALSE
	**************************************************************************/
	BOOL GetEntryAttribute(std::vector<EntryAttribute>& dst)
	{
		try
		{
			MSXML2::IXMLDOMNodeListPtr pnList = m_pDoc->getElementsByTagName(_T("entry"));
			if(!pnList) return FALSE;
			const int size = pnList->Getlength();
			for(int i=0; i<size; i++)
			{// "entry"要素下の"name"と"val"の全てのアトリビュートを検索
				MSXML2::IXMLDOMElementPtr peName = pnList->Getitem(i);
				if(!peName) return FALSE;
				EntryAttribute buf;
				{// "name"要素取得
					_variant_t pNameAttribute = peName->getAttribute(_T("name"));
					buf.name = BstrToString(pNameAttribute.bstrVal);
				}
				{// "val"要素取得
					_variant_t pValAttribute = peName->getAttribute(_T("val"));
					TSTR valstr = BstrToString(pValAttribute.bstrVal);
					#ifdef UNICODE
						buf.value = wcstol(valstr.data(),NULL,16);
					#else
						buf.value = strtol(valstr.data(),NULL,16);
					#endif
				}
				dst.push_back(buf);
			}
			return TRUE;
		}
		catch(...)
		{
			return FALSE;
		}
	}



private:
	/*-------------------------------------------------------------------------
		[Function]
			Init
		[Purpose]
			初期化処理
	-------------------------------------------------------------------------*/
	void Init(void)
	{
		// COMライブラリの初期化
		CoInitialize(NULL);
		m_pDoc.CreateInstance(__uuidof(MSXML2::DOMDocument40));
		m_pDoc->async = VARIANT_FALSE;
	}


#pragma warning (disable:4996)
	/*-------------------------------------------------------------------------
		[Function]
			BstrToString
		[Purpose]
			BSTR→String型への変換
		[Parameter1] src
			変換前文字列
		[Returns]
			変換後文字列
	-------------------------------------------------------------------------*/
	TSTR BstrToString(const BSTR& src)
	{
		TSTR dst = _T("");
		#ifdef UNICODE
			wchar_t buffer[MAX_PATH];
			wcscpy(buffer,pAttribute.bstrVal);
			dst = &buffer[0];
		#else
			char buffer[MAX_PATH];
			strcpy(buffer,ToMBCS(src).data());
			dst = &buffer[0];
		#endif
		return dst;
	}

}; // End Class

