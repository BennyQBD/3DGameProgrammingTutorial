#include "game.hpp"
#include "core/input.hpp"
#include "rendering/modelLoader.hpp"
#include "core/timing.hpp"


#include "gameCS/renderableMesh.hpp"
#include "gameCS/movementControl.hpp"
#include "gameCS/motion.hpp"
#include "gameCS/megaCube.hpp"

class TestInteraction : public Interaction
{
public:
	TestInteraction() : Interaction()
	{
//		addInteractorComponentType();
//		addInteracteeComponentType();
	}

	virtual void interact(float delta, BaseECSComponent** interactorComponents, BaseECSComponent** interacteeComponents)
	{
		DEBUG_LOG_TEMP2("Interacting!!");
	}
};

void Game::gameLoop()
{
	Color color(0.0f, 0.15f, 0.3f);
	uint32 fps = 0;
	double lastTime = Time::getTime();
	double fpsTimeCounter = 0.0;
	double updateTimer = 1.0;
	float frameTime = 1.0/60.0;
	while(app->isRunning()) {
		double currentTime = Time::getTime();
		double passedTime = currentTime - lastTime;
		lastTime = currentTime;

		fpsTimeCounter += passedTime;
		updateTimer += passedTime;

		if(fpsTimeCounter >= 1.0) {
			double msPerFrame = 1000.0/(double)fps;
			DEBUG_LOG("FPS", "NONE", "%f ms (%d fps)", msPerFrame, fps);
			fpsTimeCounter = 0;
			fps = 0;
		}
		
		bool shouldRender = false;
		while(updateTimer >= frameTime) {
			app->processMessages(frameTime, eventHandler);
			ecs.updateSystems(mainSystems, frameTime);
			interactionWorld.processInteractions(frameTime);
			updateTimer -= frameTime;
			shouldRender = true;
		}
		
		if(shouldRender) {
			gameRenderContext->clear(color, true);
			ecs.updateSystems(renderingPipeline, frameTime);
			gameRenderContext->flush();
			window->present();
			fps++;
		} else {
			Time::sleep(1);
		}
	}
}

int Game::loadAndRunScene(RenderDevice& device)
{
	// Begin scene creation
	Array<IndexedModel> models;
	Array<uint32> modelMaterialIndices;
	Array<MaterialSpec> modelMaterials;
	ModelLoader::loadModels("./res/models/monkey3.obj", models,
			modelMaterialIndices, modelMaterials);
	ModelLoader::loadModels("./res/models/tinycube.obj", models,
			modelMaterialIndices, modelMaterials);
//	IndexedModel model;
//	model.allocateElement(3); // Positions
//	model.allocateElement(2); // TexCoords
//	model.allocateElement(3); // Normals
//	model.allocateElement(3); // Tangents
//	model.setInstancedElementStartIndex(4); // Begin instanced data
//	model.allocateElement(16); // Transform matrix
//	
//	model.addElement3f(0, -0.5f, -0.5f, 0.0f);
//	model.addElement3f(0, 0.0f, 0.5f, 0.0f);
//	model.addElement3f(0, 0.5f, -0.5f, 0.0f);
//	model.addElement2f(1, 0.0f, 0.0f);
//	model.addElement2f(1, 0.5f, 1.0f);
//	model.addElement2f(1, 1.0f, 0.0f);
//	model.addIndices3i(0, 1, 2);

	VertexArray vertexArray(device, models[0], RenderDevice::USAGE_STATIC_DRAW);
	VertexArray tinyCubeVertexArray(device, models[1], RenderDevice::USAGE_STATIC_DRAW);
	
	AABB vertexArrayAABB = models[0].getAABBForElementArray(0);
	AABB tinyCubeVertexArrayAABB = models[1].getAABBForElementArray(0);

	//	ArrayBitmap bitmap;
//	bitmap.set(0,0, Color::WHITE.toInt());
//	if(!bitmap.load("./res/textures/bricks.jpg")) {
//		DEBUG_LOG("Main", LOG_ERROR, "Could not load texture!");
//		return 1;
//	}
//	Texture texture(device, bitmap, RenderDevice::FORMAT_RGB, true, false);
	DDSTexture ddsTexture;
	if(!ddsTexture.load("./res/textures/bricks.dds")) {
		DEBUG_LOG("Main", LOG_ERROR, "Could not load texture!");
		return 1;
	}
	Texture texture(device, ddsTexture);
	if(!ddsTexture.load("./res/textures/bricks2.dds")) {
		DEBUG_LOG("Main", LOG_ERROR, "Could not load texture!");
		return 1;
	}
	Texture bricks2Texture(device, ddsTexture);
	// End scene creation

	InputControl horizontal;
	InputControl vertical;
	eventHandler.addKeyControl(Input::KEY_A, horizontal, -1.0f);
	eventHandler.addKeyControl(Input::KEY_D, horizontal, 1.0f);
	eventHandler.addKeyControl(Input::KEY_LEFT, horizontal, -1.0f);
	eventHandler.addKeyControl(Input::KEY_RIGHT, horizontal, 1.0f);
	eventHandler.addKeyControl(Input::KEY_W, vertical, 1.0f);
	eventHandler.addKeyControl(Input::KEY_S, vertical, -1.0f);
	eventHandler.addKeyControl(Input::KEY_UP, vertical, 1.0f);
	eventHandler.addKeyControl(Input::KEY_DOWN, vertical, -1.0f);

	// Create components
	ColliderComponent colliderComponent;
	colliderComponent.aabb = vertexArrayAABB;

	TransformComponent transformComponent;
	transformComponent.transform.setTranslation(Vector3f(0.0f, 0.0f, 20.0f));

	MovementControlComponent movementControl;
	movementControl.movementControls.push_back(std::make_pair(Vector3f(1.0f,0.0f,0.0f) * 10.0f, &horizontal));
	movementControl.movementControls.push_back(std::make_pair(Vector3f(0.0f,1.0f,0.0f) * 10.0f, &vertical));

	RenderableMeshComponent renderableMesh;
	renderableMesh.vertexArray = &vertexArray;
	renderableMesh.texture = &texture;

	MotionComponent motionComponent;
	MegaCubeComponent megaCubeComp;
	// Create entities
	ecs.makeEntity(transformComponent, movementControl, renderableMesh, colliderComponent);
	for(uint32 i = 0; i < 1; i++) {
		transformComponent.transform.setTranslation(Vector3f(Math::randf()*10.0f-5.0f, Math::randf()*10.0f-5.0f, 20.0f));
					//Math::randf()*10.0f-5.0f + 20.0f));
		renderableMesh.vertexArray = &tinyCubeVertexArray;
		colliderComponent.aabb = tinyCubeVertexArrayAABB;
		renderableMesh.texture = Math::randf() > 0.5f ? &texture : &bricks2Texture;
		
		float vf = -4.0f;
		float af = 5.0f;
		motionComponent.acceleration = Vector3f(Math::randf(-af, af), Math::randf(-af, af), Math::randf(-af, af));
		motionComponent.velocity = motionComponent.acceleration * vf;
//		for(uint32 i = 0; i < 3; i++) {
//			megaCubeComp.pos[i] = transformComponent.transform.getTranslation()[i];
//			megaCubeComp.vel[i] = motionComponent.velocity[i];
//			megaCubeComp.acc[i] = motionComponent.acceleration[i];
//			megaCubeComp.texIndex = Math::randf() > 0.5f ? 0 : 1;
//		}
		//ecs.makeEntity(megaCubeComp);
		ecs.makeEntity(transformComponent, renderableMesh, colliderComponent);
	}
	
	// Create the systems
	MovementControlSystem movementControlSystem;
	MegaCubeMotionSystem megaCubeMotionSystem;
	Texture* textures[] = { &texture, &bricks2Texture };
	MegaCubeRenderer megaCubeRenderer(*gameRenderContext, tinyCubeVertexArray, textures, ARRAY_SIZE_IN_ELEMENTS(textures));
	MotionSystem motionSystem;
	RenderableMeshSystem renderableMeshSystem(*gameRenderContext);

	mainSystems.addSystem(movementControlSystem);
	mainSystems.addSystem(motionSystem);
	mainSystems.addSystem(megaCubeMotionSystem);
	renderingPipeline.addSystem(renderableMeshSystem);
	renderingPipeline.addSystem(megaCubeRenderer);

	// Create interactions
	TestInteraction testInteraction;
	interactionWorld.addInteraction(&testInteraction);

	gameLoop();
	return 0;
}

