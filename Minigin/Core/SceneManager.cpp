#include "SceneManager.h"
#include "Scene.h"

void minigin::SceneManager::Init()
{
	for (auto& scene : m_scenes)
	{
		scene->Init();
	}
}

void minigin::SceneManager::FixedUpdate(float fixedFrameTime)
{
	for (auto& scene : m_scenes)
	{
		scene->FixedUpdate(fixedFrameTime);
	}
}

void minigin::SceneManager::Update(float deltaTime)
{
	for(auto& scene : m_scenes)
	{
		scene->Update(deltaTime);
	}
}

void minigin::SceneManager::GuiRender() const
{
	for (const auto& scene : m_scenes)
	{
		scene->GuiRender();
	}
}

void minigin::SceneManager::Render() const
{
	for (const auto& scene : m_scenes)
	{
		scene->Render();
	}
}

void minigin::SceneManager::Cleanup()
{
	for (const auto& scene : m_scenes)
	{
		scene->Cleanup();
	}
}

minigin::Scene& minigin::SceneManager::CreateScene()
{
	m_scenes.emplace_back(new Scene());
	return *m_scenes.back();
}
