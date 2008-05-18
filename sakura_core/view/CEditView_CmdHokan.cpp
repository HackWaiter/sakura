/*!	@file
	@brief CEditView�N���X�̕⊮�֘A�R�}���h�����n�֐��Q

	@author genta
	@date	2005/01/10 �쐬
*/
/*
	Copyright (C) 1998-2001, Norio Nakatani
	Copyright (C) 2000, jepro
	Copyright (C) 2001, asa-o
	Copyright (C) 2003, Moca
	Copyright (C) 2004, Moca
	Copyright (C) 2005, genta

	This source code is designed for sakura editor.
	Please contact the copyright holders to use this code for other purpose.
*/
#include "stdafx.h"
#include "sakura_rc.h"
#include "view/CEditView.h"
#include "doc/CEditDoc.h"
#include "debug/Debug.h"
#include "charset/charcode.h"  // 2006.06.28 rastiv
#include "window/CEditWnd.h"

/*!
	@brief �R�}���h��M�O�⊮����
	
	�⊮�E�B���h�E�̔�\��

	@date 2005.01.10 genta �֐���
*/
void CEditView::PreprocessCommand_hokan( int nCommand )
{
	/* �⊮�E�B���h�E���\������Ă���Ƃ��A���ʂȏꍇ�������ăE�B���h�E���\���ɂ��� */
	if( m_bHokan ){
		if( nCommand != F_HOKAN		//	�⊮�J�n�E�I���R�}���h
		 && nCommand != F_WCHAR		//	��������
		 && nCommand != F_IME_CHAR	//	��������
		 ){
			m_pcEditWnd->m_cHokanMgr.Hide();
			m_bHokan = FALSE;
		}
	}
}

/*!
	�R�}���h���s��⊮����

	@author Moca
	@date 2005.01.10 genta �֐���
*/
void CEditView::PostprocessCommand_hokan(void)
{
	if( GetDllShareData().m_Common.m_sHelper.m_bUseHokan && !m_bExecutingKeyMacro ){ /* �L�[�{�[�h�}�N���̎��s�� */
		CNativeW	cmemData;

		/* �J�[�\�����O�̒P����擾 */
		if( 0 < GetParser().GetLeftWord( &cmemData, 100 ) ){
			ShowHokanMgr( cmemData, FALSE );
		}else{
			if( m_bHokan ){
				m_pcEditWnd->m_cHokanMgr.Hide();
				m_bHokan = FALSE;
			}
		}
	}
}

/*!	�⊮�E�B���h�E��\������
	�E�B���h�E��\��������́AHokanMgr�ɔC����̂ŁAShowHokanMgr�̒m��Ƃ���ł͂Ȃ��B
	
	@param cmemData [in] �⊮���錳�̃e�L�X�g �uAb�v�Ȃǂ�����B
	@param bAutoDecided [in] ��₪1��������m�肷��

	@date 2005.01.10 genta CEditView_Command����ړ�
*/
void CEditView::ShowHokanMgr( CNativeW& cmemData, BOOL bAutoDecided )
{
	/* �⊮�Ώۃ��[�h���X�g�𒲂ׂ� */
	CNativeW	cmemHokanWord;
	int			nKouhoNum;
	POINT		poWin;
	/* �⊮�E�B���h�E�̕\���ʒu���Z�o */
	poWin.x = GetTextArea().GetAreaLeft()
			 + (Int)(GetCaret().GetCaretLayoutPos().GetX2() - GetTextArea().GetViewLeftCol())
			  * GetTextMetrics().GetHankakuDx();
	poWin.y = GetTextArea().GetAreaTop()
			 + (Int)(GetCaret().GetCaretLayoutPos().GetY2() - GetTextArea().GetViewTopLine())
			  * GetTextMetrics().GetHankakuDy();
	this->ClientToScreen( &poWin );
	poWin.x -= (
		cmemData.GetStringLength()
		 * GetTextMetrics().GetHankakuDx()
	);

	/*	�⊮�E�B���h�E��\��
		�������AbAutoDecided == TRUE�̏ꍇ�́A�⊮��₪1�̂Ƃ��́A�E�B���h�E��\�����Ȃ��B
		�ڂ����́ASearch()�̐������Q�Ƃ̂��ƁB
	*/
	CNativeW* pcmemHokanWord;
	if ( bAutoDecided ){
		pcmemHokanWord = &cmemHokanWord;
	}
	else {
		pcmemHokanWord = NULL;
	}
	nKouhoNum = m_pcEditWnd->m_cHokanMgr.CHokanMgr::Search(
		&poWin,
		GetTextMetrics().GetHankakuHeight(),
		GetTextMetrics().GetHankakuDx(),
		cmemData.GetStringPtr(),
		m_pcEditDoc->m_cDocType.GetDocumentAttribute().m_szHokanFile,
		m_pcEditDoc->m_cDocType.GetDocumentAttribute().m_bHokanLoHiCase,
		m_pcEditDoc->m_cDocType.GetDocumentAttribute().m_bUseHokanByFile, // 2003.06.22 Moca
		pcmemHokanWord
	);
	/* �⊮���̐��ɂ���ē����ς��� */
	if (nKouhoNum <= 0) {				//	��△��
		if( m_bHokan ){
			m_pcEditWnd->m_cHokanMgr.Hide();
			m_bHokan = FALSE;
			// 2003.06.25 Moca ���s���Ă���A�r�[�v�����o���ĕ⊮�I���B
			ErrorBeep();
		}
	}
	else if( bAutoDecided && nKouhoNum == 1){ //	���1�̂݁��m��B
		if( m_bHokan ){
			m_pcEditWnd->m_cHokanMgr.Hide();
			m_bHokan = FALSE;
		}
		// 2004.05.14 Moca CHokanMgr::Search���ŉ��s���폜����悤�ɂ��A���ڏ���������̂���߂�
//		pszKouhoWord = cmemHokanWord.GetPtr( &nKouhoWordLen );
//		pszKouhoWord[nKouhoWordLen] = L'\0';
		GetCommander().Command_WordDeleteToStart();
		GetCommander().Command_INSTEXT( TRUE, cmemHokanWord.GetStringPtr(), cmemHokanWord.GetStringLength(), TRUE );
	}
	else {
		m_bHokan = TRUE;
	}
	
	//	�⊮�I���B
	if ( !m_bHokan ){
		GetDllShareData().m_Common.m_sHelper.m_bUseHokan = FALSE;	//	���͕⊮�I���̒m�点
	}
}



/*!
	�ҏW���f�[�^������͕⊮�L�[���[�h�̌���
	CHokanMgr����Ă΂��

	@return ��␔

	@author Moca
	@date 2003.06.25

	@date 2005/01/10 genta  CEditView_Command����ړ�
	@date 2007/10/17 kobake �ǂ݂₷���悤�Ƀl�X�g��󂭂��܂����B
*/
int CEditView::HokanSearchByFile(
	const wchar_t*	pszKey,			//!< [in]
	BOOL			bHokanLoHiCase,	//!< [in] �p�啶���������𓯈ꎋ����
	CNativeW**		ppcmemKouho,	//!< [in/out] ���
	int				nKouhoNum,		//!< [in] ppcmemKouho�̂��łɓ����Ă��鐔
	int				nMaxKouho		//!< [in] Max��␔(0==������)
){
	const int nKeyLen = wcslen( pszKey );
	int nLines = m_pcEditDoc->m_cDocLineMgr.GetLineCount();
	int j, nWordLen, nLineLen, nRet;

	const wchar_t* pszLine;
	const wchar_t* word;

	CLogicPoint ptCur = GetCaret().GetCaretLogicPos(); //�����J�[�\���ʒu
	
	for( CLogicInt i = CLogicInt(0); i < nLines; i++  ){
		pszLine = CDocReader(m_pcEditDoc->m_cDocLineMgr).GetLineStrWithoutEOL( i, &nLineLen );
		for( j = 0; j < nLineLen; j++ ){
			//
			if( !IS_KEYWORD_CHAR( pszLine[j] ) )continue;

			//
			word = pszLine + j;
			for( j++, nWordLen = 1;j < nLineLen && IS_KEYWORD_CHAR( pszLine[j] ); j++ ){
				nWordLen++;
			}

			//
			if( nWordLen > 1020 ){ // CDicMgr���̐����ɂ�蒷������P��͖�������
				continue;
			}
			if( nKeyLen > nWordLen )continue;

			//
			if( bHokanLoHiCase ){
				nRet = auto_memicmp( pszKey, word, nKeyLen );
			}else{
				nRet = auto_memcmp( pszKey, word, nKeyLen );
			}
			if( nRet!=0 )continue;

			// �J�[�\���ʒu�̒P��͌�₩��͂���
			if( ptCur.y == i && ptCur.x <= j && j - nWordLen <= ptCur.x ){
				continue;
			}

			if( NULL == *ppcmemKouho ){
				*ppcmemKouho = new CNativeW;
				(*ppcmemKouho)->SetString( word, nWordLen );
				(*ppcmemKouho)->AppendString( L"\n" );
				++nKouhoNum;
			}
			else{
				// �d�����Ă�����ǉ����Ȃ�
				int nLen;
				const wchar_t* ptr = (*ppcmemKouho)->GetStringPtr( &nLen );
				int nPosKouho;
				nRet = 1;
				if( bHokanLoHiCase ){
					if( nWordLen < nLen ){
						if( L'\n' == ptr[nWordLen] && 0 == auto_memicmp( ptr, word, nWordLen )  ){
							nRet = 0;
						}
						else{
							int nPosKouhoMax = nLen - nWordLen - 1;
							for( nPosKouho = 1; nPosKouho < nPosKouhoMax; nPosKouho++ ){
								if( ptr[nPosKouho] == L'\n' ){
									if( ptr[nPosKouho + nWordLen + 1] == L'\n' ){
										if( 0 == auto_memicmp( &ptr[nPosKouho + 1], word, nWordLen) ){
											nRet = 0;
											break;
										}else{
											nPosKouho += nWordLen;
										}
									}
								}
							}
						}
					}
				}else{
					if( nWordLen < nLen ){
						if( L'\n' == ptr[nWordLen] && 0 == auto_memcmp( ptr, word, nWordLen )  ){
							nRet = 0;
						}else{
							int nPosKouhoMax = nLen - nWordLen - 1;
							for( nPosKouho = 1; nPosKouho < nPosKouhoMax; nPosKouho++ ){
								if( ptr[nPosKouho] == L'\n' ){
									if( ptr[nPosKouho + nWordLen + 1] == L'\n' ){
										if( 0 == auto_memcmp( &ptr[nPosKouho + 1], word, nWordLen) ){
											nRet = 0;
											break;
										}else{
											nPosKouho += nWordLen;
										}
									}
								}
							}
						}
					}
				}
				if( 0 == nRet ){
					continue;
				}
				//2007.10.17 kobake ���������[�N���Ă܂����B�C���B
				(*ppcmemKouho)->AppendString( word, nWordLen );
				(*ppcmemKouho)->AppendString( L"\n" );
				++nKouhoNum;
			}
			if( 0 != nMaxKouho && nMaxKouho <= nKouhoNum ){
				return nKouhoNum;
			}
			
		}
	}
	return nKouhoNum;
}
