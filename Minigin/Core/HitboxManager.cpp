#include "HitboxManager.h"
#include <Hitbox.h>
#include <SceneManager.h>

void minigin::HitboxManager::ChechCollisions()
{
	auto id = SceneManager::GetInstance().ActiveSceneId();

	if (!m_Hitboxes.contains(id)) return;

	auto& boxes = m_Hitboxes.find(id)->second;

	for (auto boxA : boxes)
	{
		for (auto boxB : boxes)
		{
			boxA->CheckCollision(*boxB);
		}
	}
}

bool minigin::HitboxManager::Join(Hitbox& box)
{
	auto id = box.GetOwner().GetSceneId();

	if (!m_Hitboxes.contains(id))
	{
		m_Hitboxes.emplace(id, std::vector<Hitbox*>{&box});
		return true;
	}

	auto& boxes = m_Hitboxes.find(id)->second;

	auto itr = std::find
	(
		boxes.begin(),
		boxes.end(),
		&box
	);

	if (itr != boxes.end()) return false;

	boxes.push_back(&box);

	return true;
}

void minigin::HitboxManager::Leave(const Hitbox& box)
{
	auto id = box.GetOwner().GetSceneId();

	if (!m_Hitboxes.contains(id)) return;

	auto& boxes = m_Hitboxes.find(id)->second;

	auto itr = std::find
	(
		boxes.begin(),
		boxes.end(),
		&box
	);

	if (itr == boxes.end()) return;

	boxes.erase(itr);
}
