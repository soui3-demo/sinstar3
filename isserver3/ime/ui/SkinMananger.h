#pragma once

class CSkinMananger 
{
public:
	enum{
		MAX_SKINS=80,
	};
public:
	CSkinMananger();
	~CSkinMananger(void);

	void ClearMap(){m_mapSkin.RemoveAll();}
	int InitSkinMenu(HMENU hmenu, const SStringT & strSkinPath, int nStartId, const SStringT & strCurSkin);
	bool ExtractSkinInfo(SStringT strSkinPath,SStringT & strDesc);
	SStringT SkinPathFromID(int nSkinID) const;

	static CPoint ExtractSkinOffset(IResProvider *pResProvider);
protected:
	SMap<int,SStringT> m_mapSkin;	//map of ID->skin path
};
