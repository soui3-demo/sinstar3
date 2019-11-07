#pragma once
#include <helper/SAdapterBase.h>
#include "../FindCaller/FindCaller.h"

namespace SOUI
{
	enum PTYPE{
		x86=0,
		x64,
		Unk,
		PTYPE_COUNT,
	};
	struct TASKINFOEX : TASKINFO
	{
		PTYPE type;
	};

	class CTaskAdapter : public SMcAdapterBase
	{
	public:
		CTaskAdapter();
		~CTaskAdapter();

		bool AddTask(TASKINFO ti, PTYPE type);
		TASKINFOEX* GetTask(int iTask);
		BOOL DelTask(int iTask);
		void RemoveAll();
	protected:
		virtual int getCount();

		virtual void getView(int position, SWindow * pItem, pugi::xml_node xmlTemplate);

		//删除一行，提供外部调用。
		void DeleteItem(int iPosition);

		SStringW GetColumnName(int iCol) const;

		bool OnSort(int iCol, SHDSORTFLAG * stFlags, int nCols);

		static int __cdecl SortCmp(void *context, const void * p1, const void * p2);
	protected:
		SArray<TASKINFOEX> m_arrTasks;
	};
}
