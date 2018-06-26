#pragma once

#include "core/common.hpp"
#include "dataStructures/array.hpp"
#include <tuple>

struct BaseECSComponent;
typedef void* EntityHandle;
typedef uint32 (*ECSComponentCreateFunction)(Array<uint8>& memory, EntityHandle entity, BaseECSComponent* comp);
typedef void (*ECSComponentFreeFunction)(BaseECSComponent* comp);
#define NULL_ENTITY_HANDLE nullptr

struct BaseECSComponent
{
public:
	static uint32 registerComponentType(ECSComponentCreateFunction createfn,
			ECSComponentFreeFunction freefn, size_t size);
	EntityHandle entity = NULL_ENTITY_HANDLE;

	inline static ECSComponentCreateFunction getTypeCreateFunction(uint32 id)
	{
		return std::get<0>((*componentTypes)[id]);
	}

	inline static ECSComponentFreeFunction getTypeFreeFunction(uint32 id)
	{
		return std::get<1>((*componentTypes)[id]);
	}

	inline static size_t getTypeSize(uint32 id)
	{
		return std::get<2>((*componentTypes)[id]);
	}

	inline static bool isTypeValid(uint32 id)
	{
		return id < componentTypes->size();
	}
private:
	static Array<std::tuple<ECSComponentCreateFunction, ECSComponentFreeFunction, size_t> >* componentTypes;
};

template<typename T>
struct ECSComponent : public BaseECSComponent
{
	static const ECSComponentCreateFunction CREATE_FUNCTION;
	static const ECSComponentFreeFunction FREE_FUNCTION;
	static const uint32 ID;
	static const size_t SIZE; 
};

template<typename Component>
uint32 ECSComponentCreate(Array<uint8>& memory, EntityHandle entity, BaseECSComponent* comp)
{
	uint32 index = memory.size();
	memory.resize(index+Component::SIZE);
	Component* component = new(&memory[index])Component(*(Component*)comp);
	component->entity = entity;
	return index;
}

template<typename Component>
void ECSComponentFree(BaseECSComponent* comp)
{
	Component* component = (Component*)comp;
	component->~Component();
}

template<typename T>
const uint32 ECSComponent<T>::ID(BaseECSComponent::registerComponentType(ECSComponentCreate<T>, ECSComponentFree<T>, sizeof(T)));

template<typename T>
const size_t ECSComponent<T>::SIZE(sizeof(T));

template<typename T>
const ECSComponentCreateFunction ECSComponent<T>::CREATE_FUNCTION(ECSComponentCreate<T>);

template<typename T>
const ECSComponentFreeFunction ECSComponent<T>::FREE_FUNCTION(ECSComponentFree<T>);

//BEGIN EXAMPLE CODE
struct TestComponent : public ECSComponent<TestComponent>
{
	float x;
	float y;
};

