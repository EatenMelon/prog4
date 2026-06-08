#pragma once
#include <vector>
#include <memory>

#include <Component.h>
#include <Command.h>
#include <glm/glm.hpp>
#include <RenderComponent.h>
#include <HarpoonState.h>

namespace minigin
{
	class Texture2D;
	class Hitbox;
}

namespace digdug
{
	class DirtGrid;
	class AimComponent;
	class Harpoon final : public minigin::Component, public minigin::IObserver
	{
	public:
		using minigin::Component::Component;

		void Start() override;

		void Update(float deltaTime) override;
		void Render() const override;

		void Shoot();
		void Retract();

		void OnNotify(const minigin::IEvent& event) override;

		void EquipOnUser(minigin::GameObject& user);
		void AddDirtdGrid(DirtGrid* dirtGrid) { m_DirtGrid = dirtGrid; }

		void SetHarpoonSprite(const std::string& path);

		void DisableDuringUse(minigin::Command* command);

		minigin::GameObject* GetUser() const { return m_User; }
		AimComponent* GetAimComponent() const { return m_AimComp; }
		const glm::vec2 GetUserSize() const { return m_RenderCompUser->GetSize(); }

		const glm::vec2& GetSize() const { return m_Size; }
		minigin::Hitbox* GetHitbox() const { return m_Hitbox; }

	private:
		void TryEnableCommands();

		std::unique_ptr<HarpoonState> m_State{};

		minigin::GameObject* m_User{ nullptr };
		minigin::RenderComponent* m_RenderCompUser{ nullptr };
		AimComponent* m_AimComp{ nullptr };

		std::shared_ptr<minigin::Texture2D> m_Sprite{ nullptr };
		glm::vec2 m_Size{ 0.f, 0.f };

		minigin::Hitbox* m_Hitbox{ nullptr };
		unsigned int m_HitEventHash{ 0 };

		DirtGrid* m_DirtGrid{ nullptr };

		std::vector<minigin::Command*> m_DisableDuringUse{};
	};
}