#pragma once

#define MAX_SKINS	80
class CSkinMananger : public SObject
{
public:
	CSkinMananger(SEventSet *pEvtSets);
	~CSkinMananger(void);

	void ClearMap(){m_mapSkin.RemoveAll();}
	int InitSkinMenu(SMenuEx * hMenu, const SStringT & strSkinPath, int nStartId, const SStringT & strCurSkin);
	BOOL SetSkin(int nSkinId);
	SStringT ExtractSkinInfo(SStringT strSkinPath);
	static CPoint ExtractSkinOffset(IResProvider *pResProvider);
public:
	virtual int GetID() const
	{
		return SENDER_SKINMANAGER;
	}
protected:
	SMap<int,SStringT> m_mapSkin;	//map of ID->skin path
	SEventSet *m_pEvtSets;
};
