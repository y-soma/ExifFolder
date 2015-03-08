#pragma once

#include <map>
#include "Tools.h"
#include "Xml.h"


/**************************************************************************
	[Class]
		CImgExif
	[Details]
		画像のExif情報を管理するクラス
**************************************************************************/
class CImgExif
{

/* Constructor / Destructor */
public:
	CImgExif(const TSTR& getpath)
	:Path(getpath),m_FileType(0),m_InfinityPrev(0)
	{
		// 初期化
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
			ファイルタイプを列挙
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
			ExifタグのIFDタイプを列挙
	**************************************************************************/	
	enum IFD_TYPE
	{
		UNKNOWN            = 0xFFFF,
		_0thIFD            = 0x0000,
		_1stIFD            = 0x0001,
		ExifIFD            = 0x000E,
		// ↓OLYMPUS Maker Note
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
			Exifタグのデータタイプを列挙
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
			Exifタグ1つ分を示す構造体
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
			1つ分のIFDセグメントを示す構造体
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
	// 取得したExifタグ
	std::vector<ExifTag> Tags;
	// パス
	TSTR Path;
	// 画像のバイナリデータ
	std::vector<BYTE> Data;
	// 読み込み結果(成功 : 1  Exif非対応、または画像ファイルでない : 0  失敗 : -1)
	int Result;
	// エンディアン(BIG : 0  LITTLE : 1  defalut : -1)
	int Endian;
	// 任意に(タグID, 詳細)を登録する
	std::map<ULONG,TSTR> StdTag;
	// 各IFD取得フラグ
	BOOL ExifIfd, MakrNote, InterIfd;
	// ファイルタイプ
	int m_FileType;
	// 無限再帰防止用
	ULONG m_InfinityPrev;



/* Public Member Function */
public:
	/**************************************************************************
		[Function]
			GetData
		[Details]
			Exifデータを取得する
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
			読み込み結果を取得する
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
			基本Exifタグ登録用の変数を初期化
	**************************************************************************/
	static void InitStdTag(std::map<ULONG,TSTR>& src)
	{
		// 基本タグ情報のタグ№とタグ名のキー配列をセット
		const TSTR xmlpath = GetModuleDir() + _T("\\tagsetJ.xml");
		
		// PVEのXmlファイルからタグの要素をセットする
		CXml pXml(xmlpath.data());
		//std::vector<TSTR> str(0,_T(""));
		//pXml.GetAttribute(str,_T("entry"),_T("name"));
		std::vector<CXml::EntryAttribute> eAttribute;
		pXml.GetEntryAttribute(eAttribute);

		src.clear();
		for(UINT i=0; i<eAttribute.size(); i++)
			src.insert(std::map<ULONG,TSTR>::value_type(eAttribute[i].value,eAttribute[i].name));

		/*
		src.insert(std::map<ULONG,TSTR>::value_type(0x010E,_T("タイトル")));
		src.insert(std::map<ULONG,TSTR>::value_type(0x010F,_T("メーカー名")));
		src.insert(std::map<ULONG,TSTR>::value_type(0x0110,_T("モデル名")));
		src.insert(std::map<ULONG,TSTR>::value_type(0x0112,_T("画像方向")));
		src.insert(std::map<ULONG,TSTR>::value_type(0x011A,_T("画像の幅の解像度")));
		src.insert(std::map<ULONG,TSTR>::value_type(0x011B,_T("画像の高さの解像度")));
		src.insert(std::map<ULONG,TSTR>::value_type(0x0128,_T("画像の幅と高さの解像度の単位")));
		src.insert(std::map<ULONG,TSTR>::value_type(0x0131,_T("本体バージョン")));
		src.insert(std::map<ULONG,TSTR>::value_type(0x0132,_T("ファイル更新日時")));
		src.insert(std::map<ULONG,TSTR>::value_type(0x0213,_T("YCCの画素構成")));
		src.insert(std::map<ULONG,TSTR>::value_type(0x8769,_T("Exif IFDへのポインタ")));
		src.insert(std::map<ULONG,TSTR>::value_type(0xC4A5,_T("PrintIM Version-0300")));
		src.insert(std::map<ULONG,TSTR>::value_type(0x829A,_T("露出時間")));
		src.insert(std::map<ULONG,TSTR>::value_type(0x829D,_T("F値")));
		src.insert(std::map<ULONG,TSTR>::value_type(0x8822,_T("露出プログラム")));
		src.insert(std::map<ULONG,TSTR>::value_type(0x8827,_T("ISO感度")));
		src.insert(std::map<ULONG,TSTR>::value_type(0x9000,_T("Exifバージョン")));
		src.insert(std::map<ULONG,TSTR>::value_type(0x9003,_T("画像データの生成日時")));
		src.insert(std::map<ULONG,TSTR>::value_type(0x9004,_T("デジタルデータの生成日時")));
		src.insert(std::map<ULONG,TSTR>::value_type(0x9101,_T("各コンポーネントの意味")));
		src.insert(std::map<ULONG,TSTR>::value_type(0x9204,_T("露出補正値")));
		src.insert(std::map<ULONG,TSTR>::value_type(0x9205,_T("レンズ解放F値")));
		src.insert(std::map<ULONG,TSTR>::value_type(0x9207,_T("測光方式")));
		src.insert(std::map<ULONG,TSTR>::value_type(0x9208,_T("光源")));
		src.insert(std::map<ULONG,TSTR>::value_type(0x9209,_T("フラッシュ")));
		src.insert(std::map<ULONG,TSTR>::value_type(0x920A,_T("焦点距離")));
		src.insert(std::map<ULONG,TSTR>::value_type(0x927C,_T("メーカーノート")));
		src.insert(std::map<ULONG,TSTR>::value_type(0x9286,_T("ユーザーコメント")));
		src.insert(std::map<ULONG,TSTR>::value_type(0xA000,_T("対応FlashPixバージョン")));
		src.insert(std::map<ULONG,TSTR>::value_type(0xA001,_T("色空間情報")));
		src.insert(std::map<ULONG,TSTR>::value_type(0xA002,_T("幅")));
		src.insert(std::map<ULONG,TSTR>::value_type(0xA003,_T("高さ")));
		src.insert(std::map<ULONG,TSTR>::value_type(0xA005,_T("互換性IFDへのポインタ")));
		src.insert(std::map<ULONG,TSTR>::value_type(0xA300,_T("ファイルソース")));
		src.insert(std::map<ULONG,TSTR>::value_type(0xA401,_T("カスタムイメージ処理")));
		src.insert(std::map<ULONG,TSTR>::value_type(0xA402,_T("露出モード")));
		src.insert(std::map<ULONG,TSTR>::value_type(0xA403,_T("ホワイトバランス")));
		src.insert(std::map<ULONG,TSTR>::value_type(0xA404,_T("デジタルズーム倍率")));
		src.insert(std::map<ULONG,TSTR>::value_type(0xA406,_T("シーンキャプチャーモード")));
		src.insert(std::map<ULONG,TSTR>::value_type(0xA407,_T("ゲイン制御")));
		src.insert(std::map<ULONG,TSTR>::value_type(0xA408,_T("コントラスト")));
		src.insert(std::map<ULONG,TSTR>::value_type(0xA409,_T("彩度")));
		src.insert(std::map<ULONG,TSTR>::value_type(0xA40A,_T("シャープネス")));
		src.insert(std::map<ULONG,TSTR>::value_type(0x0001,_T("互換性識別子")));
		src.insert(std::map<ULONG,TSTR>::value_type(0x0002,_T("互換バージョン")));
		src.insert(std::map<ULONG,TSTR>::value_type(0x0103,_T("圧縮種類")));
		src.insert(std::map<ULONG,TSTR>::value_type(0x0201,_T("サムネイルへのポインタ")));
		src.insert(std::map<ULONG,TSTR>::value_type(0x0202,_T("サムネイルのバイト数")));
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
			初期化処理
		[Return]
			成功 : 1  Exif非対応、または画像ファイルでない : 0  失敗 : -1
	------------------------------------------------------------------------*/
	int Init()
	{
		{// メンバの初期化
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
		{// ファイルサイズ取得
			// ファイルを最後の位置まで移動させる
			fseek(fp,0,SEEK_END);
			//long型のftell()よりも、fpos_t型のfgetpos()を使用したほうが
			//const long sz = ftell(fp);
			fpos_t gsz = 0;
			fgetpos(fp,&gsz);
			sz = static_cast<long>(gsz);
			// 元の位置へ戻す
			fseek(fp,0,SEEK_SET);
			// あまりにもファイルサイズの小さいものは処理しない
			if(sz < 0xAF) return 0;
		}

		// サイズ分の領域を確保して取得
		std::vector<BYTE> buf(sz,0);
		const size_t read = (int)fread(&buf[0],sizeof(BYTE),sz,fp);
		fclose(fp);
		if(read == -1) return -1;
			
		// メンバへコピー
		Data = buf;

		// ファイルタイプの識別とバイトオーダーの取得
		const TSTR exif = CharJoint(6,9);
		TSTR byteorder = _T("");
		if(exif == _T("Exif"))
		{// Exif識別コードチェック
			byteorder = CharJoint(12,13);
			m_FileType = JPEG;
		}
		else
		{
			byteorder = CharJoint(0,1);
			m_FileType = RAW;
		}
		
		// バイトオーダを確認し、エンディアンを確定する
		if(byteorder == _T("II"))
			Endian = 0;
		else if(byteorder == _T("MM"))
			Endian = 1;
		else
			return 0;
		
		{// タグ取得
			std::vector<ExifTag> Field;
			const long ret = m_FileType==JPEG? GetField(Field,0x0C+BinJoint((ULONG)0x10,(ULONG)4),_0thIFD):GetField(Field,BinJoint((ULONG)0x04,(ULONG)4),_0thIFD,0x00);
			if(ret == -1)
			{// 1stIFDのみ取得
				MessageBox(NULL,_T("タグの取得に失敗しました"),_T("Exif Error"),MB_OK|MB_ICONERROR);
				return -1;
			}
			// 残りの全てのタグを取得
			Tags = m_FileType==RAW? GetExifTag(Field,0x00):GetExifTag(Field);
		}

		return 1;
	}

	
	/*------------------------------------------------------------------------
		[Function]
			CharJoint
		[Details]
			指定した範囲のバイナリデータを連結して文字列で取得する
		[Argument1] s
			開始点
		[Argument2] e
			終了
		[Return]
			連結後の文字列
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
			指定位置から指定バイト長のセグメント値を取得する
		[Argument1] start
			アクセス開始位置
		[Argument2] bytes
			バイト数
		[Return]
			セグメント値
	------------------------------------------------------------------------*/
	ULONG BinJoint(const ULONG& start, const ULONG& bytes)
	{
		ULONG dst = 0;
		// データサイズを超えたアクセスはできない
		if(start >= Data.size()) return dst;
		try
		{
			// 画像へのアクセスポインタ
			const BYTE* ptr = &Data[0];
			ptr += start;
			// ビッグエンディアンの場合は逆に取り出し、リトルエンディアンの場合は正方向に取り出す
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
			指定位置から指定バイト長のセグメント値を取得する(符号有り整数バージョン)
		[Argument1] start
			アクセス開始位置
		[Argument2] bytes
			バイト数
		[Return]
			セグメント値
	------------------------------------------------------------------------*/
	LONG BinJoint(const LONG& start, const LONG& bytes)
	{
		LONG dst = 0;
		// データサイズを超えたアクセスはできない
		if((ULONG)start >= Data.size()) return dst;
		try
		{
			// 画像へのアクセスポインタ
			const BYTE* ptr = &Data[0];
			ptr += start;
			// ビッグエンディアンの場合は逆に取り出し、リトルエンディアンの場合は正方向に取り出す
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
			IFDセグメントに格納されているエントリーフィールドを取得する
		[Argument1] dst
			格納先
		[Argument2] ptr
			IFD記述ポイントの先頭
		[Argument3] ifd
			IFDタイプ
		[Argument4] StdPos
			オフセットに対する基準位置
		[Return]
			成功すると格納に成功した個数が返る。致命的なエラーが発生した場合のみ-1が返る
	------------------------------------------------------------------------*/
	long GetField(std::vector<ExifTag>& dst, const ULONG& ptr, const IFD_TYPE& ifd, const ULONG StdPos = 0x0C)
	{
		try
		{
			ULONG IfdPtr = ptr;
			long ret = 0;
			while(1)
			{
				// IFDタグの個数
				const ULONG IfdCnt = BinJoint(IfdPtr,2);
				// 現実味の無いカウント数だったら止める
				if(IfdCnt > 118) break;
				// フィールドから取得する
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
				// 次のIFDがある場合
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
			Exifタグを全て取得する
		[Argument1] src
			更新データ
		[Argument2] StdPos
			オフセットに対する基準位置
		[Return]
			成功すると格納に成功した個数が返る。致命的なエラーが発生した場合のみ-1が返る
	------------------------------------------------------------------------*/
	std::vector<ExifTag> GetExifTag(const std::vector<ExifTag>& src, const ULONG StdPos = 0x0C)
	{
		std::vector<ExifTag> dst = src;
		// 通過したことを告げるフラグ
		BOOL flg = FALSE;
		for(UINT i=0; i<src.size(); i++)
		{// 目的のIFDタグを検索
			switch(src[i].Tag)
			{
			// Exif IFDへのポインタ
			case 0x8769:
				if(!ExifIfd){
					if(GetField(dst,src[i].Offset+StdPos,ExifIFD,StdPos) == -1) return dst;
					ExifIfd = TRUE;
				}
				flg = TRUE;
				break;
			// メーカーノート
			case 0x927C:
				if(!MakrNote){
					if(GetOlympusMakerNote(dst,src[i].Offset+StdPos) == -1) return dst;
					MakrNote = TRUE;
				}
				flg = TRUE;
				break;
			// 互換性IFDへのポインタ
			case 0xA005:
				if(!InterIfd){
					if(GetField(dst,src[i].Offset+StdPos,_1stIFD,StdPos) == -1) return dst;
					InterIfd = TRUE;
				}
				flg = TRUE;
				break;
			}
		}

		// 全て取得できたら終了
		if((ExifIfd && MakrNote && InterIfd) || !flg) return dst;
		if(flg){
			m_InfinityPrev++;
			if(m_InfinityPrev > 1000) return dst;
		}
		// 上記でタグを更新できた場合はさらに次のタグを探す
		return GetExifTag(dst,StdPos);
	}


	/*------------------------------------------------------------------------
		[Function]
			GetOffset
		[Details]
			タグのオフセットを所得する
		[Argument1] type
			データタイプ
		[Argument2] ptr
			記述場所の先頭ポインタ
		[Return]
			オフセット値
	------------------------------------------------------------------------*/
	ULONG GetOffset(const UINT& type, const ULONG& ptr)
	{
		// 4byteよりも容量が小さい整数型の場合は、記述場所の先頭からその型のバイト数分のみ読み込む(それより後ろのバイトは無効領域である)
		ULONG dst = 0;
		switch(type)
		{// 4byte未満の整数型のみ取得範囲制限
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
			タグ情報を文字列として所得する
		[Argument1] src
			フィールド情報
		[Argument2] StdPos
			オフセットに対する基準位置
		[Return]
			文字列で取得したタグ値
	------------------------------------------------------------------------*/
	TSTR GetTagStr(const ExifTag& src, const ULONG StdPos = 0x0C)
	{
		TSTR dst = _T("");
		// タグ読み出し位置
		const ULONG ptr = StdPos+src.Offset;
		switch(src.Type)
		{// データタイプに応じて分岐
		case _BYTE:
		case _SHORT:
		case _LONG:
		case _SBYTE:
		case _SSHORT:
		case _SLONG:
		case _IFD:
			// 整数型の場合はオフセット値をそのまま代入する
			dst = LToStr(src.Offset);
			break;
		case _ASCII:
			dst = CharJoint(ptr,ptr+src.Count);
			break;
		case _RATIONAL:
			dst = src.Tag==0x829A? GetRATIONAL(ptr,1):GetRATIONAL(ptr);
			// F値,シャッター速の場合は小数点第1位までを丸める
			if(src.Tag == 0x829D || src.Tag == 0x920A)
				dst = DToStr(RoundEx(StrToD(dst.data()),1),1);
			break;
		case _UNDEFINED:
			// メーカー依存の型(※要取得方法検討)
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
			データがDFLOATタイプのタグを文字列で取得する
		[Argument1] start
			データ記述場所
		[Return]
			文字列で取得した値
	------------------------------------------------------------------------*/
	TSTR GetDFLOAT(const ULONG& start)	
	{
		try
		{
			std::vector<char> bin(0,0);
			{// バイナリデータをchar配列として取り出す
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

			// 64bit以下は有り得ない
			if(bin.size() < 64) return _T("");

			// 符号部1 指数部11 仮数部52
			double dst = 0.0;
			{// double型を抽出
				const char* binptr = &bin[0];
				
				// 符号
				const double Sign = !binptr[0]? 1.0:-1.0;
				
				// 指数
				int Index = 0;
				// 指数の符号(FALSE:正 TRUE:負)
				const BOOL ISign = !binptr[1]? FALSE:TRUE;
				{
					int bshift = 10;
					for(int i=2; i<12; i++)
					{
						Index += (binptr[i]<<bshift);
						bshift--;
					}
				}
				
				// 仮数
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
			データがRATIONALタイプのタグを文字列で取得する
		[Argument1] start
			データ記述場所
		[Argument2] GetType
			取得形式を指定( 0:少数型  1:分数型 )
		[Return]
			文字列で取得した値(取得形式は少数型とする)
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
				dst = LToStr(Numerator) + _T("／") + LToStr(Denominator);
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
			データがSRATIONALタイプのタグを文字列で取得する
		[Argument1] start
			データ記述場所
		[Argument2] GetType
			取得形式を指定( 0:少数型  1:分数型 )
		[Return]
			文字列で取得した値
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
				dst = LToStr(Numerator) + _T("／") + LToStr(Denominator);
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
			オリンパスのメーカーノートを取得する
		[Argument1] dst
			格納先
		[Argument2] ptr
			IFD記述ポイントの先頭
		[Return]
			成功すると格納に成功した個数が返る。致命的なエラーが発生した場合のみ-1が返る
	------------------------------------------------------------------------*/
	long GetOlympusMakerNote(std::vector<ExifTag>& dst, const ULONG& ptr)
	{
		int EndianCopy = Endian;
		try
		{
			long ret = 0;
			
			// OLYMPUSのメーカーノートかチェック
			const TSTR olymptag = CharJoint(ptr,ptr+6);
			if(olymptag != _T("OLYMPUS")) return ret;
			
			{// バイトオーダーチェック
				int EndianTmp = -1;
				const TSTR byteorder = CharJoint(ptr+8,ptr+9);
				if(byteorder == _T("II"))
					EndianTmp = 0;
				else if(byteorder == _T("MM"))
					EndianTmp = 1;

				if(EndianTmp == -1) return ret;
				if(Endian != EndianTmp) Endian = EndianTmp;
			}

			// オフセットから何Byteか先の12バイトずつ以下のIFDへのオフセットが記述されているエリアを読み取る
			// ※このIFDは不明　InterOpera   = 0x00EF,
			const IFD_TYPE ifd[5] = { HardWareIFD,CameraModeIFD,ApplicationModeIFD,ImageProcessorIFD,DebugIFD };
			for(ULONG i=0; i<sizeof(ifd)/sizeof(ifd[0]); i++)
			{// IFD取得
				// JPEGの場合はメーカーノート開始地点から38byte目から順にIFDへのオフセットが記述されている。RAWの場合は50byte目
				// 従って、オフセット値のみ読み込むようにするにはJPEGの場合、46byte目(タグ2+タイプ2+カウント4で8つ先なので38+8=46)から先を12byteずつ参照すれば良い
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

