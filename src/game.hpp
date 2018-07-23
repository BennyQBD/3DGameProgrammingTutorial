#pragma once
#include "core/application.hpp"
#include "core/window.hpp"
#include "ecs/ecs.hpp"
#include "rendering/renderContext.hpp"
#include "gameEventHandler.hpp"
#include "gameRenderContext.hpp"

class Game
{
public:
	Game(Application* appIn, Window* windowIn, GameRenderContext* gameRenderContextIn) :
		app(appIn), window(windowIn), gameRenderContext(gameRenderContextIn) {}
	int loadAndRunScene(RenderDevice& device);
private:
	Application* app;
	Window* window;
	GameRenderContext* gameRenderContext;
	GameEventHandler eventHandler;
	ECS ecs;
	ECSSystemList mainSystems;
	ECSSystemList renderingPipeline;

	void gameLoop();
};


