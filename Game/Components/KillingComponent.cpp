#include "KillingComponent.h"
#include <algorithm>

void digdug::KillingComponent::AddTarget(Target target)
{
	auto itr = std::find(m_Targets.begin(), m_Targets.end(), target);

	if (itr != m_Targets.end()) return;

	m_Targets.push_back(target);
}

void digdug::KillingComponent::RemoveTarget(Target target)
{
	auto itr = std::find(m_Targets.begin(), m_Targets.end(), target);

	if (itr == m_Targets.end()) return;

	m_Targets.erase(itr);
}

bool digdug::KillingComponent::IsTarget(Target target) const
{
	auto itr = std::find(m_Targets.begin(), m_Targets.end(), target);

	if (itr == m_Targets.end()) return false;

	return true;
}
