#pragma once
#include <string>
#include "Events.h"

namespace minigin
{
	class IObserver
	{
	public:
		virtual void OnNotify(IEvent event) = 0;
	};
}
