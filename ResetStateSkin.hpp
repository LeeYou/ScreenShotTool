#ifndef __RESETSTATE_HPP_
#define __RESETSTATE_HPP_

#include "core\SSkin.h"

//************************************
// 这个 将 一张 图片 的 state 调整  
// 不会ps啊  从 别处弄来的 png  他的state 和 soui 的 不一样 所以才有了这个skin 来调整
//************************************
class SSkinResetState: public SSkinImgList
{
	SOUI_CLASS_NAME(SSkinResetState, L"resetstate")
public:
	SSkinResetState()
	{
		m_arrState[0] = 0;
		m_arrState[1] = 1;
		m_arrState[2] = 2;
		m_arrState[3] = 3;
		m_arrState[4] = 4;
		m_arrState[5] = 5;
		m_arrState[6] = 6;
		m_arrState[7] = 7;
	}
protected:
	void _Draw(IRenderTarget *pRT, LPCRECT rcDraw, DWORD dwState, BYTE byAlpha)
	{
		DWORD dw = m_arrState[dwState];
		SSkinImgList::_Draw(pRT, rcDraw, dw, byAlpha);
	}
	DWORD m_arrState[8];

	SOUI_ATTRS_BEGIN()
		ATTR_INT(L"state0", m_arrState[0], FALSE)  //
		ATTR_INT(L"state1", m_arrState[1], FALSE)  //
		ATTR_INT(L"state2", m_arrState[2], FALSE)  //
		ATTR_INT(L"state3", m_arrState[3], FALSE)  //
		ATTR_INT(L"state4", m_arrState[4], FALSE)  //
		ATTR_INT(L"state5", m_arrState[5], FALSE)  //
		ATTR_INT(L"state6", m_arrState[6], FALSE)  //
		ATTR_INT(L"state7", m_arrState[7], FALSE)  //
	SOUI_ATTRS_END()

};
//////////////////////////////////////////////////////////////////////////
#endif // __RESETSTATE_HPP_

