#include "ecsSystem.hpp"

bool BaseECSSystem::isValid()
{
	for(uint32 i = 0;  i < componentFlags.size(); i++) {
		if((componentFlags[i] & BaseECSSystem::FLAG_OPTIONAL) == 0) {
			return true;
		}
	}
	return false;
}

bool ECSSystemList::removeSystem(BaseECSSystem& system)
{
	for(uint32 i = 0; i < systems.size(); i++) {
		if(&system == systems[i]) {
			systems.erase(systems.begin() + i);
			return true;
		}
	}
	return false;
}

