#pragma once

#include <Engine/Core/CoreMinimal.h>
#include <initializer_list>

namespace StravaEngine::Core
{
template <class Type>
class ArrayList
{
public:
	ArrayList();
	explicit ArrayList(Size size);
	ArrayList(Size size, const Type& value);
	template <class IteratorType>
	vector(IteratorType first, IteratorType last);
	ArrayList(const ArrayList& arrayList);
	ArrayList(ArrayList&& arrayList);
	ArrayList(std::initializer_list<Type> initializerList);

private:
	Type* m_data = nullptr;
	Size m_size = 0;
};
}