#pragma once

#include <memory>
#include <unordered_map>

#include <Engine/Core/ArrayProxy.h>
#include <Engine/Core/ArrayList.h>

#if 0
namespace StravaEngine::Graphics
{
enum class SystemRenderTexture
{
	FinalOutput,
	Count
};

template <class KeyType, class ResourceType, std::enable_if_t<std::is_enum_v<KeyType> && ResourceType::SystemManageable::value, std::nullptr_t> = nullptr>
class SystemResourceManager
{
public:
	SystemResourceManager() {}
	~SystemResourceManager() {}

	void Reset()
	{
		m_map.clear();
	}

	ResourceType* Get(KeyType key) const
	{
		auto itr = m_map.find(key);
		if (itr == m_map.cend())
		{
			return nullptr;
		}
		return itr.second;
	}

	void Register(KeyType key, ResourceType* resource)
	{
		auto itr = m_map.find(key);
		if (itr == m_map.cend())
		{
			m_map.emplace(key, resource);
		}
	}

private:
	std::unordered_map<KeyType, ResourceType*> m_map;
};
}
#endif