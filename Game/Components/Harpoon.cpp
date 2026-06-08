#include "Harpoon.h"

#include <utility>
#include <unordered_set>

#include <Hitbox.h>
#include <Texture2D.h>
#include <Renderer.h>
#include <ResourceManager.h>

#include <Inflatable.h>
#include <HarpoonState.h>
#include <AimComponent.h>

void digdug::Harpoon::Start()
{
	if (m_User == nullptr)
	{
		throw std::runtime_error("Harpoon is not equiped on any game object!");
	}

	GetOwner().SetParent(m_User);

	m_RenderCompUser = m_User->GetComponent<minigin::RenderComponent>();

	if (m_RenderCompUser != nullptr)
	{
		m_Size *= m_RenderCompUser->GetUniformScale();
	}
	else
	{
		throw std::runtime_error("Harpoons user requires a render component!");
	}

	m_AimComp = m_User->GetComponent<AimComponent>();

	if (m_AimComp == nullptr)
	{
		throw std::runtime_error("Harpoon cannot be aimed!");
	}

	m_Hitbox = GetOwner().GetComponent<minigin::Hitbox>();
	if (m_Hitbox == nullptr)
	{
		throw std::runtime_error("Harpoon doesn't have a hitbox!");
	}

	m_State = std::make_unique<HarpoonIdleState>(this);

	minigin::HitEvent event{ nullptr };
	m_HitEventHash = event.GetEventHash();

	m_Hitbox->HitEnterEvent().Subscribe(this);
}

void digdug::Harpoon::Update(float deltaTime)
{
	auto newState = m_State->Update(deltaTime, m_DirtGrid);

	if (newState == nullptr) return;
	m_State = std::move(newState);

	TryEnableCommands();
}

void digdug::Harpoon::Render() const
{
	const float extension = m_State->GetExtend();
	if (extension <= 0.f) return;

	const auto origin{ GetOwner().GetWorldPosition() };
	auto renderer = minigin::Renderer::GetInstance().GetSDLRenderer();

	SDL_FRect src{};
	src.x = m_Sprite->GetSize().x * (1 - extension);
	src.y = 0.f;
	src.w = m_Sprite->GetSize().x * extension;
	src.h = m_Sprite->GetSize().y;

	SDL_FRect dst{};
	dst.x = origin.x;
	dst.y = origin.y;
	dst.w = m_Size.x * extension;
	dst.h = m_Size.y;
	
	SDL_FPoint point{};
	point.x = 0.f;
	point.y = 0.f;

	const auto size{ m_State->GetHarpoonSize() };

	switch (m_AimComp->GetDirection())
	{
	case minigin::Direction::Down:	dst.x += size.x;	break;
	case minigin::Direction::Up:	dst.y += size.y;	break;
	default: break;
	}

	SDL_RenderTextureRotated
	(
		renderer,
		m_Sprite->GetSDLTexture(),
		&src,
		&dst,
		m_RenderCompUser->GetRotation(),
		&point,
		m_RenderCompUser->GetSDLFlipMode()
	);
}

void digdug::Harpoon::Shoot()
{
	auto newState = m_State->StartShoot();

	if (newState == nullptr) return;
	m_State = std::move(newState);

	TryEnableCommands();
}

void digdug::Harpoon::Retract()
{
	auto newState = m_State->StartRetract();

	if (newState == nullptr) return;
	m_State = std::move(newState);

	TryEnableCommands();
}

void digdug::Harpoon::OnNotify(const minigin::IEvent& event)
{
	if (m_HitEventHash != event.GetEventHash()) return;

	const auto& hitEvent = static_cast<const minigin::HitEvent&>(event);

	auto inflatable = hitEvent.Who()->GetOwner().GetComponent<Inflatable>();

	if (inflatable == nullptr) return;

	auto newState = m_State->OnAttach(inflatable);

	if (newState == nullptr) return;
	m_State = std::move(newState);

	TryEnableCommands();
}

void digdug::Harpoon::EquipOnUser(minigin::GameObject& user)
{
	m_User = &user;
}

void digdug::Harpoon::SetHarpoonSprite(const std::string& path)
{
	m_Sprite = minigin::ResourceManager::GetInstance().LoadTexture(path);
	m_Size = m_Sprite->GetSize();
}

void digdug::Harpoon::DisableDuringUse(minigin::Command* command)
{
	if (command == nullptr) return;

	auto itr = std::find(m_DisableDuringUse.begin(), m_DisableDuringUse.end(), command);

	if (itr != m_DisableDuringUse.end()) return;

	m_DisableDuringUse.push_back(command);
}

void digdug::Harpoon::TryEnableCommands()
{
	if (m_DisableDuringUse.empty()) return;

	std::unordered_set<minigin::Command*> toRemove{};

	for (auto command : m_DisableDuringUse)
	{
		try
		{
			command->Enable(!m_State->UsingHarpoon());
		}
		catch (const std::exception& e)
		{
			std::cerr << "ERROR: Harpoon::Update" << e.what() << "\n";
			toRemove.emplace(command);
		}
	}

	if (toRemove.empty()) return;

	m_DisableDuringUse.erase
	(
		std::remove_if
		(
			m_DisableDuringUse.begin(),
			m_DisableDuringUse.end(),

			[&](minigin::Command* cmd)
			{
				return toRemove.contains(cmd);
			}
		),
		m_DisableDuringUse.end()
	);
}
