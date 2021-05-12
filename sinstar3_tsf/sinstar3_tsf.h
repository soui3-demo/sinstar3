#pragma once

#include <Ctffunc.h>
#include "../sinstar3_proxy/SimpleWnd.h"
#include "../sinstar3_proxy/SinstarProxy.h"
#include "LanguageBar.h"

#define  UICLASSNAME _T("sinstar3_tsfwnd")

class CCandidateList;

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
					 public ITfDisplayAttributeProvider,
					 public ITextService,
					 public CSimpleWnd
{

	friend class CCandidateList;

	enum{
		TID_INIT=100,
	};
public:
	// CClassFactory factory callback
	static HRESULT CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppvObj);

public:
    CSinstar3Tsf();
    virtual ~CSinstar3Tsf();
	std::wstring GetPeeditString()
	{
		if (_pcand)
		{
			return _pcand->_ctx.preedit.str;
		}
		return _strPreedit;
	}
	void UpdateUI(ITfContext* pContext, bool bPageChanged, UINT curPage);
	void UpdatePreedit(UINT64 pContext,const std::wstring &strPreedit);
	TfGuidAtom GetDisplayAttribInfo()const
	{
		return _gaDisplayAttributeInput;
	}
	virtual void UpdateUI(UINT64 imeContext, bool bPageChanged, UINT curPage)override;
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

	// ITfDisplayAttributeProvider
	STDMETHODIMP EnumDisplayAttributeInfo(IEnumTfDisplayAttributeInfo **ppEnum);
	STDMETHODIMP GetDisplayAttributeInfo(REFGUID guidInfo, ITfDisplayAttributeInfo **ppInfo);

	//ITextService
	BOOL InputStringW(LPCWSTR pszBuf, int nLen) override;
	BOOL IsCompositing() const override;
	void StartComposition(UINT64 imeContext) override;
	void ReplaceSelCompositionW(UINT64 imeContext,int nLeft,int nRight,const WCHAR* wszComp,int nLen) override;
	void UpdateResultAndCompositionStringW(UINT64 imeContext,const WCHAR *wszResultStr,int nResStrLen,const WCHAR *wszCompStr,int nCompStrLen) override;
	void EndComposition(UINT64 imeContext) override;
	EInputMethod GetConversionMode() override;
	void SetConversionMode( EInputMethod eInputMode) override;
	DWORD GetActiveWnd() const override;	

    ITfThreadMgr *_GetThreadMgr() { return _pThreadMgr; }

    // utility function for compartment
    BOOL _IsKeyboardDisabled();
    BOOL _IsKeyboardOpen() const;
    HRESULT _SetKeyboardOpen(BOOL fOpen);


    // functions for the composition object.
	void OnStartComposition(TfEditCookie ec, ITfComposition* pComposition, ITfContext* pContext);
	void OnStartComposition(TfEditCookie ec,ITfComposition *pComposition);
	ITfComposition* GetITfComposition(){return _pComposition;}

	void _StartComposition(ITfContext *pContext);
	void _ChangeComposition(ITfContext *pContext,int nLeft,int nRight,const WCHAR* wszComp,int nLen);
	void _UpdateResultAndCompositionStringW(ITfContext * pContext,const WCHAR *wszResultStr,int nResStrLen,const WCHAR *wszCompStr,int nCompStrLen);
	void _EndComposition(ITfContext *pContext,bool bClearCtx=false);
	void _TerminateComposition(TfEditCookie ecWrite,ITfContext *pContext,bool bClearCtx);
	BOOL _IsCompositing() const;
	BOOL _GetSegRange(TfEditCookie ec,ITfRange **pRange,int nLeft,int nRight);
	BOOL _SetCompositionDisplayAttributes(TfEditCookie ec, _In_ ITfContext *pContext, ITfRange* pRangeComposition);

	void _SyncFocus(BOOL bFocus);
	void OnAsyncFocus();
protected:
	virtual LRESULT WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void OnReconnReady();

	ITfContext * GetImeContext();
	void   ReleaseImeContext(ITfContext * imeContext);
	void SetOpenStatus(BOOL bOpen);
	BOOL GetOpenStatus() const;

private:

	BOOL _InitSinstar3(HWND hWnd);
	BOOL _UninitSinstar3();

    // initialize and uninitialize ThreadMgrEventSink.
    BOOL _InitThreadMgrEventSink();
    void _UninitThreadMgrEventSink();

    // initialize TextEditSink.
    BOOL _InitTextEditSink(ITfDocumentMgr *pDocMgr);

    // initialize and uninitialize KeyEventSink.
    BOOL _InitKeyEventSink();
    void _UninitKeyEventSink();

	BOOL _InitLanguageBar();
	void _UninitLanguageBar();

	BOOL _InitDisplayAttributeGuidAtom();

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

	SOUI::SComPtr<ITfComposition> _pComposition;	//save composition ptr, indicator.
	BOOL         _bCompositing;                     //save startComposition and endComposition call
	bool		 _bChangedDocMgr;
	CLangBarItemButton* _pLangBarItem;

	DWORD _dwCookieTextLayoutSink;					// Cookie for ITfContextKeyEventSink

	// 标志是否在一个可编辑的文档内
	BOOL _bInEditDocument;	
	BOOL _bKeyDownTested;
	BOOL _bKeyUpTested;
	CCandidateList* _pcand;
	TfGuidAtom _gaDisplayAttributeInput;
	TfGuidAtom _gaDisplayAttributeConverted;
	//非uiless模式下保存inline字符串
	std::wstring _strPreedit;
public:
	CSinstarProxy*   m_pSinstar3;
	BOOL		_bHasFocus;
	BOOL		_bInKeyProc;
	bool		_bUILess;
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
		IUNKNOWN_ADD_IID(ITfDisplayAttributeProvider)
	IUNKNOWN_END()
};

