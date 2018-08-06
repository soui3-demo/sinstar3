#pragma once
#include "SMutexView.h"

namespace SOUI
{
	class SStatusBackground : public SMutexView
	{
		SOUI_CLASS_NAME(SStatusBackground,L"status_bg")
	public:
		SStatusBackground(void);
		~SStatusBackground(void);

	protected:
		virtual CSize GetDesiredSize(int nParentWid, int nParentHei);

	};

}
