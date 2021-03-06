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
#ifndef SAND_ITEM_H_INCLUDED
#define SAND_ITEM_H_INCLUDED

#include "item/builtin/block.h"
#include "util/global_instance_maker.h"

namespace programmerjake
{
namespace voxels
{
namespace Items
{
namespace builtin
{
class Sand final : public ItemBlock
{
    friend class global_instance_maker<Sand>;
private:
    Sand();
public:
    static const Sand *pointer()
    {
        return global_instance_maker<Sand>::getInstance();
    }
    static ItemDescriptorPointer descriptor()
    {
        return pointer();
    }
    virtual Item getSmeltedItem(Item item) const
    {
        #warning add glass
        return Item();
    }
};
class Gravel final : public ItemBlock
{
    friend class global_instance_maker<Gravel>;
private:
    Gravel();
public:
    static const Gravel *pointer()
    {
        return global_instance_maker<Gravel>::getInstance();
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

#endif // SAND_ITEM_H_INCLUDED
