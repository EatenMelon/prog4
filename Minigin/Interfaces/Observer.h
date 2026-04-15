#pragma once
#include <string>
#include "Events.h"

namespace dae
{
	class Observer
	{
	public:
		virtual void OnNotify(Event event) = 0;
	};
}
