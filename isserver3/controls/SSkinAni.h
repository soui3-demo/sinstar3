#pragma once

#include <interface/srender-i.h>
namespace SOUI
{
    class SSkinAni : public SSkinObjBase
    {
        SOUI_CLASS_NAME(SSkinAni,L"skinani")
    public:
        SSkinAni():m_nFrames(0),m_iFrame(0),m_bTile(FALSE),m_filterLevel(kLow_FilterLevel)
        {

        }

        virtual ~SSkinAni()
        {
        }

		virtual long GetFrameDelay(int iFrame=-1) = 0;

		virtual int LoadFromFile(LPCTSTR pszFileName)=0;

        virtual int LoadFromMemory(LPVOID pBits,size_t szData)=0;

		/**
        * ActiveNextFrame
        * @brief    激活下一帧
        * @return   void 
        * Describe  
        */    
        void ActiveNextFrame()
        {
            if(m_nFrames>1)
            {
                m_iFrame++;
                if(m_iFrame==m_nFrames) m_iFrame=0;
                SelectActiveFrame(m_iFrame);
            }
        }

        /**
        * SelectActiveFrame
        * @brief    激活指定帧
        * @param    int iFrame --  帧号
        * @return   void
        * Describe  
        */    
        void SelectActiveFrame(int iFrame) const
        {
            if(m_nFrames>1 && iFrame<m_nFrames)
            {
                m_iFrame = iFrame;
            }
        }

		LONG GetExpandCode() const{
			return MAKELONG(m_bTile?EM_TILE:EM_STRETCH,m_filterLevel);
		}
	public:
		SOUI_ATTRS_BEGIN()
			ATTR_RECT(L"margin",m_rcMargin,TRUE)
			ATTR_MARGIN(L"margin2",m_rcMargin,TRUE)
			ATTR_BOOL(L"tile",m_bTile,TRUE)
			ATTR_ENUM_BEGIN(L"filterLevel",FilterLevel,TRUE)
				ATTR_ENUM_VALUE(L"none",kNone_FilterLevel)
				ATTR_ENUM_VALUE(L"low",kLow_FilterLevel)
				ATTR_ENUM_VALUE(L"medium",kMedium_FilterLevel)
				ATTR_ENUM_VALUE(L"high",kHigh_FilterLevel)
			ATTR_ENUM_END(m_filterLevel)
		SOUI_ATTRS_END()
	protected:
		int m_nFrames;
        mutable int m_iFrame;
		CRect		m_rcMargin;
		FilterLevel	m_filterLevel;
		BOOL		m_bTile;
    };

}