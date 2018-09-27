#pragma once

#include <Ctffunc.h>

class CSinstar3Tsf : public CUnknown,
					 public ITfTextInputProcessorEx,
                     public ITfThreadMgrEventSink,
					 public ITfThreadFocusSink,
                     public ITfTextEditSink,
                     public ITfKeyEventSink,
                     public ITfCompositionSink,
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


    // ITfTextInputProcessor
    STDMETHODIMP Activate(ITfThreadMgr *pThreadMgr, TfClientId tfClientId);
    STDMETHODIMP Deactivate();

	/* ITfTextInputProcessorEx */
	STDMETHODIMP ActivateEx(ITfThreadMgr *pThreadMgr, TfClientId tfClientId, DWORD dwFlags);

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

    // ITfKeyEventSink
    STDMETHODIMP OnSetFocus(BOOL fForeground);
    STDMETHODIMP OnTestKeyDown(ITfContext *pContext, WPARAM wParam, LPARAM lParam, BOOL *pfEaten);
	STDMETHODIMP OnTestKeyUp(ITfContext *pContext, WPARAM wParam, LPARAM lParam, BOOL *pfEaten);
    STDMETHODIMP OnKeyDown(ITfContext *pContext, WPARAM wParam, LPARAM lParam, BOOL *pfEaten);
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
	BOOL InputStringW(LPCWSTR pszBuf, int nLen);
	BOOL IsCompositing() const;
	void StartComposition(LPVOID pImeContext);
	void ReplaceSelCompositionW(LPVOID pImeContext,int nLeft,int nRight,const WCHAR* wszComp,int nLen);
	void UpdateResultAndCompositionStringW(LPVOID lpImeContext,const WCHAR *wszResultStr,int nResStrLen,const WCHAR *wszCompStr,int nCompStrLen);
	void EndComposition(LPVOID pImeContext);
	LPVOID GetImeContext();
	BOOL   ReleaseImeContext(LPVOID lpImeContext);
	EInputMethod GetConversionMode();
	void SetConversionMode( EInputMethod eInputMode);
	BOOL SetOpenStatus(LPVOID lpImeContext,BOOL bOpen);
	BOOL GetOpenStatus(LPVOID lpImeContext) const;

    ITfThreadMgr *_GetThreadMgr() { return _pThreadMgr; }

    // utility function for compartment
    BOOL _IsKeyboardDisabled();
    BOOL _IsKeyboardOpen() const;
    HRESULT _SetKeyboardOpen(BOOL fOpen);


    // functions for the composition object.
	void OnStartComposition(TfEditCookie ec,ITfComposition *pComposition);
	ITfComposition* GetITfComposition(){return _pComposition;}

	void _StartComposition(ITfContext *pContext);
	void _ChangeComposition(ITfContext *pContext,int nLeft,int nRight,const WCHAR* wszComp,int nLen);
	void _UpdateResultAndCompositionStringW(ITfContext * pContext,const WCHAR *wszResultStr,int nResStrLen,const WCHAR *wszCompStr,int nCompStrLen);
	void _EndComposition(ITfContext *pContext);
	BOOL _EndCompositionEx();
	void _TerminateComposition(TfEditCookie ecWrite,ITfContext *pContext,bool bClearCtx);
	BOOL _IsComposing() const;
	BOOL _GetSegRange(TfEditCookie ec,ITfRange **pRange,int nLeft,int nRight);
		
private:

	BOOL _InitSinstar3();
	BOOL _UninitSinstar3();

    // initialize and uninitialize ThreadMgrEventSink.
    BOOL _InitThreadMgrEventSink();
    void _UninitThreadMgrEventSink();

    // initialize TextEditSink.
    BOOL _InitTextEditSink(ITfDocumentMgr *pDocMgr);

    // initialize and uninitialize KeyEventSink.
    BOOL _InitKeyEventSink();
    void _UninitKeyEventSink();


	// 
	HRESULT _AdviseTextLayoutSink(ITfContext *pContext);
	HRESULT _UnadviseTextLayoutSink(ITfContext *pContext);

	//
	BOOL _InitThreadCompartment();
	void _UninitThreadCompartment();

    ITfThreadMgr *_pThreadMgr;
    TfClientId _tfClientId;
	DWORD		_dwActivateFlag;

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

	SOUI::SComPtr<ITfComposition> _pComposition;
	DWORD _dwCookieTextLayoutSink; // Cookie for ITfContextKeyEventSink

	// 标志是否在一个可编辑的文档内
	BOOL _bInEditDocument;	
	BOOL _bKeyDownTested;
	BOOL _bKeyUpTested;

public:
	ISinstar*   m_pSinstar3;
	BOOL		_bHasFocus;
	BOOL		_bInKeyProc;

    // the current composition object.
	TfEditCookie _ec;
	BOOL         _bCompsiting;

public:
	IUNKNOWN_BEGIN(ITfTextInputProcessorEx)
		IUNKNOWN_ADD_IID(ITfTextInputProcessor)
		IUNKNOWN_ADD_IID(ITfThreadMgrEventSink)
		IUNKNOWN_ADD_IID(ITfThreadFocusSink)
		IUNKNOWN_ADD_IID(ITfTextEditSink)
		IUNKNOWN_ADD_IID(ITfKeyEventSink)
		IUNKNOWN_ADD_IID(ITfCompositionSink)
		IUNKNOWN_ADD_IID(ITfTextLayoutSink)
		IUNKNOWN_ADD_IID(ITfFnConfigure)
		IUNKNOWN_ADD_IID(ITfFunction)
		IUNKNOWN_ADD_IID(ITfCompartmentEventSink)
	IUNKNOWN_END()

};

