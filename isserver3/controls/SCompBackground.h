#pragma once

namespace SOUI
{
	class SCompBackground : public SWindow
	{
		SOUI_CLASS_NAME(SCompBackground, L"comp_bg")
	public:
		SCompBackground();
		~SCompBackground();

	protected:
		virtual CSize GetDesiredSize(int nParentWid, int nParentHei);

	};

}
