#include "GameObject.h"
#include "input/input_manager.h"
#include "graphics/renderer_3d.h"
#include "graphics/material.h"
GameObject::GameObject() :
    position_(gef::Vector4::kZero),
    rotation_(gef::Vector4::kZero),
    scale_(gef::Vector4::kOne), // NOTE - scale_ doesn't default to zero
    velocity_(gef::Vector4::kZero),
    angular_velocity_(gef::Vector4::kZero),
    scale_velocity_(gef::Vector4::kZero),
    acceleration_(gef::Vector4::kZero),
    angular_acceleration_(gef::Vector4::kZero),
    scale_acceleration_(gef::Vector4::kZero)
{
    
}

GameObject::~GameObject()
{
}

void GameObject::Init(PrimitiveBuilder* primitive_builder_)
{
   player_.set_mesh(primitive_builder_->GetDefaultCubeMesh());
}

void GameObject::BuildTransform()
{
    scale.Scale(scale_);
    rotX.RotationX(rotation_.x());
    rotY.RotationY(rotation_.y());
    rotZ.RotationZ(rotation_.z());
    trans.SetIdentity();
    trans.SetTranslation(position_);

    final = scale * rotX * rotY * rotZ * trans;

    set_transform(final);

}

void GameObject::Update(float dt)
{
 //  gef::InputManager* Control=input;
    if (body_)
    {
        position_ = gef::Vector4(body_->GetPosition().x, body_->GetPosition().y, 0.0f);
        rotation_ = gef::Vector4(0.0f, 0.0f, body_->GetAngle());
    }
    else 
    {
        velocity_ += acceleration_ * dt;
        angular_velocity_ += angular_acceleration_ * dt;
        scale_velocity_ += scale_acceleration_ * dt;

        position_ += velocity_ * dt;
        rotation_ += angular_velocity_ * dt;
        scale_ += scale_velocity_ * dt;

        BuildTransform();
    }

}

void GameObject::Render(gef::Renderer3D* renderer_3d)
{
   
    renderer_3d->set_override_material(&material_);
    renderer_3d->DrawMesh(*this);
    renderer_3d->set_override_material(NULL);
}

void GameObject::SetBox2DBody(b2Body* body_1)
{
   body_ = body_1;
}


