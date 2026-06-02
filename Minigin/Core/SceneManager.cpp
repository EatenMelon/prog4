#include "SceneManager.h"
#include "SceneManager.h"
#include "Scene.h"
#include <imgui.h>
#include <string>

void minigin::SceneManager::FixedUpdate(float fixedFrameTime)
{
	if (m_ActiveSceneIdx >= m_scenes.size()) return;

	m_scenes[m_ActiveSceneIdx]->FixedUpdate(fixedFrameTime);
}

void minigin::SceneManager::Update(float deltaTime)
{
	if (m_ActiveSceneIdx >= m_scenes.size()) return;

	m_scenes[m_ActiveSceneIdx]->Update(deltaTime);
}

void minigin::SceneManager::GuiRender()
{
	if (m_ActiveSceneIdx >= m_scenes.size()) return;

	m_scenes[m_ActiveSceneIdx]->GuiRender();

	bool open = true;
	ImGui::Begin("Scene selector", &open);
	{
		ImGui::Text("Active Scene [%zu]", m_ActiveSceneIdx);

		for (size_t s{ 0 }; s < m_scenes.size(); ++s)
		{
			if (s == m_ActiveSceneIdx) continue;

			std::string label = "Scene " + std::to_string(s);

			if (!ImGui::Button(label.c_str(), { 120, 0 }))
			{
				continue;
			}

			SetActiveScene(s);
		}
	}
	ImGui::End();
}

void minigin::SceneManager::Render() const
{
	if (m_ActiveSceneIdx >= m_scenes.size()) return;

	m_scenes[m_ActiveSceneIdx]->Render();
}

void minigin::SceneManager::Cleanup()
{
	for (auto& scene : m_scenes)
	{
		scene->Cleanup();
	}

	m_scenes.clear();
	m_ActiveSceneIdx = 0;
}

bool minigin::SceneManager::SetActiveScene(size_t index)
{
	if (index >= m_scenes.size()) return false;

	if (index == m_ActiveSceneIdx) return false;

	m_ActiveSceneIdx = index;

	return true;
}

size_t minigin::SceneManager::ActiveSceneId() const
{
	return m_ActiveSceneIdx;
}

minigin::Scene& minigin::SceneManager::CreateScene()
{
	m_scenes.emplace_back(new Scene(m_scenes.size()));
	return *m_scenes.back();
}
