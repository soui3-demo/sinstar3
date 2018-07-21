#pragma once

#define CMD_MENU_DEF	 220


class CSkinMananger : public SObject
{
public:
	CSkinMananger(void);
	~CSkinMananger(void);

	void ClearMap(){m_mapSkin.RemoveAll();}
	int InitSkinMenu(SMenuEx * hMenu, const SStringT & strSkinPath, int nStartId, const SStringT & strCurSkin);
	BOOL SetSkin(int nSkinId);
	int InitSkinMenu(HMENU hMenu, const SStringT &strSkinPath,int nStartId,const SStringT &strCurSkin);
	SStringT ExtractSkinInfo(SStringT strSkinPath);
public:
	virtual int GetID() const
	{
		return SENDER_SKINMANAGER;
	}
protected:
	SMap<int,SStringT> m_mapSkin;	//map of ID->skin path
};
