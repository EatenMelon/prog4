#pragma once
#include <Singleton.h>

#include <unordered_map>
#include <vector>

namespace minigin
{
	class HitboxManager final : public Singleton<HitboxManager>
	{
	public:
		void ChechCollisions();

	private:
		friend class Hitbox;
		bool Join(Hitbox& box);
		void Leave(const Hitbox& box);

		std::unordered_map<size_t, std::vector<Hitbox*>> m_Hitboxes{};

	};
}