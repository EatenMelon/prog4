#pragma once
#include <string>
#include "Events.h"

namespace minigin
{
	class IObserver
	{
	public:
		virtual ~IObserver() = default;
		virtual void OnNotify(const IEvent* event) = 0;
	};
}
