#include "SceneManager.h"
#include "Scene.h"

void dae::SceneManager::FixedUpdate(float fixedFrameTime)
{
	for (auto& scene : m_scenes)
	{
		scene->FixedUpdate(fixedFrameTime);
	}
}

void dae::SceneManager::Update(float deltaTime)
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

void dae::SceneManager::Cleanup()
{
	for (const auto& scene : m_scenes)
	{
		scene->Cleanup();
	}
}

dae::Scene& dae::SceneManager::CreateScene()
{
	m_scenes.emplace_back(new Scene());
	return *m_scenes.back();
}
