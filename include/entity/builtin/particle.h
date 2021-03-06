/*
 * Copyright (C) 2012-2015 Jacob R. Lifshay
 * This file is part of Voxels.
 *
 * Voxels is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Voxels is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Voxels; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 */
#ifndef PARTICLE_H_INCLUDED
#define PARTICLE_H_INCLUDED

#include "entity/entity.h"
#include "physics/physics.h"
#include <vector>
#include <cmath>
#include "render/generate.h"

namespace programmerjake
{
namespace voxels
{
namespace Entities
{
namespace builtin
{

class Particle : public EntityDescriptor
{
public:
    const bool collideWithBlocks;
    const VectorF gravity;
    const float extent;
private:
    struct ParticleData final
    {
        double time = 0.0f;
    };
protected:
    std::vector<TextureDescriptor> frames;
    float framesPerSecond;
    bool loop;
    float existDuration;
    virtual ColorF colorizeColor(double time) const
    {
        return colorizeIdentity();
    }
    double &getTime(std::shared_ptr<void> data) const
    {
        assert(data != nullptr);
        return static_cast<ParticleData *>(data.get())->time;
    }
public:
    Particle(std::wstring name, std::vector<TextureDescriptor> frames, float framesPerSecond, float existDuration, bool loop = false, bool collideWithBlocks = true, VectorF gravity = VectorF(0), float extent = 1.0f / 12.0f)
        : EntityDescriptor(name), collideWithBlocks(collideWithBlocks), gravity(gravity), extent(extent), frames(std::move(frames)), framesPerSecond(framesPerSecond), loop(loop), existDuration(existDuration)
    {
    }
    virtual std::shared_ptr<PhysicsObject> makePhysicsObject(Entity &entity, World &world, PositionF position, VectorF velocity, std::shared_ptr<PhysicsWorld> physicsWorld) const override
    {
        if(!collideWithBlocks)
        {
            return PhysicsObject::makeEmpty(position, velocity, physicsWorld, gravity);
        }
        return PhysicsObject::makeCylinder(position, velocity,
                                           gravity != VectorF(0), false,
                                           extent, extent,
                                           PhysicsProperties(PhysicsProperties::blockCollisionMask, 0, 0, 1, gravity),
                                           physicsWorld);
    }
    virtual void makeData(Entity &entity, World &world, WorldLockManager &lock_manager) const override
    {
        entity.data = std::shared_ptr<void>(new ParticleData);
    }
    virtual Matrix getSelectionBoxTransform(const Entity &entity) const override
    {
        return Matrix::translate(-0.5f, -0.5f, -0.5f).concat(Matrix::scale(extent)).concat(Matrix::translate(entity.physicsObject->getPosition()));
    }
    virtual void moveStep(Entity &entity, World &world, WorldLockManager &lock_manager, double deltaTime) const override
    {
        double &time = getTime(entity.data);
        time += deltaTime;
        if(time >= existDuration)
        {
            entity.destroy();
        }
    }
    virtual void render(Entity &entity, Mesh &dest, RenderLayer rl, Matrix cameraToWorldMatrix) const override
    {
        if(rl != RenderLayer::Opaque)
            return;
        double time = getTime(entity.data);
        std::size_t frameIndex = (std::size_t)(int)std::floor(time * framesPerSecond);
        if(loop)
            frameIndex %= frames.size();
        else if(frameIndex >= frames.size())
            return;
        TextureDescriptor frame = frames[frameIndex];

        VectorF upVector = cameraToWorldMatrix.applyNoTranslate(VectorF(0, extent, 0));
        VectorF rightVector = cameraToWorldMatrix.applyNoTranslate(VectorF(extent, 0, 0));
        VectorF center = entity.physicsObject->getPosition();

        VectorF nxny = center - upVector - rightVector;
        VectorF nxpy = center + upVector - rightVector;
        VectorF pxny = center - upVector + rightVector;
        VectorF pxpy = center + upVector + rightVector;
        ColorF c = colorizeColor(time);
        dest.append(Generate::quadrilateral(frame,
                                            nxny, c,
                                            pxny, c,
                                            pxpy, c,
                                            nxpy, c));
    }
};

}
}
}
}

#endif // PARTICLE_H_INCLUDED
