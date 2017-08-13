#pragma once

#include <Ctffunc.h>

class CSinstar3Tsf : public ITfTextInputProcessor,
                     public ITfThreadMgrEventSink,
					 public ITfThreadFocusSink,
                     public ITfTextEditSink,
                     public ITfKeyEventSink,
                     public ITfCompositionSink,
					 public ITfDisplayAttributeProvider,
					 public ITfTextLayoutSink,
					 public ITfFnConfigure,
					 public ITfCompartmentEventSink,
					 public ITextService
{
public:
	// CClassFactory factory callback
	static HRESULT CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppvObj);

public:
    CSinstar3Tsf();
    virtual ~CSinstar3Tsf();

    // IUnknown
	IUNKNOWN_BEGIN(IID_ITfTextInputProcessor,ITfTextInputProcessor)
		IUNKNOWN_ADD_IID(IID_ITfThreadMgrEventSink,ITfThreadMgrEventSink)
		IUNKNOWN_ADD_IID(IID_ITfThreadFocusSink,ITfThreadFocusSink)
		IUNKNOWN_ADD_IID(IID_ITfTextEditSink,ITfTextEditSink)
		IUNKNOWN_ADD_IID(IID_ITfKeyEventSink,ITfKeyEventSink)
		IUNKNOWN_ADD_IID(IID_ITfDisplayAttributeProvider,ITfDisplayAttributeProvider)
		IUNKNOWN_ADD_IID(IID_ITfCompositionSink,ITfCompositionSink)
		IUNKNOWN_ADD_IID(IID_ITfTextLayoutSink,ITfTextLayoutSink)
		IUNKNOWN_ADD_IID(IID_ITfFnConfigure,ITfFnConfigure)
		IUNKNOWN_ADD_IID(IID_ITfFunction,ITfFunction)
		IUNKNOWN_ADD_IID(IID_ITfCompartmentEventSink,ITfCompartmentEventSink)
	IUNKNOWN_END()

    // ITfTextInputProcessor
    STDMETHODIMP Activate(ITfThreadMgr *pThreadMgr, TfClientId tfClientId);
    STDMETHODIMP Deactivate();

    // ITfThreadMgrEventSink
    STDMETHODIMP OnInitDocumentMgr(ITfDocumentMgr *pDocMgr);
    STDMETHODIMP OnUninitDocumentMgr(ITfDocumentMgr *pDocMgr);
    STDMETHODIMP OnSetFocus(ITfDocumentMgr *pDocMgrFocus, ITfDocumentMgr *pDocMgrPrevFocus);
    STDMETHODIMP OnPushContext(ITfContext *pContext);
    STDMETHODIMP OnPopContext(ITfContext *pContext);

	// ITfThreadFocusSink
	STDMETHODIMP OnSetThreadFocus();
	STDMETHODIMP OnKillThreadFocus();

    // ITfTextEditSink
    STDMETHODIMP OnEndEdit(ITfContext *pContext, TfEditCookie ecReadOnly, ITfEditRecord *pEditRecord);

	// ITfDisplayAttributeProvider
	STDMETHODIMP EnumDisplayAttributeInfo(IEnumTfDisplayAttributeInfo **ppEnum);
	STDMETHODIMP GetDisplayAttributeInfo(REFGUID guidInfo, ITfDisplayAttributeInfo **ppInfo);

    // ITfKeyEventSink
    STDMETHODIMP OnSetFocus(BOOL fForeground);
    STDMETHODIMP OnTestKeyDown(ITfContext *pContext, WPARAM wParam, LPARAM lParam, BOOL *pfEaten);
    STDMETHODIMP OnKeyDown(ITfContext *pContext, WPARAM wParam, LPARAM lParam, BOOL *pfEaten);
    STDMETHODIMP OnTestKeyUp(ITfContext *pContext, WPARAM wParam, LPARAM lParam, BOOL *pfEaten);
    STDMETHODIMP OnKeyUp(ITfContext *pContext, WPARAM wParam, LPARAM lParam, BOOL *pfEaten);
	STDMETHODIMP OnPreservedKey(ITfContext *pic, REFGUID rguid, BOOL *pfEaten);
    // ITfCompositionSink
    STDMETHODIMP OnCompositionTerminated(TfEditCookie ecWrite, ITfComposition *pComposition);

	// ITfTextLayoutSink
	STDMETHODIMP OnLayoutChange(ITfContext *pContext, TfLayoutCode lcode, ITfContextView *pContextView);
	
	//ITfFnConfigure
	STDMETHODIMP  Show(	HWND hwndParent,LANGID langid,REFGUID rguidProfile);
	STDMETHODIMP  GetDisplayName(BSTR* pbstrName);

	// ITfCompartmentEventSink
    STDMETHODIMP OnChange(REFGUID rguidCompartment);


	//ITextService
	BOOL IsCompositing();
	void StartComposition(LPVOID pImeContext);
	void ReplaceSelCompositionW(LPVOID pImeContext,int nLeft,int nRight,WCHAR* wszComp,int nLen);
	void UpdateResultAndCompositionStringW(LPVOID lpImeContext,WCHAR *wszResultStr,int nResStrLen,WCHAR *wszCompStr,int nCompStrLen);
	void EndComposition(LPVOID pImeContext);
	int  MoveCaretPos(LPVOID lpImeContext,int nPos,BOOL bSet);
	LPVOID GetImeContext();
	BOOL   ReleaseImeContext(LPVOID lpImeContext);
	EInputMethod GetConversionMode();
	void SetConversionMode( EInputMethod eInputMode);
	BOOL RegisterIMEHotKey(REFGUID guidHotKey,LPCWSTR pszName,const PRESERVEDKEY *pKey);
	BOOL UnregisterIMEHotKey(REFGUID guidHotKey,const PRESERVEDKEY *pKey);
	BOOL SetOpenStatus(LPVOID lpImeContext,BOOL bOpen)
	{
		return _SetKeyboardOpen(bOpen);
	}
	BOOL GetOpenStatus(LPVOID lpImeContext)
	{
		return _IsKeyboardOpen();
	}
	BOOL GetLanguageBarItemMgr(ITfLangBarItemMgr **ppLangBarMgr,GUID *pGuidTIP);
	LRESULT DoWildRequest(WPARAM wParam,LPARAM lParam){return E_NOTIMPL;}


    ITfThreadMgr *_GetThreadMgr() { return _pThreadMgr; }
    TfClientId _GetClientId() { return _tfClientId; }

    // utility function for compartment
    BOOL _IsKeyboardDisabled();
    BOOL _IsKeyboardOpen();
    HRESULT _SetKeyboardOpen(BOOL fOpen);


    // functions for the composition object.
	void _StartComposition(ITfContext *pContext);
	void _ChangeComposition(ITfContext *pContext,int nLeft,int nRight,WCHAR* wszComp,int nLen);
	void _UpdateResultAndCompositionStringW(ITfContext * pContext,WCHAR *wszResultStr,int nResStrLen,WCHAR *wszCompStr,int nCompStrLen);
	void _EndComposition(ITfContext *pContext);
	BOOL _EndCompositionEx();
	int  _MoveCaretPos(ITfContext *pContext,int nPos,BOOL bSet);
	void _TerminateComposition(TfEditCookie ec, ITfContext *pContext);
	BOOL _IsComposing();
	void _SetComposition(ITfComposition *pComposition);
	BOOL _SetCompositionDisplayAttributes(TfEditCookie ec, ITfContext *pContext,ITfRange *pRange, TF_DA_ATTR_INFO attr);
	BOOL _GetSegRange(TfEditCookie ec,ITfRange **pRange,int nLeft,int nRight);
	
	void UpdateCompAttr(ITfContext *pContext,TfEditCookie ec,ITfRange *pRangeComposition);
	void _ClearCompositionDisplayAttributes(TfEditCookie ec, ITfContext *pContext);

	void CSinstar3Tsf::ShowCandidateWindow();
	
private:

	BOOL _InitBaiduJP3();
	BOOL _UninitBaiduJP3();

    // initialize and uninitialize ThreadMgrEventSink.
    BOOL _InitThreadMgrEventSink();
    void _UninitThreadMgrEventSink();

    // initialize TextEditSink.
    BOOL _InitTextEditSink(ITfDocumentMgr *pDocMgr);

    // initialize and uninitialize KeyEventSink.
    BOOL _InitKeyEventSink();
    void _UninitKeyEventSink();

	BOOL _InitDisplayAttributeGuidAtom();

	// 
	HRESULT _AdviseTextLayoutSink(ITfContext *pContext);
	HRESULT _UnadviseTextLayoutSink(ITfContext *pContext);

	//
	BOOL _InitThreadCompartment();
	void _UninitThreadCompartment();

    // utility function for KeyEventSink
    BOOL _IsKeyEaten(ITfContext *pContext, WPARAM wParam, LPARAM lParam);

    ITfThreadMgr *_pThreadMgr;
    TfClientId _tfClientId;

    // The cookie of ThreadMgrEventSink
    DWORD _dwThreadMgrEventSinkCookie;
	DWORD _dwThreadFocusSinkCookie;

	DWORD _dwThreadInputModeSinkCookie;
	DWORD _dwThreadKeyboardSinkCookie;
    //
    // private variables for TextEditSink
    //
    ITfContext   *_pTextEditSinkContext;
    DWORD _dwTextEditSinkCookie;

public:
	ISinstar* m_pBaiduJP3;
	BOOL		_bHasFocus;
	BOOL _bGetLayoutChangeMsg;
	DWORD		_dwLastLayoutChangeTime;
	BOOL		_bInKeyProc;

    // the current composition object.
    ITfComposition *_pComposition;

	// guidatom for the display attibute.
	TfGuidAtom _gaDisplayAttributeInput;
	TfGuidAtom _gaDisplayAttributeConverted;
	TfGuidAtom _gaDisplayAttributeTargetConverted;

	DWORD _dwCookieTextLayoutSink; // Cookie for ITfContextKeyEventSink

	// 标志是否在一个可编辑的文档内
	BOOL _bInEditDocument;	

	BOOL _bUseCursorPos;

	BOOL _bUseMSMethod;

	BOOL _bPosSaved;

	RECT _rtStart;
};

