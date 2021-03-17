#pragma once

#if ANK_DEBUG_INTERFACE

#include "Core/Utils/InterfaceGfxDbg/AnkDebugInterface.h"
#include "Core/ECS/Components/ANKComponent.h"

/*
	Register component for access in AnkDebugInterface
*/
#define ANK_REG_COMP_INTERFACE(componentName) ANK_REG_COMP(componentName)

/*
	Register variable for access in AnkDebugInterface
*/
#define ANK_REG_VAR_INTERFACE(type, name) ANK_COMP_VAR(type, name)

#else

#define ANK_REG_COMP_INTERFACE(componentName) ANK_REG_COMP(componentName)
#define ANK_REG_COMP_INTERFACE_VARIABLE(type, name) ANK_COMP_VAR(type, name)

#endif