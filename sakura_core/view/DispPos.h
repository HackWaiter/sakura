#pragma once

struct DispPos{
public:
	DispPos(int nDx,int nDy)
	: m_nDx(nDx)
	, m_nDy(nDy)
	{
		m_ptDrawOrigin.x=0;
		m_ptDrawOrigin.y=0;
		m_ptDrawLayout.x=CLayoutInt(0);
		m_ptDrawLayout.y=CLayoutInt(0);
		m_nLineRef=CLayoutInt(0);
	}

	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //
	//                         �`��ʒu                            //
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //
	//�Œ�l
	void InitDrawPos(const POINT& pt)
	{
		m_ptDrawOrigin=pt;
		m_ptDrawLayout.x=m_ptDrawLayout.y=CLayoutInt(0);
	}

	//�擾
	CMyPoint GetDrawPos() const
	{
		return CMyPoint(
			m_ptDrawOrigin.x + (Int)m_ptDrawLayout.x * m_nDx,
			m_ptDrawOrigin.y + (Int)m_ptDrawLayout.y * m_nDy
		);
	}

	//�i��
	void ForwardDrawCol (int nColOffset ){ m_ptDrawLayout.x += nColOffset; }
	void ForwardDrawLine(int nOffsetLine){ m_ptDrawLayout.y += nOffsetLine; }

	//���Z�b�g
	void ResetDrawCol(){ m_ptDrawLayout.x = CLayoutInt(0); }

	//�擾
	CLayoutInt GetDrawCol() const{ return m_ptDrawLayout.x; }
	CLayoutInt GetDrawLine() const{ return m_ptDrawLayout.y; }

	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //
	//                     �e�L�X�g�Q�ƈʒu                        //
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //

	//�ύX
	void SetLayoutLineRef(CLayoutInt nLogicLine){ m_nLineRef = nLogicLine; }
	void ForwardLayoutLineRef(int nOffsetLine){ m_nLineRef += CLayoutInt(nOffsetLine); }

	//�擾
	CLayoutInt GetLayoutLineRef() const{ return m_nLineRef; }


private:
	//�Œ�v�f
	int				m_nDx;			//���p�����̕����Ԋu�B�Œ�B
	int				m_nDy;			//���p�����̍s�Ԋu�B�Œ�B
	POINT			m_ptDrawOrigin;	//�`��ʒu��B�P�ʂ̓s�N�Z���B�Œ�B

	//�`��ʒu
	CLayoutPoint	m_ptDrawLayout; //�`��ʒu�B���΃��C�A�E�g�P�ʁB

	//�e�L�X�g�Q�ƈʒu
	CLayoutInt		m_nLineRef; //��΃��C�A�E�g�P�ʁB
};