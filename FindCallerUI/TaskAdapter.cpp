#include "stdafx.h"
#include "TaskAdapter.h"

namespace SOUI {
	CTaskAdapter::CTaskAdapter()
	{
	}


	CTaskAdapter::~CTaskAdapter()
	{
	}

	bool CTaskAdapter::AddTask(TASKINFO ti, PTYPE type)
	{
		TASKINFOEX tiex;
		memcpy(&tiex, &ti, sizeof(ti));
		tiex.type = type;
		for(int i=0;i<m_arrTasks.GetCount();i++)
		{
			if(m_arrTasks[i].pid == ti.pid)
			{
				if(m_arrTasks[i].type == Unk && type!=Unk)
				{
					m_arrTasks[i]=tiex;
				}
				return false;
			}
		}
		m_arrTasks.Add(tiex);
		return true;
	}

	TASKINFOEX * CTaskAdapter::GetTask(int iTask)
	{
		if (iTask >= m_arrTasks.GetCount())
			return NULL;
		return &m_arrTasks[iTask];
	}

	BOOL CTaskAdapter::DelTask(int iTask)
	{
		if (iTask >= m_arrTasks.GetCount())
			return FALSE;
		m_arrTasks.RemoveAt(iTask);
		notifyDataSetChanged();
		return TRUE;
	}

	int CTaskAdapter::getCount()
	{
		return m_arrTasks.GetCount();
	}

	void CTaskAdapter::getView(int position, SWindow * pItem, pugi::xml_node xmlTemplate)
	{
		if (pItem->GetChildrenCount() == 0)
		{
			pItem->InitFromXml(xmlTemplate);
		}

		TASKINFOEX ti = m_arrTasks[position];
		static LPCTSTR szType[PTYPE_COUNT]={
			_T("x86"),
			_T("x64"),
			_T("unknown")
		};
		pItem->FindChildByID(R.id.txt_type)->SetWindowText(szType[ti.type]);
		pItem->FindChildByID(R.id.txt_pid)->SetWindowText(SStringT().Format(_T("%u"),ti.pid));
		pItem->FindChildByID(R.id.txt_name)->SetWindowText(ti.szName);
		pItem->FindChildByID(R.id.txt_path)->SetWindowText(ti.szPath);
	}

	//删除一行，提供外部调用。

	void CTaskAdapter::DeleteItem(int iPosition)
	{
		if (iPosition >= 0 && iPosition < getCount())
		{
			m_arrTasks.RemoveAt(iPosition);
			notifyDataSetChanged();
		}
	}

	SStringW CTaskAdapter::GetColumnName(int iCol) const {
		LPCWSTR KColNames[] = {
			L"col_type",
			L"col_pid",
			L"col_name",
			L"col_path",
		};
		return KColNames[iCol];
	}

	 bool CTaskAdapter::OnSort(int iCol, SHDSORTFLAG * stFlags, int nCols)
	{
		return true;
	}

	 int CTaskAdapter::SortCmp(void * context, const void * p1, const void * p2)
	{

		return 0;
	}

	 void CTaskAdapter::RemoveAll()
	 {
		 m_arrTasks.RemoveAll();
		 notifyDataSetChanged();
	 }

}

