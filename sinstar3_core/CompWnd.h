#pragma once

#include "ImeWnd.h"

namespace SOUI
{
	class CCompWnd :
		public CImeWnd
	{
	public:
		CCompWnd(void);
		~CCompWnd(void);

		void SetCompStr(const SStringT &strComp);
		SStringT GetCompStr() const;

	protected:
		SStringT m_strComp;
	};

}
