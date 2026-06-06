#include "HitboxManager.h"
#include "SceneManager.h"
#include <Hitbox.h>

void minigin::HitboxManager::Init()
{
	SelectSceneEvent event{};
	m_ResetEventHash = event.GetEventHash();

	SceneManager::GetInstance().SceneSelectEvent().Subscrube(this);
}

void minigin::HitboxManager::ChechCollisions()
{
	for (auto boxA : m_Hitboxes)
	{
		for (auto boxB : m_Hitboxes)
		{
			boxA->CheckCollision(*boxB);
		}
	}
}

void minigin::HitboxManager::OnNotify(const IEvent& event)
{
	if (event.GetEventHash() != m_ResetEventHash)
	{
		return;
	}

	m_Hitboxes.clear();
}

bool minigin::HitboxManager::Join(Hitbox& box)
{
	auto itr = std::find
	(
		m_Hitboxes.begin(),
		m_Hitboxes.end(),
		&box
	);

	if (itr != m_Hitboxes.end()) return false;

	m_Hitboxes.push_back(&box);

	return true;
}

void minigin::HitboxManager::Leave(const Hitbox& box)
{
	auto itr = std::find
	(
		m_Hitboxes.begin(),
		m_Hitboxes.end(),
		&box
	);

	if (itr == m_Hitboxes.end()) return;

	m_Hitboxes.erase(itr);
}
