#include "SceneManager.h"
#include "Scene.h"

void dae::SceneManager::FixedUpdate([[maybe_unused]] float fixedFrameTime)
{
	for (auto& scene : m_scenes)
	{
		scene->FixedUpdate(fixedFrameTime);
	}
}

void dae::SceneManager::Update([[maybe_unused]] float deltaTime)
{
	for(auto& scene : m_scenes)
	{
		scene->Update(deltaTime);
	}
}

void dae::SceneManager::Render()
{
	for (const auto& scene : m_scenes)
	{
		scene->Render();
	}
}

dae::Scene& dae::SceneManager::CreateScene()
{
	m_scenes.emplace_back(new Scene());
	return *m_scenes.back();
}
