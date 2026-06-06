#pragma once
#include <Singleton.h>
#include <vector>

namespace minigin
{
	class Hitbox;
	class HitboxManager final : public Singleton<HitboxManager>
	{
	public:
		void ChechCollisions();
		void Reset();

	private:
		friend class Hitbox;
		bool Join(Hitbox& box);
		void Leave(const Hitbox& box);

		std::vector<Hitbox*> m_Hitboxes{};

	};
}