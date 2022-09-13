#pragma once
#include <graphics/mesh_instance.h>
#include <graphics/material.h>
#include <box2d/box2d.h>
#include <maths/vector2.h>
#include <primitive_builder.h>
namespace gef
{
	class InputManager;
	class Renderer3D;
	
}
class GameObject :
	public gef::MeshInstance,
	public gef::Material
{
public:

	GameObject();
	~GameObject();

	gef::Vector4 scale_,rotation_,position_, velocity_, acceleration_, angular_velocity_, angular_acceleration_, scale_velocity_, scale_acceleration_;
	void Init(PrimitiveBuilder*primitive_builder_);
	void BuildTransform();
	void Update(float dt);
	void Render(gef::Renderer3D* renderer_3d);
	void SetBox2DBody(b2Body*body_1);
	

	gef::Matrix44 scale, rotX, rotY, rotZ, trans;
	gef::Matrix44 final;
	Material* material;

	gef::MeshInstance player_;
	gef::Material material_;
	b2Body* body_;
};

