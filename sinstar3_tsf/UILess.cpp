#include "stdafx.h"
#include "UILess.h"
#include "EditSession.h"
/* ITfEditSession */
/* Inline Preedit */
class CInlinePreeditEditSession : public CEditSessionBase
{
public:
	CInlinePreeditEditSession(CSinstar3Tsf* pTextService, ITfContext* pContext)
		: CEditSessionBase(pTextService, pContext)
	{
	}

	/* ITfEditSession */
	STDMETHODIMP DoEditSession(TfEditCookie ec);

private:
};

BOOL UILess::_ShowInlinePreedit(CSinstar3Tsf* pTextService, DWORD _tfClientId, SOUI::SComPtr<ITfContext> pContext) {
	SOUI::SComPtr<CInlinePreeditEditSession> pEditSession;
	pEditSession.Attach(new CInlinePreeditEditSession(pTextService, pContext));
	if (pEditSession != NULL)
	{
		HRESULT hr;
		pContext->RequestEditSession(_tfClientId, pEditSession, TF_ES_ASYNCDONTCARE | TF_ES_READWRITE, &hr);
	}
	return TRUE;
}
STDMETHODIMP CInlinePreeditEditSession::DoEditSession(TfEditCookie ec)
{
	//输入字符串绘制inline
	std::wstring preedit = GetTextService()->GetPeeditString();

	SOUI::SComPtr<ITfRange> pRangeComposition;
	SOUI::SComPtr<ITfComposition> _pComposition = _pTextService->GetITfComposition();

	if (_pComposition == NULL) return E_FAIL;
	if ((_pComposition->GetRange(&pRangeComposition)) != S_OK)
		return E_FAIL;

	if ((pRangeComposition->SetText(ec, 0, preedit.c_str(), preedit.length())) != S_OK)
		return E_FAIL;

	int sel_start = 0, sel_end = preedit.length(); /* TODO: Check the availability and correctness of these values */

	/* Set caret */
	LONG cch;
	TF_SELECTION tfSelection;
	pRangeComposition->Collapse(ec, TF_ANCHOR_START);
	pRangeComposition->ShiftEnd(ec, sel_end, &cch, NULL);
	pRangeComposition->ShiftStart(ec, sel_start, &cch, NULL);
	tfSelection.range = pRangeComposition;
	tfSelection.style.ase = TF_AE_NONE;
	tfSelection.style.fInterimChar = FALSE;

	
	TfGuidAtom  gaDisplayAttribute = _pTextService->GetDisplayAttribInfo();	
	if (TF_INVALID_GUIDATOM != gaDisplayAttribute)
	{
		SOUI::SComPtr<ITfProperty> pDisplayAttributeProperty;
		if (SUCCEEDED(_pContext->GetProperty(GUID_PROP_ATTRIBUTE, &pDisplayAttributeProperty)))
		{
			VARIANT var;
			VariantInit(&var);
			//All display attributes are TfGuidAtoms and TfGuidAtoms are VT_I4. 
			var.vt = VT_I4;
			var.lVal = gaDisplayAttribute;
			//Set the display attribute value over the range. 
			pDisplayAttributeProperty->SetValue(ec, tfSelection.range, &var);
		}
	}

	_pContext->SetSelection(ec, 1, &tfSelection);

	return S_OK;
}

SOUI::SComPtr<ITfContext> CCandidateList::GetContextDocument()
{
	return SOUI::SComPtr<ITfContext>();
}

//HRESULT __stdcall CCandidateList::GetContext(ITfContext** ppic)
//{
//	return E_NOTIMPL;
//}
//
//HRESULT __stdcall CCandidateList::GetString(BSTR* pstr)
//{
//	auto str = SysAllocString(L"我是谁？");
//	if (pstr)
//	{
//		*pstr = str;
//	}
//	return S_OK;
//}
//
//HRESULT __stdcall CCandidateList::GetMaxReadingStringLength(UINT* pcchMax)
//{
//	if (pcchMax)
//		return E_INVALIDARG;
//	*pcchMax = 10;
//	return S_OK;
//}
//
//HRESULT __stdcall CCandidateList::GetErrorIndex(UINT* pErrorIndex)
//{
//	if (pErrorIndex)
//		return E_INVALIDARG;
//	*pErrorIndex = 10;
//	return S_OK;
//}
//
//HRESULT __stdcall CCandidateList::IsVerticalOrderPreferred(BOOL* pfVertical)
//{
//	if (pfVertical)
//		return E_INVALIDARG; 
//	*pfVertical = TRUE;
//	return S_OK;
//}

CCandidateList::CCandidateList(CSinstar3Tsf* pTextService) :
	_tsf(pTextService),
	_ui_id(TF_INVALID_UIELEMENTID),
	_pbShow(FALSE),
	_changed_flags(0)
{
	_ctx.cinfo.highlighted = 0;
	_ctx.cinfo.currentPage = 0;
	_ctx.cinfo.candies.push_back(Text(L"启程输入法0"));
	_ctx.preedit = Text(L"qic");
}

CCandidateList::~CCandidateList()
{
}

std::wstring GuidToString(const GUID& guid)
{
	wchar_t buf[64] = { 0 };
	swprintf_s(
		buf,
		LR"({%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X})",
		guid.Data1, guid.Data2, guid.Data3,
		guid.Data4[0], guid.Data4[1],
		guid.Data4[2], guid.Data4[3],
		guid.Data4[4], guid.Data4[5],
		guid.Data4[6], guid.Data4[7]);
	return std::wstring(buf);
}

STDMETHODIMP CCandidateList::QueryInterface(REFIID riid, void** ppvObj)
{
	if (ppvObj == nullptr)
	{
		return E_INVALIDARG;
	}

	*ppvObj = nullptr;

	if (IsEqualIID(riid, IID_ITfUIElement) ||
		IsEqualIID(riid, IID_ITfCandidateListUIElement) ||
		IsEqualIID(riid, IID_ITfCandidateListUIElementBehavior)
		)
	{
		*ppvObj = (ITfCandidateListUIElementBehavior*)this;
	}
#if WINVER>= 0x0602
	else if (IsEqualIID(riid, __uuidof(ITfFnSearchCandidateProvider)))
	{
		*ppvObj = (ITfFnSearchCandidateProvider*)this;
	}
	else if (IsEqualIID(riid, IID_IUnknown) ||
		IsEqualIID(riid, __uuidof(ITfIntegratableCandidateListUIElement)))
	{
		*ppvObj = (ITfIntegratableCandidateListUIElement*)this;
	}
#endif
	if (*ppvObj)
	{
		std::wstring outlogstr = _T("UILess::QueryInterface INTERFACE GUID------");
		outlogstr += GuidToString(riid);
		SLOGFMTI(outlogstr.c_str());
		AddRef();
		return S_OK;
	}
	else//请求了不支持的IID。记录下来。
	{
		std::wstring outlogstr = _T("UILess::QueryInterface NOINTERFACE GUID------");
		outlogstr += GuidToString(riid);
		SLOGFMTI(outlogstr.c_str());
	}

	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CCandidateList::AddRef(void)
{
	return ++_cRef;
}

STDMETHODIMP_(ULONG) CCandidateList::Release(void)
{
	LONG cr = --_cRef;

	assert(_cRef >= 0);

	if (_cRef == 0)
	{
		delete this;
	}

	return cr;
}

STDMETHODIMP CCandidateList::GetDescription(BSTR* pbstr)
{
	static auto str = SysAllocString(L"Sinstar Candidate List");
	if (pbstr)
	{
		*pbstr = str;
	}
	return S_OK;
}

STDMETHODIMP CCandidateList::GetGUID(GUID* pguid)
{
	// {0EEC72CF-711A-443A-A403-FF8CAFCD9AC0}
	*pguid = { 0xeec72cf, 0x711a, 0x443a, { 0xa4, 0x3, 0xff, 0x8c, 0xaf, 0xcd, 0x9a, 0xc0 } };
	return S_OK;
}

STDMETHODIMP CCandidateList::Show(BOOL showCandidateWindow)
{
	SLOGFMTI("UILess::Show :%s", showCandidateWindow ? "TRUE" : "FALSE");
	return E_NOTIMPL;
}

STDMETHODIMP CCandidateList::IsShown(BOOL* pIsShow)
{
	SLOGFMTI("UILess::IsShown");
	if (!pIsShow) return E_INVALIDARG;
	// If _ui_id is valid, means we are showing a candidate ui. show_ui_ means we
	// should show the ui via text service's own window.
	*pIsShow = ((_ui_id != TF_INVALID_UIELEMENTID) && _pbShow);
	return S_OK;
}

STDMETHODIMP CCandidateList::GetUpdatedFlags(DWORD* pdwFlags)
{
	if (!pdwFlags)
		return E_INVALIDARG;

	*pdwFlags = _changed_flags;

	// TSF call GetUpdatedFlags to ask what is changed after the previous call.
	// We clear the flags after TSF got the changed information, so that the next
	// call to this function will return the changes from now on.
	_changed_flags = 0;

	return S_OK;
}

STDMETHODIMP CCandidateList::GetDocumentMgr(ITfDocumentMgr** ppdim)
{
	*ppdim = nullptr;
	ITfThreadMgr* pThreadMgr = _tsf->_GetThreadMgr();
	if (pThreadMgr == nullptr)
	{
		return E_FAIL;
	}
	if (FAILED(pThreadMgr->GetFocus(ppdim)) || (*ppdim == nullptr))
	{
		return E_FAIL;
	}
	return S_OK;
}

STDMETHODIMP CCandidateList::GetCount(UINT* pCandidateCount)
{
	SLOGFMTI("UILess::GetCount Current Count:%d", _ctx.cinfo.candies.size());
	if (!pCandidateCount)
		return E_INVALIDARG;
	*pCandidateCount = (UINT)_ctx.cinfo.candies.size();
	return S_OK;
}

STDMETHODIMP CCandidateList::GetSelection(UINT* pSelectedCandidateIndex)
{
	SLOGFMTI("UILess::GetSelection Current Selection:%d", _ctx.cinfo.highlighted);
	if (!pSelectedCandidateIndex)
		return E_INVALIDARG;
	*pSelectedCandidateIndex = _ctx.cinfo.highlighted;
	return S_OK;
}

STDMETHODIMP CCandidateList::GetString(UINT uIndex, BSTR* pbstr)
{
	SLOGFMTI("UILess::GetString uIndex:%d", uIndex);
	if (!pbstr)
		return E_INVALIDARG;

	*pbstr = nullptr;
	auto& cinfo = _ctx.cinfo;
	if (uIndex >= cinfo.candies.size())
		return E_INVALIDARG;
	auto& str = cinfo.candies[uIndex].str;
	*pbstr = SysAllocString(str.c_str());
	SLOGFMTI(L"UILess::GetString uIndex:%d,bstr:%s", uIndex, *pbstr);
	return S_OK;
}

STDMETHODIMP CCandidateList::GetPageIndex(UINT* pIndex, UINT uSize, UINT* puPageCnt)
{
	//uSize =pIndex的大小， 调用方先以pIndex==NULL调用来获取puPageCnt中的页数
	//每页大小
#define PAGESIZE 5

	SLOGFMTI("UILess::GetPageIndex uSize:%d", uSize);
	if (!puPageCnt)
		return E_INVALIDARG;
	auto& cinfo = _ctx.cinfo;
	*puPageCnt = cinfo.candies.size() / PAGESIZE + (cinfo.candies.size() % PAGESIZE ? 1 : 0);
	if (pIndex) {
		if (uSize < *puPageCnt) {
			return E_INVALIDARG;
		}
		for (UINT i = 0; i < uSize; i++)
		{
			pIndex[i] = i * PAGESIZE;
		}
	}
	return S_OK;
}

STDMETHODIMP CCandidateList::SetPageIndex(UINT* pIndex, UINT uPageCnt)
{
	SLOGFMTI("UILess::SetPageIndex");
	if (!pIndex)
		return E_INVALIDARG;
	return S_OK;
}

STDMETHODIMP CCandidateList::GetCurrentPage(UINT* puPage)
{
	SLOGFMTI("UILess::GetCurrentPage");
	if (!puPage)
		return E_INVALIDARG;
	*puPage = _ctx.cinfo.currentPage;
	return S_OK;
}

STDMETHODIMP CCandidateList::SetSelection(UINT nIndex)
{
	SLOGFMTI("UILess::SetSelection");
	return S_OK;
}

STDMETHODIMP CCandidateList::Finalize(void)
{
	SLOGFMTI("UILess::Finalize");
	ITfContext* pCtx = _tsf->GetImeContext();
	if (pCtx)
	{
		_tsf->_EndComposition(pCtx, true);
	}
	return S_OK;
}

STDMETHODIMP CCandidateList::Abort(void)
{
	SLOGFMTI("UILess::Abort");
	ITfContext* pCtx = _tsf->GetImeContext();
	if (pCtx)
	{
		_tsf->_EndComposition(pCtx, false);
	}
	return S_OK;
}

#if WINVER>= 0x0602

STDMETHODIMP CCandidateList::SetIntegrationStyle(GUID guidIntegrationStyle)
{
	SLOGFMTI("UILess::SetIntegrationStyle");
	return S_OK;
}

STDMETHODIMP CCandidateList::GetSelectionStyle(_Out_ TfIntegratableCandidateListSelectionStyle* ptfSelectionStyle)
{
	SLOGFMTI("UILess::GetSelectionStyle");
	*ptfSelectionStyle = _selectionStyle;
	return S_OK;
}

STDMETHODIMP CCandidateList::OnKeyDown(_In_ WPARAM wParam, _In_ LPARAM lParam, _Out_ BOOL* pIsEaten)
{
	SLOGFMTI("UILess::OnKeyDown");
	*pIsEaten = TRUE;
	return S_OK;
}

STDMETHODIMP CCandidateList::ShowCandidateNumbers(_Out_ BOOL* pIsShow)
{
	SLOGFMTI("UILess::ShowCandidateNumbers");
	*pIsShow = TRUE;
	return S_OK;
}

STDMETHODIMP CCandidateList::FinalizeExactCompositionString()
{
	SLOGFMTI("UILess::FinalizeExactCompositionString");
	return E_NOTIMPL;
}

HRESULT __stdcall CCandidateList::GetDisplayName(BSTR* pbstrName)
{
	*pbstrName = SysAllocString(L"启程之星");
	return S_OK;
}

HRESULT __stdcall CCandidateList::GetSearchCandidates(BSTR bstrQuery, BSTR bstrApplicationId, ITfCandidateList** pplist)
{
	*pplist = NULL;
	return S_FALSE;
}

HRESULT __stdcall CCandidateList::SetResult(BSTR bstrQuery, BSTR bstrApplicationID, BSTR bstrResult)
{
	return S_OK;
}

#endif

HRESULT CCandidateList::BeginUIElement()
{
	SOUI::SComPtr<ITfThreadMgr> pThreadMgr = _tsf->_GetThreadMgr();
	SOUI::SComPtr<ITfUIElementMgr> pUIElementMgr;
	auto hr = pThreadMgr->QueryInterface(&pUIElementMgr);
	_pbShow = TRUE;

	if (FAILED(hr) || (pUIElementMgr == NULL))
		return S_OK;

	if (FAILED(pUIElementMgr->BeginUIElement(this, &_pbShow, &_ui_id))) {
		_pbShow = TRUE;
		_ui_id = TF_INVALID_UIELEMENTID;
		return S_OK;
	}
	if (_pbShow)
	{
		return pUIElementMgr->EndUIElement(_ui_id);
	}
	return S_OK;
}

HRESULT CCandidateList::UpdateUIElement()
{
	//可以显示UI则不再调用 UpdateUIElement
	if (_pbShow)
		return S_OK;
	if (_ui_id == TF_INVALID_UIELEMENTID) return E_UNEXPECTED;

	HRESULT hr = S_OK;
	SOUI::SComPtr<ITfUIElementMgr> pUIElementMgr;
	SOUI::SComPtr<ITfThreadMgr> pThreadMgr = _tsf->_GetThreadMgr();
	if (nullptr == pThreadMgr)
	{
		return S_OK;
	}
	hr = pThreadMgr->QueryInterface(IID_ITfUIElementMgr, (void**)&pUIElementMgr);

	if (hr == S_OK)
	{		
		return pUIElementMgr->UpdateUIElement(_ui_id);
	}
	return S_OK;
}

HRESULT CCandidateList::EndUI()
{
	_ctx.cinfo.candies.clear(); _idx = 0;
	if (_pbShow)
		return S_OK;

	if (_ui_id == TF_INVALID_UIELEMENTID) return E_UNEXPECTED;

	SOUI::SComPtr<ITfThreadMgr> pThreadMgr = _tsf->_GetThreadMgr();
	SOUI::SComPtr<ITfUIElementMgr> pUIElementMgr;
	auto hr = pThreadMgr->QueryInterface(&pUIElementMgr);

	if (pUIElementMgr != NULL)
		return pUIElementMgr->EndUIElement(_ui_id);
	return S_OK;
}


