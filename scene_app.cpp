#include "scene_app.h"
#include <system/platform.h>
#include <graphics/sprite_renderer.h>
#include <graphics/font.h>
#include <system/debug_log.h>
#include <graphics/renderer_3d.h>
#include <maths/math_utils.h>
#include <box2d/box2d.h>
#include<input/input_manager.h>

SceneApp::SceneApp(gef::Platform& platform) :
	Application(platform),
	sprite_renderer_(NULL),
	renderer_3d_(NULL),
	primitive_builder_(NULL),
	font_(NULL)
{
}

void SceneApp::Init()
{
	sprite_renderer_ = gef::SpriteRenderer::Create(platform_);

	// create the renderer for draw 3D geometry
	renderer_3d_ = gef::Renderer3D::Create(platform_);

	// initialise primitive builder to make create some 3D geometry easier
	primitive_builder_ = new PrimitiveBuilder(platform_);


	input = gef::InputManager::Create(platform_);

	// setup the mesh for the player
	player_.set_mesh(primitive_builder_->GetDefaultCubeMesh());
	floor_.set_mesh(primitive_builder_->CreateBoxMesh(gef::Vector4(10,1,3)));
	

	InitFont();
	SetupLights();

	//----------------------------MAIN CUBE SETTING
	b2Vec2 gravity(0.0f, -9.8f);
	world_ = new b2World(gravity);

	

	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position = b2Vec2(0.0f,5.0f);
	body_ = world_->CreateBody(&bodyDef);

	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(0.5f, 0.5f);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicBox;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.4f;
	b2Fixture* fixture = body_->CreateFixture(&fixtureDef);


	//--------------------------FLOOR SETTINGS
	b2BodyDef bodyDef2;
	bodyDef2.type = b2_staticBody;
	bodyDef2.position = b2Vec2(0.0f, -3.0f);
	body_1_ = world_->CreateBody(&bodyDef2);

	b2PolygonShape staticBox;
	staticBox.SetAsBox(10.0f, 1.0f);

	b2FixtureDef fixtureDef2;
	fixtureDef2.shape = &staticBox;
	fixtureDef2.density = 1.0f;
	fixtureDef2.friction = 0.4f;
	b2Fixture* fixture2 = body_1_->CreateFixture(&fixtureDef2);



}


void SceneApp::CleanUp()
{
	CleanUpFont();

	delete primitive_builder_;
	primitive_builder_ = NULL;

	delete renderer_3d_;
	renderer_3d_ = NULL;

	delete sprite_renderer_;
	sprite_renderer_ = NULL;

	delete world_;
	world_ = NULL;
}

bool SceneApp::Update(float frame_time)
{
	gef::Keyboard* keys = input->keyboard();

	fps_ = 1.0f / frame_time;
	int velocityIterations = 6;
	int positionIterations = 2;
	world_->Step(frame_time, velocityIterations, positionIterations);//WORLD CREATION

	//----------------------------------PLAYER ATTACHED TO BODY
	yPos_ = body_->GetTransform().p.y;
	if (keys->IsKeyDown(keys->KC_Q))	body_->ApplyForceToCenter(b2Vec2(-10.0f, 0.0f), true);
	if (keys->IsKeyDown(keys->KC_E))	body_->ApplyLinearImpulse(b2Vec2(0.0f, 3.0f), b2Vec2(0.5f, 0.0f), true);
	
	
	gef::Matrix44 player_transform;
	player_transform.SetIdentity();
	player_transform.RotationZ(body_->GetAngle()); 
	player_transform.SetTranslation(gef::Vector4(body_->GetPosition().x,
									body_->GetPosition().y,
									0.0f));
	CreateDynamicObjects();
	
	player_.set_transform(player_transform);

	gef::Matrix44 floor_transform;
	floor_transform.SetIdentity();
	floor_transform.SetTranslation(gef::Vector4(0, -3, 0));
	floor_.set_transform(floor_transform);

	for (int i=0;i<objects_.size();i++)
	{
		objects_[i]->Update(frame_time);
	}

	input->Update();
	return true;
	
}

void SceneApp::Render()
{
	// setup camera

	// projection
	float fov = gef::DegToRad(45.0f);
	float aspect_ratio = (float)platform_.width() / (float)platform_.height();
	gef::Matrix44 projection_matrix;
	projection_matrix = platform_.PerspectiveProjectionFov(fov, aspect_ratio, 0.1f, 100.0f);
	renderer_3d_->set_projection_matrix(projection_matrix);

	// view
	gef::Vector4 camera_eye(-2.0f, 2.0f, 15.0f);
	gef::Vector4 camera_lookat(0.0f, 0.0f, 0.0f);
	gef::Vector4 camera_up(0.0f, 1.0f, 0.0f);
	gef::Matrix44 view_matrix;
	view_matrix.LookAt(camera_eye, camera_lookat, camera_up);
	renderer_3d_->set_view_matrix(view_matrix);


	// draw 3d geometry
	renderer_3d_->Begin();

	renderer_3d_->set_override_material(&primitive_builder_->red_material());

	renderer_3d_->DrawMesh(floor_);
	renderer_3d_->DrawMesh(player_);
	for(auto )
	
	//renderer_3d_->DrawMesh(objects_);
	renderer_3d_->set_override_material(NULL);

	renderer_3d_->End();

	// start drawing sprites, but don't clear the frame buffer
	sprite_renderer_->Begin(false);
	DrawHUD();
	sprite_renderer_->End();
}



void SceneApp::InitFont()
{
	font_ = new gef::Font(platform_);
	font_->Load("comic_sans");
}

void SceneApp::CleanUpFont()
{
	delete font_;
	font_ = NULL;
}

void SceneApp::DrawHUD()
{
	if(font_)
	{
		// display frame rate
		font_->RenderText(sprite_renderer_, 
						  gef::Vector4(400.0f, 510.0f, -0.9f),
						  1.0f, 0xffffffff,
						  gef::TJ_LEFT, 
						  "YPOS: %.1f FPS: %.1f", 
						  yPos_,
						  fps_);
	}
}

void SceneApp::SetupLights()
{
	// grab the data for the default shader used for rendering 3D geometry
	gef::Default3DShaderData& default_shader_data = renderer_3d_->default_shader_data();

	// set the ambient light
	default_shader_data.set_ambient_light_colour(gef::Colour(0.25f, 0.25f, 0.25f, 1.0f));

	// add a point light that is almost white, but with a blue tinge
	// the position of the light is set far away so it acts light a directional light
	gef::PointLight default_point_light;
	default_point_light.set_colour(gef::Colour(0.7f, 0.7f, 1.0f, 1.0f));
	default_point_light.set_position(gef::Vector4(-500.0f, 400.0f, 700.0f));
	default_shader_data.AddPointLight(default_point_light);
}

void SceneApp::CreateDynamicObjects()
{
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position = b2Vec2(RandomFloat(-1.0f, 1.0f), RandomFloat(0.0f, 10.0f));
	b2Body* body = world_->CreateBody(&bodyDef);

	float boxX = RandomFloat(0.1f, 0.7f);
	float boxY = RandomFloat(0.1f, 0.7f);

	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(boxX * 0.5f, boxY * 0.5f);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicBox;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.3f;
	b2Fixture* fixture = body->CreateFixture(&fixtureDef);

	GameObject* object = new GameObject;
	object->Init(primitive_builder_);
	object->scale_ = gef::Vector4(boxX, boxY, 1.0f);
	object->SetBox2DBody(body);

	objects_.push_back(object);


	
}

float SceneApp::RandomFloatZeroToOne()
{
	return rand() / (RAND_MAX + 1.0f);
}
float SceneApp::RandomFloatMinusOneToOne()
{
	return (RandomFloatZeroToOne() * 2.0f) - 1.0f;
}

float SceneApp::RandomFloat(float min, float max)
{
	return min + RandomFloatZeroToOne() * (max - min);
}