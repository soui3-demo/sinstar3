#pragma once

#define MAX_SKINS	80
class CSkinMananger 
{
public:
	CSkinMananger();
	~CSkinMananger(void);

	void ClearMap(){m_mapSkin.RemoveAll();}
	int InitSkinMenu(HMENU hmenu, const SStringT & strSkinPath, int nStartId, const SStringT & strCurSkin);
	SStringT ExtractSkinInfo(SStringT strSkinPath);
	SStringT SkinPathFromID(int nSkinID) const;

	static CPoint ExtractSkinOffset(IResProvider *pResProvider);
protected:
	SMap<int,SStringT> m_mapSkin;	//map of ID->skin path
};
