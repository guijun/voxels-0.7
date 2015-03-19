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
#ifndef ITEM_MINERALS_H_INCLUDED
#define ITEM_MINERALS_H_INCLUDED

#include "texture/texture_atlas.h"
#include "item/builtin/image.h"

namespace programmerjake
{
namespace voxels
{
namespace Items
{
namespace builtin
{

class Coal final : public ItemImage
{
    friend class global_instance_maker<Coal>;
private:
    Coal()
        : ItemImage(L"builtin.coal", TextureAtlas::Coal.td(), nullptr)
    {
    }
public:
    static const Coal *pointer()
    {
        return global_instance_maker<Coal>::getInstance();
    }
    static ItemDescriptorPointer descriptor()
    {
        return pointer();
    }
};

class IronIngot final : public ItemImage
{
    friend class global_instance_maker<IronIngot>;
private:
    IronIngot()
        : ItemImage(L"builtin.iron_ingot", TextureAtlas::IronIngot.td(), nullptr)
    {
    }
public:
    static const IronIngot *pointer()
    {
        return global_instance_maker<IronIngot>::getInstance();
    }
    static ItemDescriptorPointer descriptor()
    {
        return pointer();
    }
};

class GoldIngot final : public ItemImage
{
    friend class global_instance_maker<GoldIngot>;
private:
    GoldIngot()
        : ItemImage(L"builtin.gold_ingot", TextureAtlas::GoldIngot.td(), nullptr)
    {
    }
public:
    static const GoldIngot *pointer()
    {
        return global_instance_maker<GoldIngot>::getInstance();
    }
    static ItemDescriptorPointer descriptor()
    {
        return pointer();
    }
};

}
}
}
}

#endif // ITEM_MINERALS_H_INCLUDED