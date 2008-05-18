#include "stdafx.h"
#include "charset/charcode.h"



namespace WCODE
{
	bool CalcHankakuByFont(wchar_t);

	//2007.08.30 kobake �ǉ�
	bool IsHankaku(wchar_t wc)
	{
		//���قږ����؁B���W�b�N���m�肵����C�����C��������Ɨǂ��B

		//�Q�l�Fhttp://www.swanq.co.jp/blog/archives/000783.html
		if(
			   wc<=0x007E //ACODE�Ƃ�
//			|| wc==0x00A5 //�~�}�[�N
//			|| wc==0x203E //�ɂ��
			|| (wc>=0xFF61 && wc<=0xFF9f)	// ���p�J�^�J�i
		)return true;

		//0x7F �` 0xA0 �����p�Ƃ݂Ȃ�
		//http://ja.wikipedia.org/wiki/Unicode%E4%B8%80%E8%A6%A7_0000-0FFF �����āA�Ȃ�ƂȂ�
		if(wc>=0x007F && wc<=0x00A0)return true;	// Control Code ISO/IEC 6429

		// �����͑S�p�Ƃ݂Ȃ�
		if ( wc>=0x4E00 && wc<=0x9FBB		// Unified Ideographs, CJK
		  || wc>=0x3400 && wc<=0x4DB5		// Unified Ideographs Extension A, CJK
		  || wc>=0xAC00 && wc<=0xD7A3		// 	Hangul Syllables
		) return false;	// Private Use Area

		// �O���͑S�p�Ƃ݂Ȃ�
		if (wc>=0xE000 && wc<=0xE8FF)	return false;	// Private Use Area

		//$$ ���B�������I�Ɍv�Z�����Ⴆ�B(����̂�)
		return CalcHankakuByFont(wc);
	}

	//!���䕶���ł��邩�ǂ���
	bool IsControlCode(wchar_t wc)
	{
		//���s�͐��䕶���Ƃ݂Ȃ��Ȃ�
		if(IsLineDelimiter(wc))return false;

		//�^�u�͐��䕶���Ƃ݂Ȃ��Ȃ�
		if(wc==TAB)return false;

		return iswcntrl(wc)!=0;
	}

	/*!
		��Ǔ_��
		2008.04.27 kobake CLayoutMgr::IsKutoTen ���番��

		@param[in] c1 ���ׂ镶��1�o�C�g��
		@param[in] c2 ���ׂ镶��2�o�C�g��
		@retval true ��Ǔ_�ł���
		@retval false ��Ǔ_�łȂ�
	*/
	bool IsKutoten( wchar_t wc )
	{
		//��Ǔ_��`
		static const wchar_t *KUTOTEN=
			L"��,."
			L"�B�A�C�D"
		;

		const wchar_t* p;
		for(p=KUTOTEN;*p;p++){
			if(*p==wc)return true;
		}
		return false;
	}


/*!
	UNICODE�������̃L���b�V���N���X�B
	1����������2�r�b�g�ŁA�l��ۑ����Ă����B
	00:��������
	01:���p
	10:�S�p
	11:-
*/
class LocalCache{
public:
	LocalCache()
	{
		/* LOGFONT�̏����� */
		memset( &m_lf, 0, sizeof(m_lf) );

		// HDC �̏�����
		HDC hdc=GetDC(NULL);
		m_hdc = CreateCompatibleDC(hdc);
		ReleaseDC(NULL, hdc);

		m_hFont =NULL;
		m_hfntOld =NULL;
	}
	~LocalCache()
	{
		// -- -- ��n�� -- -- //
		if (m_hFont != NULL) {
			SelectObject(m_hdc, m_hfntOld);
			DeleteObject(m_hFont);
		}
		DeleteDC(m_hdc);
	}
	// �ď�����
	void Init( const LOGFONT &lf )
	{
		if (m_hfntOld != NULL) {
			m_hfntOld = (HFONT)SelectObject(m_hdc, m_hfntOld);
			DeleteObject(m_hfntOld);
		}

		m_lf = lf;

		m_hFont = ::CreateFontIndirect( &lf );
		m_hfntOld = (HFONT)SelectObject(m_hdc,m_hFont);

		// -- -- ���p� -- -- //
		GetTextExtentPoint32W_AnyBuild(m_hdc,L"x",1,&m_han_size);
	}
	void SetCache(wchar_t c, bool cache_value)
	{
		int v=cache_value?0x1:0x2;
		GetDllShareData().m_bCharWidthCache[c/4] &= ~( 0x3<< ((c%4)*2) ); //�Y���ӏ��N���A
		GetDllShareData().m_bCharWidthCache[c/4] |=  ( v  << ((c%4)*2) ); //�Y���ӏ��Z�b�g
	}
	bool GetCache(wchar_t c) const
	{
		return _GetRaw(c)==0x1?true:false;
	}
	bool ExistCache(wchar_t c) const
	{
		assert(GetDllShareData().m_nCharWidthCacheTest==0x12345678);
		return _GetRaw(c)!=0x0;
	}
	bool CalcHankakuByFont(wchar_t c)
	{
		SIZE size={m_han_size.cx*2,0}; //�֐������s�����Ƃ��̂��Ƃ��l���A�S�p���ŏ��������Ă���
		GetTextExtentPoint32W_AnyBuild(m_hdc,&c,1,&size);
		return (size.cx<=m_han_size.cx);
	}
protected:
	int _GetRaw(wchar_t c) const
	{
		return (GetDllShareData().m_bCharWidthCache[c/4]>>((c%4)*2))&0x3;
	}
private:
	HDC m_hdc;
	HFONT m_hfntOld;
	HFONT m_hFont;
	SIZE m_han_size;
	LOGFONT	m_lf;							// 2008/5/15 Uchi
};

static LocalCache cache;

//�������̓��I�v�Z�B���p�Ȃ�true�B
bool CalcHankakuByFont(wchar_t c)
{
	// -- -- �L���b�V�������݂���΁A��������̂܂ܕԂ� -- -- //
	if(cache.ExistCache(c))return cache.GetCache(c);

	// -- -- ���Δ�r -- -- //
	bool value;;
	value = cache.CalcHankakuByFont(c);

	// -- -- �L���b�V���X�V -- -- //
	cache.SetCache(c,value);

	return cache.GetCache(c);
}
}

//	�������̓��I�v�Z�p�L���b�V���̏������B	2007/5/18 Uchi
void InitCharWidthCache( const LOGFONT &lf )
{
	WCODE::cache.Init(lf);
}

//	�������̓��I�v�Z�p�L���b�V���̏������B	2007/5/18 Uchi
void InitCharWidthCacheCommon()
{
	// �L���b�V���̃N���A
	memset(GetDllShareData().m_bCharWidthCache, 0, sizeof(GetDllShareData().m_bCharWidthCache));
	GetDllShareData().m_nCharWidthCacheTest=0x12345678;
}