#pragma once
#include "Client.h"
namespace weasel
{

	class ClientImpl
	{
	public:
		ClientImpl();
		~ClientImpl();

		bool Connect(ServerLauncher const& launcher);
		void Disconnect();
		void ShutdownServer() {}
		void StartSession();
		void EndSession();
		void StartMaintenance();
		void EndMaintenance();
		bool Echo();
		bool ProcessKeyEvent(KeyEvent const& keyEvent);
		bool CommitComposition();
		bool ClearComposition();
		void UpdateInputPosition(RECT const& rc);
		void FocusIn();
		void FocusOut();
		void TrayCommand(UINT menuId);
		bool GetResponseData(ResponseHandler const& handler);

	protected:
		bool _Connected() const { return _connected; }
		bool _Active() const { return _connected && session_id != 0; }

	private:
		UINT session_id;
		std::wstring app_name;
		bool is_ime;

		bool _connected;
	};

}