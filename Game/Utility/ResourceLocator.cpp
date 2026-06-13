#include "ResourceLocator.h"

#include <fstream>

#include <nlohmann/json.hpp>
using Json = nlohmann::json;

void digdug::ResourceLocator::LoadFromFile(const std::filesystem::path& path)
{
	std::ifstream fs{ path.string() };

	if (fs.fail()) return;

	m_SpriteLocations.clear();
	m_FontLocations.clear();
	m_SoundLocations.clear();

	m_RootSprites = "";
	m_RootFonts = "";
	m_RootSound = "";
	
	Json jsonFile = Json::parse(fs);

	for (auto& resources : jsonFile)
	{
		auto& typeObj = resources[m_TypeSyntax];
		Type type{};

		if (typeObj == m_SpriteSyntax) type = Type::Sprite;
		else if (typeObj == m_FontSyntax) type = Type::Font;
		else if (typeObj == m_SoundSyntax) type = Type::Sound;
		else continue;

		auto root = resources[m_RootSyntax];
		SetRoot(type, root.get<std::string>());

		for (auto& file : resources[m_FileSyntax])
		{
			auto& name = file[m_NameSyntax];
			auto& filePath = file[m_PathSyntax];

			AddResource(type, name.get<std::string>(), filePath.get<std::string>());
		}
	}
}

void digdug::ResourceLocator::SetRoot(Type resourceType, const std::filesystem::path& root)
{
	switch (resourceType)
	{
	case digdug::ResourceLocator::Type::Sprite: m_RootSprites = root;	break;
	case digdug::ResourceLocator::Type::Font:	m_RootFonts = root;		break;
	case digdug::ResourceLocator::Type::Sound:	m_RootSound = root;		break;

	default:
		break;
	}
}

void digdug::ResourceLocator::AddResource(Type resourceType, const std::string& name, const std::string& file)
{
	std::unordered_map<std::string, std::string>* map{ nullptr };
	std::filesystem::path root{};

	switch (resourceType)
	{
	case digdug::ResourceLocator::Type::Sprite:
		map = &m_SpriteLocations;
		root = m_RootSprites;
		break;

	case digdug::ResourceLocator::Type::Font:
		map = &m_FontLocations;	
		root = m_RootFonts;
		break;

	case digdug::ResourceLocator::Type::Sound:
		map = &m_SoundLocations;
		root = m_RootSound;
		break;

	default:
		throw std::runtime_error("ERROR: ResourceLocator::GetResource, so such file type!\n");
		break;
	}

	const auto path = root/file;
	map->insert_or_assign(name, path.string());
}

std::string digdug::ResourceLocator::GetResource(Type resourceType, const std::string& name) const
{
	const std::unordered_map<std::string, std::string>* map{ nullptr };
	std::filesystem::path root{};

	switch (resourceType)
	{
	case digdug::ResourceLocator::Type::Sprite:
		map = &m_SpriteLocations;
		root = m_RootSprites;
		break;

	case digdug::ResourceLocator::Type::Font:
		map = &m_FontLocations;
		root = m_RootFonts;
		break;

	case digdug::ResourceLocator::Type::Sound:
		map = &m_SoundLocations;
		root = m_RootSound;
		break;

	default:
		throw std::runtime_error("ERROR: ResourceLocator::GetResource, so such file type!\n");
		break;
	}

	auto itr = map->find(name);

	if (itr == map->end())
	{
		throw std::runtime_error("ERROR: ResourceLocator::GetResource, [" + name + "] so such file location has been registered!\n");
	}

	return itr->second;
}
