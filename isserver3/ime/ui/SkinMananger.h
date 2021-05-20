#pragma once

class CSkinMananger 
{
public:
	CSkinMananger();
	~CSkinMananger(void);

	void ClearMap();
	int InitSkinMenu(HMENU hmenu, const SStringT & strSkinPath, int nStartId, const SStringT & strCurSkin);
	bool ExtractSkinInfo(SStringT strSkinPath,SStringT & strDesc);
	SStringT SkinPathFromID(int nSkinID) const;
	SStringT SkinPathFromCtxID(int nCtxID) const;
	static bool ExtractSkinOffset(IResProvider *pResProvider,SkinInfo & info);
protected:
	SMap<int,SStringT> m_mapSkin;	//map of ID->skin path
	SMap<int,SStringT> m_mapCtxId2Path;//map of sub menu to path.
	int				   m_nMaxCtxID;
};
