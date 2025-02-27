
#include "stdafx.h"
#include "IExit.h"
#include "ExitEvent.h"


IExit::IExit()
{
	IEXIT_HANDLE->AddExitEvent(this);
}

IExit::~IExit()
{
	IEXIT_HANDLE->DeleteExitEvent(this);
}

bool IExit::OnExit()
{
	return true;
}

