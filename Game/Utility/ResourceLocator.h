#pragma once
#include <string>
#include <filesystem>
#include <unordered_map>

#include <Singleton.h>

namespace digdug
{
	class ResourceLocator final : public minigin::Singleton<ResourceLocator>
	{
	public:
		enum class Type { Sprite, Font, Sound };

		void LoadFromFile(const std::filesystem::path& path);

		void SetRoot(Type resourceType, const std::filesystem::path& root);
		void AddResource(Type resourceType, const std::string& name, const std::string& file);
		std::string GetResource(Type resourceType, const std::string& name) const;
		

	private:
		std::filesystem::path m_RootSprites{};
		std::filesystem::path m_RootFonts{};
		std::filesystem::path m_RootSound{};

		std::unordered_map<std::string, std::string> m_SpriteLocations{};
		std::unordered_map<std::string, std::string> m_FontLocations{};
		std::unordered_map<std::string, std::string> m_SoundLocations{};

		const std::string m_TypeSyntax{ "type" };
		const std::string m_RootSyntax{ "root" };
		const std::string m_FileSyntax{ "files" };
		const std::string m_NameSyntax{ "name" };
		const std::string m_PathSyntax{ "location" };

		const std::string m_SpriteSyntax{ "sprite" };
		const std::string m_FontSyntax{ "font" };
		const std::string m_SoundSyntax{ "sound" };
	};
}