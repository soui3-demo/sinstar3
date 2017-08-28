#pragma once

class EventSetSkin : public TplEventArgs<EventSetSkin>
{
	SOUI_CLASS_NAME(EventSetSkin,L"on_set_skin")
public:
	EventSetSkin(SObject *pSender):TplEventArgs<EventSetSkin>(pSender){}
	enum{EventID=EVENT_SETSKIN};
};

class CSkinMananger : public SObject
{
public:
	CSkinMananger(void);
	~CSkinMananger(void);

	void ClearMap(){m_mapSkin.RemoveAll();}
	BOOL SetSkin(int nSkinId);
	int InitSkinMenu(HMENU hMenu, const SStringT &strSkinPath,int nStartId);
	SStringT ExtractSkinInfo(SStringT strSkinPath);
public:
	virtual int GetID() const
	{
		return SENDER_SKINMANAGER;
	}
protected:
	SMap<int,SStringT> m_mapSkin;	//map of ID->skin path
};
