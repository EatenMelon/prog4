#include "Scene.h"
#include <algorithm>
#include <cassert>
#include <InputManager.h>

using namespace minigin;

void minigin::Scene::Load()
{
	// refresh bindings for the new scene
	// ensuring the old bindings don't affect
	// new objects in memory
	InputManager::GetInstance().Init(4);

	m_Loader(*this);
}

void Scene::Add(std::unique_ptr<GameObject> object)
{
	assert(object != nullptr && "Cannot add a null GameObject to the scene.");
	object->SetSceneId(m_Id);
	m_objects.emplace_back(std::move(object));
}

void Scene::Remove(const GameObject& object)
{
	m_objects.erase(
		std::remove_if(
			m_objects.begin(),
			m_objects.end(),
			[&object](const auto& ptr) { return ptr.get() == &object; }
		),
		m_objects.end()
	);
}

void Scene::RemoveAll()
{
	m_objects.clear();
}

void Scene::FixedUpdate(float fixedFrameTime)
{
	for (auto& object : m_objects)
	{
		object->Update(fixedFrameTime);
	}
}

void Scene::Update(float deltaTime)
{
	for(auto& object : m_objects)
	{
		object->Update(deltaTime);
	}
}

void minigin::Scene::GuiRender() const
{
	for (const auto& object : m_objects)
	{
		object->GuiRender();
	}
}

void Scene::Render() const
{
	for (const auto& object : m_objects)
	{
		object->Render();
	}
}

void Scene::Cleanup()
{
	std::erase_if
	(
		m_objects,
		[](const auto& obj)
		{
			return obj->MarkedForDestruction();
		}
	);

	for (auto& object : m_objects)
	{
		object->Cleanup();
	}
}

