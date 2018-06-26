#pragma once

#include "ecsComponent.hpp"

class BaseECSSystem
{
public:
	enum
	{
		FLAG_OPTIONAL = 1,
	};
	BaseECSSystem() {}
	virtual void updateComponents(float delta, BaseECSComponent** components) {}
	const Array<uint32>& getComponentTypes()
	{
		return componentTypes;
	}
	const Array<uint32>& getComponentFlags()
	{
		return componentFlags;
	}
	bool isValid();
protected:
	void addComponentType(uint32 componentType, uint32 componentFlag = 0)
	{
		componentTypes.push_back(componentType);
		componentFlags.push_back(componentFlag);
	}
private:
	Array<uint32> componentTypes;
	Array<uint32> componentFlags;
};

class ECSSystemList
{
public:
	inline bool addSystem(BaseECSSystem& system)
	{
		if(!system.isValid()) {
			return false;
		}
		systems.push_back(&system);
		return true;
	}
	inline size_t size() {
		return systems.size();
	}
	inline BaseECSSystem* operator[](uint32 index) {
		return systems[index];
	}
	bool removeSystem(BaseECSSystem& system);
private:
	Array<BaseECSSystem*> systems;
};
