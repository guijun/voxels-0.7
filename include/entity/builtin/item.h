#ifndef ITEM_H_INCLUDED
#define ITEM_H_INCLUDED

#include "entity/entity.h"
#include <unordered_map>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <random>

using namespace std;

namespace Entities
{
namespace builtin
{
class Item : public EntityDescriptor
{
protected:
    enum_array<Mesh, RenderLayer> meshes;
    Matrix preorientSelectionBoxTransform;
    static constexpr float baseSize = 0.5f, extraHeight = 0.1f;
    Item(wstring name, enum_array<Mesh, RenderLayer> meshes, Matrix preorientSelectionBoxTransform)
        : EntityDescriptor(name, PhysicsObjectConstructor::cylinderMaker(baseSize / 2, baseSize / 2 + extraHeight / 2, true, false, PhysicsProperties(PhysicsProperties::blockCollisionMask, PhysicsProperties::itemCollisionMask))), meshes(meshes), preorientSelectionBoxTransform(preorientSelectionBoxTransform)
    {
    }
    struct ItemData final
    {
        float angle = 0, bobPhase = 0;
        #warning change back item timeLeft
        double timeLeft = (5 * 60, 8);
        int8_t count = 1;
        ItemData()
        {
            minstd_rand generator((int)this);
            generator.discard(1000);
            uniform_real_distribution<float> distribution(0, 2 * M_PI);
            angle = distribution(generator);
            bobPhase = distribution(generator);
        }
    };
    static shared_ptr<ItemData> getItemData(const Entity &entity)
    {
        shared_ptr<ItemData> retval = static_pointer_cast<ItemData>(entity.data);
        if(retval == nullptr)
        {
            retval = shared_ptr<ItemData>(new ItemData);
        }
        return retval;
    }
    static shared_ptr<ItemData> getOrMakeItemData(Entity &entity)
    {
        shared_ptr<ItemData> retval = static_pointer_cast<ItemData>(entity.data);
        if(retval == nullptr)
        {
            retval = shared_ptr<ItemData>(new ItemData);
            entity.data = static_pointer_cast<void>(retval);
        }
        return retval;
    }
    static Matrix getTransform(const shared_ptr<ItemData> &data)
    {
        return Matrix::rotateY(data->angle).concat(Matrix::translate(0, extraHeight / 2 * std::sin(data->bobPhase), 0));
    }
public:
    virtual void render(const Entity &entity, Mesh &dest, RenderLayer rl) const override
    {
        shared_ptr<ItemData> data = getItemData(entity);
        dest.append(transform(getTransform(data).concat(Matrix::translate(entity.physicsObject->getPosition())), meshes[rl]));
    }
    virtual void moveStep(Entity &entity, World &world, double deltaTime) const override
    {
        shared_ptr<ItemData> data = getOrMakeItemData(entity);
        constexpr float angleSpeed = 2 * M_PI / 7.5f;
        constexpr float bobSpeed = 2.1f * angleSpeed;
        data->angle = std::fmod(data->angle + angleSpeed * (float)deltaTime, M_PI * 2);
        data->bobPhase = std::fmod(data->bobPhase + bobSpeed * (float)deltaTime, M_PI * 2);
        data->timeLeft -= deltaTime;
        if(data->timeLeft <= 0)
            entity.destroy();
        else
        {
            //cout << "Entity " << (void *)&entity << ": pos:" << (VectorF)entity.physicsObject->getPosition() << endl;
        }
    }
    virtual Matrix getSelectionBoxTransform(const Entity &entity) const override
    {
        shared_ptr<ItemData> data = getItemData(entity);
        return preorientSelectionBoxTransform.concat(getTransform(data)).concat(Matrix::translate(entity.physicsObject->getPosition()));
    }
    virtual void makeData(Entity &entity, World &world) const override
    {
        getOrMakeItemData(entity);
    }
};
}
}

#endif // ITEM_H_INCLUDED