#pragma once

#include "Tools.h"

#import <msxml4.dll>


/*/////////////////////////////////////////////////////////////////////////
	[Class]
		CXml
	[Purpose]
		Xml�戵���N���X
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
		// ���Ȃ��Ă����v����
		// COM���C�u�����̏I������
		//::CoUninitialize();
	};

public:
	/**************************************************************************
		[Structure]
			EntryAttribute
		[Purpose]
			"entry"�t�B�[���h��̃A�g���r���[�g���Ǘ�����\����
	**************************************************************************/
	typedef struct tagENTRYATTRIBUTE
	{
		// �v�f��
		TSTR name;
		// �l
		long value;
		// Constructor
		tagENTRYATTRIBUTE():name(_T("")),value(0){}
	} EntryAttribute;

// �����o�ϐ�
private:
	// Xml����h�L�������g
	MSXML2::IXMLDOMDocument2Ptr m_pDoc;
	// Xml�t�@�C���𐳏�ɓǂݍ��񂾂��ǂ����̃t���O
	BOOL m_bXmlLoadFlg;


// �����o�֐�
public:
	/**************************************************************************
		[Function]
			LoadXml
		[Purpose]
			Xml�t�@�C���ǂݍ���
		[Parameter1] path
			�t�@�C���p�X
		[Returns]
			�����FTRUE  ���s�FFALSE
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
			Xml�ǂݍ��݃t���O�̎擾
		[Returns]
			�t���O�̃����o�ϐ��̎Q�Ƃ�Ԃ�
	**************************************************************************/
	inline const BOOL& IsXmlLoad(void) const { return m_bXmlLoadFlg; };


	/**************************************************************************
		[Function]
			GetAttribute
		[Purpose]
			Xml�t�@�C�����̎w�肵���v�f���̃A�g���r���[�g���擾����
		[Parameter1] dst
			���ʊi�[
		[Parameter2] pMainElements
			���C���v�f��
		[Parameter3] pAttributeName
			�A�g���r���[�g��
		[Parameter4] pSubElements
			�T�u�v�f��������ꍇ�͎w�肷��
		[Returns]
			�����FTRUE  ���s�FFALSE
	**************************************************************************/
	BOOL GetAttribute(std::vector<TSTR>& dst, const TCHAR* const pMainElements, const TCHAR* const pAttributeName, const TCHAR* const pSubElements = NULL)
	{
		try
		{
			MSXML2::IXMLDOMNodeListPtr pnList = m_pDoc->getElementsByTagName(pMainElements);
			if(!pnList) return FALSE;
			const int size = pnList->Getlength();
			for(int i=0; i<size; i++)
			{// �w�肵���v�f���̑S�ẴA�g���r���[�g������
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
			Xml�t�@�C������"entry"�v�f����"name","value"�̃A�g���r���[�g���擾����
		[Parameter1] dst
			���ʊi�[
		[Returns]
			�����FTRUE  ���s�FFALSE
	**************************************************************************/
	BOOL GetEntryAttribute(std::vector<EntryAttribute>& dst)
	{
		try
		{
			MSXML2::IXMLDOMNodeListPtr pnList = m_pDoc->getElementsByTagName(_T("entry"));
			if(!pnList) return FALSE;
			const int size = pnList->Getlength();
			for(int i=0; i<size; i++)
			{// "entry"�v�f����"name"��"val"�̑S�ẴA�g���r���[�g������
				MSXML2::IXMLDOMElementPtr peName = pnList->Getitem(i);
				if(!peName) return FALSE;
				EntryAttribute buf;
				{// "name"�v�f�擾
					_variant_t pNameAttribute = peName->getAttribute(_T("name"));
					buf.name = BstrToString(pNameAttribute.bstrVal);
				}
				{// "val"�v�f�擾
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
			����������
	-------------------------------------------------------------------------*/
	void Init(void)
	{
		// COM���C�u�����̏�����
		CoInitialize(NULL);
		m_pDoc.CreateInstance(__uuidof(MSXML2::DOMDocument40));
		m_pDoc->async = VARIANT_FALSE;
	}


#pragma warning (disable:4996)
	/*-------------------------------------------------------------------------
		[Function]
			BstrToString
		[Purpose]
			BSTR��String�^�ւ̕ϊ�
		[Parameter1] src
			�ϊ��O������
		[Returns]
			�ϊ��㕶����
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

