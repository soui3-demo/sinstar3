#include "stdafx.h"
#include "WeaselClientImpl.h"

using namespace weasel;

ClientImpl::ClientImpl()
	: session_id(0),
	  is_ime(false),
	_connected(false)
{
}

ClientImpl::~ClientImpl()
{
}

//http://stackoverflow.com/questions/557081/how-do-i-get-the-hmodule-for-the-currently-executing-code
HMODULE GetCurrentModule()
{ // NB: XP+ solution!
  HMODULE hModule = NULL;
  GetModuleHandleEx(
    GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
    (LPCTSTR)GetCurrentModule,
    &hModule);

  return hModule;
}

bool ClientImpl::Connect(ServerLauncher const& launcher)
{
	_connected = true;
	return true;
}

void ClientImpl::Disconnect()
{
	if (_Active())
		EndSession();
	_connected = false;
}

bool ClientImpl::ProcessKeyEvent(KeyEvent const& keyEvent)
{
	if (!_Active())
		return false;
	return true;
}

bool ClientImpl::CommitComposition()
{
	if (!_Active())
		return false;
	return true;
}

bool ClientImpl::ClearComposition()
{
	if (!_Active())
		return false;
}

void ClientImpl::UpdateInputPosition(RECT const& rc)
{
	if (!_Active())
		return;

}

void ClientImpl::FocusIn()
{
	DWORD client_caps = 0;  /* TODO */
}

void ClientImpl::FocusOut()
{
}

void ClientImpl::TrayCommand(UINT menuId)
{
}

void ClientImpl::StartSession()
{
	if (_Active() && Echo())
		return;

	session_id = 100;
}

void ClientImpl::EndSession()
{
	session_id = 0;
}

void ClientImpl::StartMaintenance()
{
	session_id = 0;
}

void ClientImpl::EndMaintenance()
{
	session_id = 0;
}

bool ClientImpl::Echo()
{
	if (!_Active())
		return false;

	return true;
}

bool ClientImpl::GetResponseData(ResponseHandler const& handler)
{
	if (!handler) {
		return false;
	}

	return true;
}



Client::Client() 
	: m_pImpl(new ClientImpl())
{}

Client::~Client()
{
	if (m_pImpl)
		delete m_pImpl;
}

bool Client::Connect(ServerLauncher launcher)
{
	return m_pImpl->Connect(launcher);
}

void Client::Disconnect()
{
	m_pImpl->Disconnect();
}

void Client::ShutdownServer()
{
	m_pImpl->ShutdownServer();
}

bool Client::ProcessKeyEvent(KeyEvent const& keyEvent)
{
	return m_pImpl->ProcessKeyEvent(keyEvent);
}

bool Client::CommitComposition()
{
	return m_pImpl->CommitComposition();
}

bool Client::ClearComposition()
{
	return m_pImpl->ClearComposition();
}

void Client::UpdateInputPosition(RECT const& rc)
{
	m_pImpl->UpdateInputPosition(rc);
}

void Client::FocusIn()
{
	m_pImpl->FocusIn();
}

void Client::FocusOut()
{
	m_pImpl->FocusOut();
}

void Client::StartSession()
{
	m_pImpl->StartSession();
}

void Client::EndSession()
{
	m_pImpl->EndSession();
}

void Client::StartMaintenance()
{
	m_pImpl->StartMaintenance();
}

void Client::EndMaintenance()
{
	m_pImpl->EndMaintenance();
}

void Client::TrayCommand(UINT menuId)
{
	m_pImpl->TrayCommand(menuId);
}

bool Client::Echo()
{
	return m_pImpl->Echo();
}

bool Client::GetResponseData(ResponseHandler handler)
{
	return m_pImpl->GetResponseData(handler);
}
