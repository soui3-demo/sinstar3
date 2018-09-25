#include "stdafx.h"
#include <WeaselUI.h>

using namespace weasel;

bool UI::Create(HWND parent)
{
	return true;
}

void UI::Destroy()
{
}

void UI::Show()
{
	isShowing_ = true;
}

void UI::Hide()
{
	isShowing_ = false;
}

void UI::ShowWithTimeout(DWORD millisec)
{
}

bool UI::IsCountingDown() const
{
	return false;
}

bool UI::IsShown() const
{
	return isShowing_;
}

void UI::Refresh()
{
}

void UI::UpdateInputPosition(RECT const& rc)
{
}

void UI::Update(const Context &ctx, const Status &status)
{
	ctx_ = ctx;
	status_ = status;
	Refresh();
}


