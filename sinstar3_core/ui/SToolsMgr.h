#pragma once
class SToolsMgr
{
public:
	SToolsMgr();
	~SToolsMgr();

	void ClearMap() { m_mapTool.RemoveAll(); }
	int InitToolMenu(SMenuEx * hMenu, const SStringT & strToolPath, int nStartId);
	SStringT ExtractToolInfo(SStringT strToolPath);
	SStringT ToolPathFromID(int nToolID) const;

protected:
	SMap<int, SStringT> m_mapTool;	//map of ID->tool path

};

