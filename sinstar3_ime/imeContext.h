#pragma  once

class CImeContext
{
public:
	CImeContext()
	{
		_hIMC=0;
		_lpContext=0;
		_lpTranBuf=NULL;
		_nMsgInBuf=0;
	}
	~CImeContext()
	{
		if(_hIMC && _lpContext)
		{
			ImmUnlockIMC(_hIMC);
		}
		_hIMC=0;
		_lpContext=NULL;
		_lpTranBuf=NULL;
	}

	BOOL Init(HIMC hIMC)
	{
		if(!(_lpContext=ImmLockIMC(hIMC)))
		{
			ImmUnlockIMC(hIMC);
			return FALSE;
		}
		_hIMC=hIMC;
		_lpTranBuf=NULL;
		_nMsgInBuf=0;
		return TRUE;
	}

	BOOL Init(HIMC hIMC,LPDWORD lpTransBuf)
	{
		if(!(_lpContext=ImmLockIMC(hIMC))) return FALSE;
		_hIMC=hIMC;
		_lpTranBuf=(PTRANSMSGLIST)lpTransBuf;
		_nMsgInBuf=0;
		return TRUE;
	}
	BOOL GenerateMessage(UINT uMsg,WPARAM wParam,LPARAM lParam)
	{
		_ASSERT(_hIMC && _lpContext);
		BOOL bRet=FALSE;
		TRANSMSG msg={uMsg,wParam,lParam};
		if(_lpTranBuf)
		{
			if(_nMsgInBuf< _lpTranBuf->uMsgCount)
			{
				_lpTranBuf->TransMsg[_nMsgInBuf++]=msg;
				bRet=TRUE;
			}
		}else if (IsWindow(_lpContext->hWnd))
		{
			PTRANSMSG lpMsg;
			if (!(_lpContext->hMsgBuf = ImmReSizeIMCC(_lpContext->hMsgBuf,
				(_lpContext->dwNumMsgBuf +1) * sizeof(TRANSMSG))))
				return FALSE;

			if (!(lpMsg = (PTRANSMSG)ImmLockIMCC(_lpContext->hMsgBuf)))
				return FALSE;

			lpMsg += _lpContext->dwNumMsgBuf;
			*lpMsg = msg;
			ImmUnlockIMCC(_lpContext->hMsgBuf);
			_lpContext->dwNumMsgBuf++;

			ImmGenerateMessage(_hIMC);
			bRet=TRUE;
		}
		return bRet;
	}

	HIMC _hIMC;
	LPINPUTCONTEXT _lpContext;
	PTRANSMSGLIST _lpTranBuf;
	UINT _nMsgInBuf;
};