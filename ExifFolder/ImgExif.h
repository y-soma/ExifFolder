#pragma once

#include <map>
#include "Tools.h"
#include "Xml.h"


/**************************************************************************
	[Class]
		CImgExif
	[Details]
		�摜��Exif�����Ǘ�����N���X
**************************************************************************/
class CImgExif
{

/* Constructor / Destructor */
public:
	CImgExif(const TSTR& getpath)
	:Path(getpath),m_FileType(0),m_InfinityPrev(0)
	{
		// ������
		Result = Init();
	};
	~CImgExif(void)
	{
	};

public:
/* Enumeration */
	/**************************************************************************	
		[Enumeration]
			FILE_TYPE
		[Details]
			�t�@�C���^�C�v���
	**************************************************************************/	
	enum FILE_TYPE
	{
		JPEG	= 0x00F1,
		RAW		= 0x00F2,
	};
	
	/**************************************************************************	
		[Enumeration]
			IFD_TYPE
		[Details]
			Exif�^�O��IFD�^�C�v���
	**************************************************************************/	
	enum IFD_TYPE
	{
		UNKNOWN            = 0xFFFF,
		_0thIFD            = 0x0000,
		_1stIFD            = 0x0001,
		ExifIFD            = 0x000E,
		// ��OLYMPUS Maker Note
		HardWareIFD        = 0x00E1,
		CameraModeIFD      = 0x00E2,
		ApplicationModeIFD = 0x00E3,
		ImageProcessorIFD  = 0x00E4,
		DebugIFD           = 0x00E5,
		InterOpera         = 0x00EF,
	};
	
	/**************************************************************************	
		[Enumeration]
			DATA_TYPE
		[Details]
			Exif�^�O�̃f�[�^�^�C�v���
	**************************************************************************/	
	enum DATA_TYPE
	{
		_UNKNOWN   = 0x0000,
		_BYTE      = 0x0001,
		_ASCII     = 0x0002,
		_SHORT     = 0x0003,
		_LONG      = 0x0004,
		_RATIONAL  = 0x0005,
		_SBYTE     = 0x0006,
		_UNDEFINED = 0x0007,
		_SSHORT    = 0x0008,
		_SLONG     = 0x0009,
		_SRATIONAL = 0x000A,
		_FLOAT     = 0x000B,
		_DFLOAT    = 0x000C,
		_IFD       = 0x000D,
	};


/* Structure */
	/**************************************************************************
		[Structure]
			ExifTag
		[Details]
			Exif�^�O1���������\����
	**************************************************************************/
	typedef struct EXIF_TAG {
		IFD_TYPE IFD;
		UINT Tag;
		DATA_TYPE Type;
		ULONG Count;
		ULONG Offset;
		TSTR Content;
		TSTR Value;

		EXIF_TAG(){/* Constructor */
			IFD = UNKNOWN;
			Tag = 0;
			Type = _UNKNOWN;
			Count = 0;
			Offset = 0;
			Content.clear();
			Value.clear();
		}
	} ExifTag, *PExifTag;


	/**************************************************************************
		[Structure]
			Ifd
		[Details]
			1����IFD�Z�O�����g�������\����
	**************************************************************************/
	typedef struct IFD {
		UINT Count;
		std::vector<ExifTag> Field;
		ULONG NextIFD;
		
		IFD(){/* Constructor */
			Count = 0;
			NextIFD = 0;
		}
	} Ifd, *PIfd;


/* Member */
private:
	// �擾����Exif�^�O
	std::vector<ExifTag> Tags;
	// �p�X
	TSTR Path;
	// �摜�̃o�C�i���f�[�^
	std::vector<BYTE> Data;
	// �ǂݍ��݌���(���� : 1  Exif��Ή��A�܂��͉摜�t�@�C���łȂ� : 0  ���s : -1)
	int Result;
	// �G���f�B�A��(BIG : 0  LITTLE : 1  defalut : -1)
	int Endian;
	// �C�ӂ�(�^�OID, �ڍ�)��o�^����
	std::map<ULONG,TSTR> StdTag;
	// �eIFD�擾�t���O
	BOOL ExifIfd, MakrNote, InterIfd;
	// �t�@�C���^�C�v
	int m_FileType;
	// �����ċA�h�~�p
	ULONG m_InfinityPrev;



/* Public Member Function */
public:
	/**************************************************************************
		[Function]
			GetData
		[Details]
			Exif�f�[�^���擾����
		[Return]
			Tags
	**************************************************************************/
	const std::vector<ExifTag> GetData(void)
	{
		return Tags;
	}

	/**************************************************************************
		[Function]
			GetResult
		[Details]
			�ǂݍ��݌��ʂ��擾����
		[Return]
			Result
	**************************************************************************/
	const int GetResult(void)
	{
		return Result;
	}

	/**************************************************************************
		[Function]
			InitStdTag
		[Details]
			��{Exif�^�O�o�^�p�̕ϐ���������
	**************************************************************************/
	static void InitStdTag(std::map<ULONG,TSTR>& src)
	{
		// ��{�^�O���̃^�O���ƃ^�O���̃L�[�z����Z�b�g
		const TSTR xmlpath = GetModuleDir() + _T("\\tagsetJ.xml");
		
		// PVE��Xml�t�@�C������^�O�̗v�f���Z�b�g����
		CXml pXml(xmlpath.data());
		//std::vector<TSTR> str(0,_T(""));
		//pXml.GetAttribute(str,_T("entry"),_T("name"));
		std::vector<CXml::EntryAttribute> eAttribute;
		pXml.GetEntryAttribute(eAttribute);

		src.clear();
		for(UINT i=0; i<eAttribute.size(); i++)
			src.insert(std::map<ULONG,TSTR>::value_type(eAttribute[i].value,eAttribute[i].name));

		/*
		src.insert(std::map<ULONG,TSTR>::value_type(0x010E,_T("�^�C�g��")));
		src.insert(std::map<ULONG,TSTR>::value_type(0x010F,_T("���[�J�[��")));
		src.insert(std::map<ULONG,TSTR>::value_type(0x0110,_T("���f����")));
		src.insert(std::map<ULONG,TSTR>::value_type(0x0112,_T("�摜����")));
		src.insert(std::map<ULONG,TSTR>::value_type(0x011A,_T("�摜�̕��̉𑜓x")));
		src.insert(std::map<ULONG,TSTR>::value_type(0x011B,_T("�摜�̍����̉𑜓x")));
		src.insert(std::map<ULONG,TSTR>::value_type(0x0128,_T("�摜�̕��ƍ����̉𑜓x�̒P��")));
		src.insert(std::map<ULONG,TSTR>::value_type(0x0131,_T("�{�̃o�[�W����")));
		src.insert(std::map<ULONG,TSTR>::value_type(0x0132,_T("�t�@�C���X�V����")));
		src.insert(std::map<ULONG,TSTR>::value_type(0x0213,_T("YCC�̉�f�\��")));
		src.insert(std::map<ULONG,TSTR>::value_type(0x8769,_T("Exif IFD�ւ̃|�C���^")));
		src.insert(std::map<ULONG,TSTR>::value_type(0xC4A5,_T("PrintIM Version-0300")));
		src.insert(std::map<ULONG,TSTR>::value_type(0x829A,_T("�I�o����")));
		src.insert(std::map<ULONG,TSTR>::value_type(0x829D,_T("F�l")));
		src.insert(std::map<ULONG,TSTR>::value_type(0x8822,_T("�I�o�v���O����")));
		src.insert(std::map<ULONG,TSTR>::value_type(0x8827,_T("ISO���x")));
		src.insert(std::map<ULONG,TSTR>::value_type(0x9000,_T("Exif�o�[�W����")));
		src.insert(std::map<ULONG,TSTR>::value_type(0x9003,_T("�摜�f�[�^�̐�������")));
		src.insert(std::map<ULONG,TSTR>::value_type(0x9004,_T("�f�W�^���f�[�^�̐�������")));
		src.insert(std::map<ULONG,TSTR>::value_type(0x9101,_T("�e�R���|�[�l���g�̈Ӗ�")));
		src.insert(std::map<ULONG,TSTR>::value_type(0x9204,_T("�I�o�␳�l")));
		src.insert(std::map<ULONG,TSTR>::value_type(0x9205,_T("�����Y���F�l")));
		src.insert(std::map<ULONG,TSTR>::value_type(0x9207,_T("��������")));
		src.insert(std::map<ULONG,TSTR>::value_type(0x9208,_T("����")));
		src.insert(std::map<ULONG,TSTR>::value_type(0x9209,_T("�t���b�V��")));
		src.insert(std::map<ULONG,TSTR>::value_type(0x920A,_T("�œ_����")));
		src.insert(std::map<ULONG,TSTR>::value_type(0x927C,_T("���[�J�[�m�[�g")));
		src.insert(std::map<ULONG,TSTR>::value_type(0x9286,_T("���[�U�[�R�����g")));
		src.insert(std::map<ULONG,TSTR>::value_type(0xA000,_T("�Ή�FlashPix�o�[�W����")));
		src.insert(std::map<ULONG,TSTR>::value_type(0xA001,_T("�F��ԏ��")));
		src.insert(std::map<ULONG,TSTR>::value_type(0xA002,_T("��")));
		src.insert(std::map<ULONG,TSTR>::value_type(0xA003,_T("����")));
		src.insert(std::map<ULONG,TSTR>::value_type(0xA005,_T("�݊���IFD�ւ̃|�C���^")));
		src.insert(std::map<ULONG,TSTR>::value_type(0xA300,_T("�t�@�C���\�[�X")));
		src.insert(std::map<ULONG,TSTR>::value_type(0xA401,_T("�J�X�^���C���[�W����")));
		src.insert(std::map<ULONG,TSTR>::value_type(0xA402,_T("�I�o���[�h")));
		src.insert(std::map<ULONG,TSTR>::value_type(0xA403,_T("�z���C�g�o�����X")));
		src.insert(std::map<ULONG,TSTR>::value_type(0xA404,_T("�f�W�^���Y�[���{��")));
		src.insert(std::map<ULONG,TSTR>::value_type(0xA406,_T("�V�[���L���v�`���[���[�h")));
		src.insert(std::map<ULONG,TSTR>::value_type(0xA407,_T("�Q�C������")));
		src.insert(std::map<ULONG,TSTR>::value_type(0xA408,_T("�R���g���X�g")));
		src.insert(std::map<ULONG,TSTR>::value_type(0xA409,_T("�ʓx")));
		src.insert(std::map<ULONG,TSTR>::value_type(0xA40A,_T("�V���[�v�l�X")));
		src.insert(std::map<ULONG,TSTR>::value_type(0x0001,_T("�݊������ʎq")));
		src.insert(std::map<ULONG,TSTR>::value_type(0x0002,_T("�݊��o�[�W����")));
		src.insert(std::map<ULONG,TSTR>::value_type(0x0103,_T("���k���")));
		src.insert(std::map<ULONG,TSTR>::value_type(0x0201,_T("�T���l�C���ւ̃|�C���^")));
		src.insert(std::map<ULONG,TSTR>::value_type(0x0202,_T("�T���l�C���̃o�C�g��")));
		// Maker Note
		src.insert(std::map<ULONG,TSTR>::value_type(0x0101,_T("Body Serial.No")));
		src.insert(std::map<ULONG,TSTR>::value_type(0x0102,_T("MC Serial.No")));
		src.insert(std::map<ULONG,TSTR>::value_type(0x0104,_T("Body FW.Version")));
		src.insert(std::map<ULONG,TSTR>::value_type(0x0203,_T("Lens Name")));
		*/
		
	}


/* Private Member Function */
private:
	#pragma warning (disable:4996)
	/*------------------------------------------------------------------------
		[Function]
			Init
		[Details]
			����������
		[Return]
			���� : 1  Exif��Ή��A�܂��͉摜�t�@�C���łȂ� : 0  ���s : -1
	------------------------------------------------------------------------*/
	int Init()
	{
		{// �����o�̏�����
			Data.resize(0,0);
			Endian = -1;
			ExifIfd = FALSE; MakrNote = FALSE; InterIfd = FALSE;
			InitStdTag(StdTag);
		}
		
		FILE* fp;
	#ifndef UNICODE
		fp = fopen(Path.data(),"rb,ccs=UNICODE");
	#else
		fp = _wfopen(Path.data(),L"rb,ccs=UNICODE");
	#endif
		if(!fp) return -1;
		
		ULONG sz = 0;
		{// �t�@�C���T�C�Y�擾
			// �t�@�C�����Ō�̈ʒu�܂ňړ�������
			fseek(fp,0,SEEK_END);
			//long�^��ftell()�����Afpos_t�^��fgetpos()���g�p�����ق���
			//const long sz = ftell(fp);
			fpos_t gsz = 0;
			fgetpos(fp,&gsz);
			sz = static_cast<long>(gsz);
			// ���̈ʒu�֖߂�
			fseek(fp,0,SEEK_SET);
			// ���܂�ɂ��t�@�C���T�C�Y�̏��������̂͏������Ȃ�
			if(sz < 0xAF) return 0;
		}

		// �T�C�Y���̗̈���m�ۂ��Ď擾
		std::vector<BYTE> buf(sz,0);
		const size_t read = (int)fread(&buf[0],sizeof(BYTE),sz,fp);
		fclose(fp);
		if(read == -1) return -1;
			
		// �����o�փR�s�[
		Data = buf;

		// �t�@�C���^�C�v�̎��ʂƃo�C�g�I�[�_�[�̎擾
		const TSTR exif = CharJoint(6,9);
		TSTR byteorder = _T("");
		if(exif == _T("Exif"))
		{// Exif���ʃR�[�h�`�F�b�N
			byteorder = CharJoint(12,13);
			m_FileType = JPEG;
		}
		else
		{
			byteorder = CharJoint(0,1);
			m_FileType = RAW;
		}
		
		// �o�C�g�I�[�_���m�F���A�G���f�B�A�����m�肷��
		if(byteorder == _T("II"))
			Endian = 0;
		else if(byteorder == _T("MM"))
			Endian = 1;
		else
			return 0;
		
		{// �^�O�擾
			std::vector<ExifTag> Field;
			const long ret = m_FileType==JPEG? GetField(Field,0x0C+BinJoint((ULONG)0x10,(ULONG)4),_0thIFD):GetField(Field,BinJoint((ULONG)0x04,(ULONG)4),_0thIFD,0x00);
			if(ret == -1)
			{// 1stIFD�̂ݎ擾
				MessageBox(NULL,_T("�^�O�̎擾�Ɏ��s���܂���"),_T("Exif Error"),MB_OK|MB_ICONERROR);
				return -1;
			}
			// �c��̑S�Ẵ^�O���擾
			Tags = m_FileType==RAW? GetExifTag(Field,0x00):GetExifTag(Field);
		}

		return 1;
	}

	
	/*------------------------------------------------------------------------
		[Function]
			CharJoint
		[Details]
			�w�肵���͈͂̃o�C�i���f�[�^��A�����ĕ�����Ŏ擾����
		[Argument1] s
			�J�n�_
		[Argument2] e
			�I��
		[Return]
			�A����̕�����
	------------------------------------------------------------------------*/
	TSTR CharJoint(const ULONG& s, const ULONG& e)
	{
		TSTR dst = _T("");
		if(s > e || s > Data.size() || e > Data.size()) return dst;
		for(ULONG i=s; i<=e; i++)
		{
			TCHAR buf = Data[i];
			dst += buf;
		}
		return dst;
	}


	/*------------------------------------------------------------------------
		[Function]
			BinJoint
		[Details]
			�w��ʒu����w��o�C�g���̃Z�O�����g�l���擾����
		[Argument1] start
			�A�N�Z�X�J�n�ʒu
		[Argument2] bytes
			�o�C�g��
		[Return]
			�Z�O�����g�l
	------------------------------------------------------------------------*/
	ULONG BinJoint(const ULONG& start, const ULONG& bytes)
	{
		ULONG dst = 0;
		// �f�[�^�T�C�Y�𒴂����A�N�Z�X�͂ł��Ȃ�
		if(start >= Data.size()) return dst;
		try
		{
			// �摜�ւ̃A�N�Z�X�|�C���^
			const BYTE* ptr = &Data[0];
			ptr += start;
			// �r�b�O�G���f�B�A���̏ꍇ�͋t�Ɏ��o���A���g���G���f�B�A���̏ꍇ�͐������Ɏ��o��
			const ULONG AryMax = bytes-1;
			for(ULONG i=0; i<=AryMax; i++)
			{
				const ULONG bshift = !Endian? i:AryMax-i;
				dst += (ptr[i]<<(bshift*8));
			}
			return dst;
		}
		catch(...)
		{
			return 0;
		}
	}


	/*------------------------------------------------------------------------
		[Function]
			BinJoint
		[Details]
			�w��ʒu����w��o�C�g���̃Z�O�����g�l���擾����(�����L�萮���o�[�W����)
		[Argument1] start
			�A�N�Z�X�J�n�ʒu
		[Argument2] bytes
			�o�C�g��
		[Return]
			�Z�O�����g�l
	------------------------------------------------------------------------*/
	LONG BinJoint(const LONG& start, const LONG& bytes)
	{
		LONG dst = 0;
		// �f�[�^�T�C�Y�𒴂����A�N�Z�X�͂ł��Ȃ�
		if((ULONG)start >= Data.size()) return dst;
		try
		{
			// �摜�ւ̃A�N�Z�X�|�C���^
			const BYTE* ptr = &Data[0];
			ptr += start;
			// �r�b�O�G���f�B�A���̏ꍇ�͋t�Ɏ��o���A���g���G���f�B�A���̏ꍇ�͐������Ɏ��o��
			const LONG AryMax = bytes-1;
			for(LONG i=0; i<=AryMax; i++)
			{
				const LONG bshift = !Endian? i:AryMax-i;
				dst += (ptr[i]<<(bshift*8));
			}
			return dst;
		}
		catch(...)
		{
			return 0;
		}
	}


	/*------------------------------------------------------------------------
		[Function]
			GetField
		[Details]
			IFD�Z�O�����g�Ɋi�[����Ă���G���g���[�t�B�[���h���擾����
		[Argument1] dst
			�i�[��
		[Argument2] ptr
			IFD�L�q�|�C���g�̐擪
		[Argument3] ifd
			IFD�^�C�v
		[Argument4] StdPos
			�I�t�Z�b�g�ɑ΂����ʒu
		[Return]
			��������Ɗi�[�ɐ������������Ԃ�B�v���I�ȃG���[�����������ꍇ�̂�-1���Ԃ�
	------------------------------------------------------------------------*/
	long GetField(std::vector<ExifTag>& dst, const ULONG& ptr, const IFD_TYPE& ifd, const ULONG StdPos = 0x0C)
	{
		try
		{
			ULONG IfdPtr = ptr;
			long ret = 0;
			while(1)
			{
				// IFD�^�O�̌�
				const ULONG IfdCnt = BinJoint(IfdPtr,2);
				// �������̖����J�E���g����������~�߂�
				if(IfdCnt > 118) break;
				// �t�B�[���h����擾����
				for(ULONG i=0; i<IfdCnt; i++)
				{
					ExifTag buf;
					buf.IFD = ifd;
					buf.Tag = (UINT)BinJoint(IfdPtr+2+(i*12),2);
					const UINT dtype = BinJoint(IfdPtr+4+(i*12),2);
					buf.Type = dtype > 0 && dtype < 13? (DATA_TYPE)dtype:_UNKNOWN;
					buf.Count = BinJoint(IfdPtr+6+(i*12),4);
					buf.Offset = GetOffset(buf.Type,IfdPtr+10+(i*12));
					buf.Value = RemoveNull(GetTagStr(buf,StdPos));
					buf.Content = StdTag[buf.Tag]==_T("")? _T("UNKNOWN"):StdTag[buf.Tag];
					dst.push_back(buf);
					ret++;
				}
				// ����IFD������ꍇ
				const ULONG NextIfd = 2 + IfdPtr + (IfdCnt*12);
				IfdPtr = 0x0C + BinJoint(NextIfd,4);
				if(!IfdPtr) break;
			}
			return ret;
		}
		catch(...)
		{
			return -1;
		}
	}


	/*------------------------------------------------------------------------
		[Function]
			GetExifTag
		[Details]
			Exif�^�O��S�Ď擾����
		[Argument1] src
			�X�V�f�[�^
		[Argument2] StdPos
			�I�t�Z�b�g�ɑ΂����ʒu
		[Return]
			��������Ɗi�[�ɐ������������Ԃ�B�v���I�ȃG���[�����������ꍇ�̂�-1���Ԃ�
	------------------------------------------------------------------------*/
	std::vector<ExifTag> GetExifTag(const std::vector<ExifTag>& src, const ULONG StdPos = 0x0C)
	{
		std::vector<ExifTag> dst = src;
		// �ʉ߂������Ƃ�������t���O
		BOOL flg = FALSE;
		for(UINT i=0; i<src.size(); i++)
		{// �ړI��IFD�^�O������
			switch(src[i].Tag)
			{
			// Exif IFD�ւ̃|�C���^
			case 0x8769:
				if(!ExifIfd){
					if(GetField(dst,src[i].Offset+StdPos,ExifIFD,StdPos) == -1) return dst;
					ExifIfd = TRUE;
				}
				flg = TRUE;
				break;
			// ���[�J�[�m�[�g
			case 0x927C:
				if(!MakrNote){
					if(GetOlympusMakerNote(dst,src[i].Offset+StdPos) == -1) return dst;
					MakrNote = TRUE;
				}
				flg = TRUE;
				break;
			// �݊���IFD�ւ̃|�C���^
			case 0xA005:
				if(!InterIfd){
					if(GetField(dst,src[i].Offset+StdPos,_1stIFD,StdPos) == -1) return dst;
					InterIfd = TRUE;
				}
				flg = TRUE;
				break;
			}
		}

		// �S�Ď擾�ł�����I��
		if((ExifIfd && MakrNote && InterIfd) || !flg) return dst;
		if(flg){
			m_InfinityPrev++;
			if(m_InfinityPrev > 1000) return dst;
		}
		// ��L�Ń^�O���X�V�ł����ꍇ�͂���Ɏ��̃^�O��T��
		return GetExifTag(dst,StdPos);
	}


	/*------------------------------------------------------------------------
		[Function]
			GetOffset
		[Details]
			�^�O�̃I�t�Z�b�g����������
		[Argument1] type
			�f�[�^�^�C�v
		[Argument2] ptr
			�L�q�ꏊ�̐擪�|�C���^
		[Return]
			�I�t�Z�b�g�l
	------------------------------------------------------------------------*/
	ULONG GetOffset(const UINT& type, const ULONG& ptr)
	{
		// 4byte�����e�ʂ������������^�̏ꍇ�́A�L�q�ꏊ�̐擪���炻�̌^�̃o�C�g�����̂ݓǂݍ���(��������̃o�C�g�͖����̈�ł���)
		ULONG dst = 0;
		switch(type)
		{// 4byte�����̐����^�̂ݎ擾�͈͐���
		case _BYTE:
		case _SBYTE:
			dst = BinJoint(ptr,1);
			break;
		case _SHORT:
		case _SSHORT:
			dst = BinJoint(ptr,2);
			break;
		default:
			dst = BinJoint(ptr,4);
			break;
		}
		return dst;
	}


	/*------------------------------------------------------------------------
		[Function]
			GetTagStr
		[Details]
			�^�O���𕶎���Ƃ��ď�������
		[Argument1] src
			�t�B�[���h���
		[Argument2] StdPos
			�I�t�Z�b�g�ɑ΂����ʒu
		[Return]
			������Ŏ擾�����^�O�l
	------------------------------------------------------------------------*/
	TSTR GetTagStr(const ExifTag& src, const ULONG StdPos = 0x0C)
	{
		TSTR dst = _T("");
		// �^�O�ǂݏo���ʒu
		const ULONG ptr = StdPos+src.Offset;
		switch(src.Type)
		{// �f�[�^�^�C�v�ɉ����ĕ���
		case _BYTE:
		case _SHORT:
		case _LONG:
		case _SBYTE:
		case _SSHORT:
		case _SLONG:
		case _IFD:
			// �����^�̏ꍇ�̓I�t�Z�b�g�l�����̂܂ܑ������
			dst = LToStr(src.Offset);
			break;
		case _ASCII:
			dst = CharJoint(ptr,ptr+src.Count);
			break;
		case _RATIONAL:
			dst = src.Tag==0x829A? GetRATIONAL(ptr,1):GetRATIONAL(ptr);
			// F�l,�V���b�^�[���̏ꍇ�͏����_��1�ʂ܂ł��ۂ߂�
			if(src.Tag == 0x829D || src.Tag == 0x920A)
				dst = DToStr(RoundEx(StrToD(dst.data()),1),1);
			break;
		case _UNDEFINED:
			// ���[�J�[�ˑ��̌^(���v�擾���@����)
			dst = CharJoint(ptr,ptr+src.Count);
			break;
		case _SRATIONAL:
			dst = GetSRATIONAL(ptr);
			break;
		case _FLOAT:
			break;
		case _DFLOAT:
			dst = GetDFLOAT(ptr);
			break;
		case _UNKNOWN:
		default:
			break;
		}
		return dst;
	}


	/*------------------------------------------------------------------------
		[Function]
			GetDFLOAT
		[Details]
			�f�[�^��DFLOAT�^�C�v�̃^�O�𕶎���Ŏ擾����
		[Argument1] start
			�f�[�^�L�q�ꏊ
		[Return]
			������Ŏ擾�����l
	------------------------------------------------------------------------*/
	TSTR GetDFLOAT(const ULONG& start)	
	{
		try
		{
			std::vector<char> bin(0,0);
			{// �o�C�i���f�[�^��char�z��Ƃ��Ď��o��
				const BYTE* ptr = &Data[0];
				ptr += start;
				const LONG AryMax = 7;
				for(LONG i=0; i<=AryMax; i++)
				{
					const int BitSize = sizeof(BYTE)*8;
					int bit = 1;
					const LONG bshift = !Endian? i:AryMax-i;
					for(int j=0; j<BitSize; j++)
					{
						const char buf = ptr[bshift]&bit? 1:0;
						bin.push_back(buf);
						bit <<= 1;
					}
				}
			}

			// 64bit�ȉ��͗L�蓾�Ȃ�
			if(bin.size() < 64) return _T("");

			// ������1 �w����11 ������52
			double dst = 0.0;
			{// double�^�𒊏o
				const char* binptr = &bin[0];
				
				// ����
				const double Sign = !binptr[0]? 1.0:-1.0;
				
				// �w��
				int Index = 0;
				// �w���̕���(FALSE:�� TRUE:��)
				const BOOL ISign = !binptr[1]? FALSE:TRUE;
				{
					int bshift = 10;
					for(int i=2; i<12; i++)
					{
						Index += (binptr[i]<<bshift);
						bshift--;
					}
				}
				
				// ����
				double Mantissa = 0.0;
				{
					int bshift = 52;
					for(int i=12; i<64; i++)
					{
						Mantissa += (binptr[i]<<bshift);
						bshift--;
					}
					Mantissa = Mantissa*pow(2.0,-52.0) + 1.0;
				}

				dst = !ISign? Mantissa*pow(2.0,(double)Index):Mantissa*pow(2.0,(double)Index*-1.0);
				dst *= Sign;
			}
			return DToStr(dst);
		}
		catch(...)
		{
			return _T("");
		}
	}


	/*------------------------------------------------------------------------
		[Function]
			GetRATIONAL
		[Details]
			�f�[�^��RATIONAL�^�C�v�̃^�O�𕶎���Ŏ擾����
		[Argument1] start
			�f�[�^�L�q�ꏊ
		[Argument2] GetType
			�擾�`�����w��( 0:�����^  1:�����^ )
		[Return]
			������Ŏ擾�����l(�擾�`���͏����^�Ƃ���)
	------------------------------------------------------------------------*/
	TSTR GetRATIONAL(const ULONG& start, const int GetType = 0)	
	{
		try
		{
			const ULONG Numerator = BinJoint(start,(ULONG)4);
			const ULONG Denominator = BinJoint(start+4,(ULONG)4);
			TSTR dst = _T("");
			switch(GetType)
			{
			case 0:
				dst = DToStr((double)Numerator / (double)Denominator);
				break;
			case 1:
				dst = LToStr(Numerator) + _T("�^") + LToStr(Denominator);
				break;
			default:
				break;
			}
			return dst;
		}
		catch(...)
		{
			return _T("");
		}
	}


	/*------------------------------------------------------------------------
		[Function]
			GetSRATIONAL
		[Details]
			�f�[�^��SRATIONAL�^�C�v�̃^�O�𕶎���Ŏ擾����
		[Argument1] start
			�f�[�^�L�q�ꏊ
		[Argument2] GetType
			�擾�`�����w��( 0:�����^  1:�����^ )
		[Return]
			������Ŏ擾�����l
	------------------------------------------------------------------------*/
	TSTR GetSRATIONAL(const ULONG& start, const int GetType = 0)	
	{
		try
		{
			const LONG Numerator = BinJoint((LONG)start,(LONG)4);
			const LONG Denominator = BinJoint((LONG)start+4,(LONG)4);
			TSTR dst = _T("");
			switch(GetType)
			{
			case 0:
				dst = DToStr((double)Numerator / (double)Denominator);
				break;
			case 1:
				dst = LToStr(Numerator) + _T("�^") + LToStr(Denominator);
				break;
			default:
				break;
			}
			return dst;
		}
		catch(...)
		{
			return _T("");
		}
	}


	/*------------------------------------------------------------------------
		[Function]
			GetOlympusMakerNote
		[Details]
			�I�����p�X�̃��[�J�[�m�[�g���擾����
		[Argument1] dst
			�i�[��
		[Argument2] ptr
			IFD�L�q�|�C���g�̐擪
		[Return]
			��������Ɗi�[�ɐ������������Ԃ�B�v���I�ȃG���[�����������ꍇ�̂�-1���Ԃ�
	------------------------------------------------------------------------*/
	long GetOlympusMakerNote(std::vector<ExifTag>& dst, const ULONG& ptr)
	{
		int EndianCopy = Endian;
		try
		{
			long ret = 0;
			
			// OLYMPUS�̃��[�J�[�m�[�g���`�F�b�N
			const TSTR olymptag = CharJoint(ptr,ptr+6);
			if(olymptag != _T("OLYMPUS")) return ret;
			
			{// �o�C�g�I�[�_�[�`�F�b�N
				int EndianTmp = -1;
				const TSTR byteorder = CharJoint(ptr+8,ptr+9);
				if(byteorder == _T("II"))
					EndianTmp = 0;
				else if(byteorder == _T("MM"))
					EndianTmp = 1;

				if(EndianTmp == -1) return ret;
				if(Endian != EndianTmp) Endian = EndianTmp;
			}

			// �I�t�Z�b�g���牽Byte�����12�o�C�g���ȉ���IFD�ւ̃I�t�Z�b�g���L�q����Ă���G���A��ǂݎ��
			// ������IFD�͕s���@InterOpera   = 0x00EF,
			const IFD_TYPE ifd[5] = { HardWareIFD,CameraModeIFD,ApplicationModeIFD,ImageProcessorIFD,DebugIFD };
			for(ULONG i=0; i<sizeof(ifd)/sizeof(ifd[0]); i++)
			{// IFD�擾
				// JPEG�̏ꍇ�̓��[�J�[�m�[�g�J�n�n�_����38byte�ڂ��珇��IFD�ւ̃I�t�Z�b�g���L�q����Ă���BRAW�̏ꍇ��50byte��
				// �]���āA�I�t�Z�b�g�l�̂ݓǂݍ��ނ悤�ɂ���ɂ�JPEG�̏ꍇ�A46byte��(�^�O2+�^�C�v2+�J�E���g4��8��Ȃ̂�38+8=46)������12byte���Q�Ƃ���Ηǂ�
				const ULONG IfdPtr = m_FileType==RAW? GetOffset(_IFD,ptr+58+(i*12)):GetOffset(_IFD,ptr+46+(i*12));
				if(GetField(dst,ptr+IfdPtr,ifd[i],ptr) == -1){
					if(Endian != EndianCopy) Endian = EndianCopy;
					return -1;
				}
				ret++;
			}
			if(Endian != EndianCopy) Endian = EndianCopy;
			return ret;
		}
		catch(...)
		{
			if(Endian != EndianCopy) Endian = EndianCopy;
			return -1;
		}
	}


	
}; // End Class

