#ifndef _SCENE_APP_H
#define _SCENE_APP_H

#include <system/application.h>
#include <maths/vector2.h>
#include "primitive_builder.h"
#include <graphics/mesh_instance.h>
#include <box2d/box2d.h>
#include "GameObject.h"
#include <vector>
#include "input/keyboard.h"
#include<input/input_manager.h>
#include <random>
// FRAMEWORK FORWARD DECLARATIONS
namespace gef
{
	class Platform;
	class SpriteRenderer;
	class Font;
	class InputManager;
	class Renderer3D;
	class keyboard;
}

class SceneApp : public gef::Application
{
public:
	SceneApp(gef::Platform& platform);
	void Init();
	void CleanUp();
	bool Update(float frame_time);
	void Render();
	
private:
	void InitFont();
	void CleanUpFont();
	void DrawHUD();
	void SetupLights();
	void createOne();
	void CreateDynamicObjects();
	float RandomFloatZeroToOne();
	float RandomFloatMinusOneToOne();
	float RandomFloat(float min, float max);
	
    
	gef::SpriteRenderer* sprite_renderer_;
	gef::Font* font_;
	gef::Renderer3D* renderer_3d_;

	PrimitiveBuilder* primitive_builder_;

	gef::MeshInstance floor_;
	gef::MeshInstance player_;

	float fps_;
	float yPos_;

	b2World *world_;
	b2Body* body_;
	b2Body* body_1_;

	gef::InputManager* input;
	std::vector<GameObject*>objects_;
};

#endif // _SCENE_APP_H
