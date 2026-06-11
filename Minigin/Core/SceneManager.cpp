#include "SceneManager.h"
#include "Scene.h"
#include <imgui.h>
#include <string>

void minigin::SceneManager::StartNextFrame()
{
	if (!m_ChangedScene) return;
	m_ChangedScene = false;
}

void minigin::SceneManager::FixedUpdate(float fixedFrameTime)
{
	if (m_ChangedScene) return;
	if (m_ActiveSceneIdx >= m_scenes.size()) return;

	m_scenes[m_ActiveSceneIdx]->FixedUpdate(fixedFrameTime);
}

void minigin::SceneManager::Update(float deltaTime)
{
	if (m_ChangedScene) return;
	if (m_ActiveSceneIdx >= m_scenes.size()) return;

	m_scenes[m_ActiveSceneIdx]->Update(deltaTime);
}

void minigin::SceneManager::GuiRender()
{
	if (m_ChangedScene) return;
	if (m_ActiveSceneIdx >= m_scenes.size()) return;

	m_scenes[m_ActiveSceneIdx]->GuiRender();
}

void minigin::SceneManager::Render() const
{
	if (m_ChangedScene) return;
	if (m_ActiveSceneIdx >= m_scenes.size()) return;

	m_scenes[m_ActiveSceneIdx]->Render();
}

void minigin::SceneManager::Cleanup()
{
	for (auto& scene : m_scenes)
	{
		scene->Cleanup();
	}
}

bool minigin::SceneManager::SetActiveScene(size_t index)
{
	if (index >= m_scenes.size()) return false;
	if (index == m_ActiveSceneIdx) return false;

	if (m_ActiveSceneIdx < m_scenes.size())
	{
		m_scenes[m_ActiveSceneIdx]->DestroyAll();
	}

	m_ActiveSceneIdx = index;

	SelectSceneEvent event{};
	m_OnSelectScene.Notify(event);
	m_scenes[m_ActiveSceneIdx]->Load();
	m_ChangedScene = true;

	return true;
}

minigin::Scene& minigin::SceneManager::GetActiveScene()
{
	if (m_scenes.size() <= 0)
	{
		CreateScene([](Scene&) {});
		m_ActiveSceneIdx = 0;
		m_ChangedScene = true;
	}

	return *m_scenes[m_ActiveSceneIdx];
}

size_t minigin::SceneManager::ActiveSceneId() const
{
	return m_ActiveSceneIdx;
}

size_t minigin::SceneManager::GetSceneCount() const
{
	return m_scenes.size();
}

void minigin::SceneManager::CreateScene(const std::function<void(Scene&)>& loader)
{
	m_scenes.emplace_back(new Scene(m_scenes.size(), loader));
}
