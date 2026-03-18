#include "Subject.h"
#include "Observer.h"
#include <algorithm>

bool dae::Subject::Subscrube(dae::Observer* subscriber)
{
	// avoid calling OnNotify of the same observer multiple times
	if (std::find(m_Observers.begin(), m_Observers.end(), subscriber) != m_Observers.end()) return false;

	m_Observers.push_back(subscriber);

	return true;
}

bool dae::Subject::UnSubscribe(dae::Observer* subscriber)
{
	auto itr = std::find(m_Observers.begin(), m_Observers.end(), subscriber);

	if (itr == m_Observers.end()) return false;

	m_Observers.erase(itr);

	return true;
}

void dae::Subject::Notify(Event event)
{
	for (auto& observer : m_Observers)
	{
		observer->OnNotify(event);
	}
}