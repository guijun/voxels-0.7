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
#ifndef ITEM_H_INCLUDED
#define ITEM_H_INCLUDED

#include "item/item_struct.h"
#include "util/linked_map.h"
#include <string>
#include "render/mesh.h"
#include "util/position.h"
#include "util/block_face.h"

namespace programmerjake
{
namespace voxels
{
class World;
class WorldLockManager;
class Entity;
class Player;
class ItemDescriptor
{
    ItemDescriptor(const ItemDescriptor &) = delete;
    void operator =(const ItemDescriptor &) = delete;
public:
    const std::wstring name;
protected:
    explicit ItemDescriptor(std::wstring name);
public:
    ~ItemDescriptor();
    static constexpr float minRenderZ = 0.75f, maxRenderZ = 1.0f;
    virtual void render(Item item, Mesh &dest, float minX, float maxX, float minY, float maxY) const = 0;
    virtual bool dataEqual(std::shared_ptr<void> data1, std::shared_ptr<void> data2) const = 0;
    virtual Entity *dropAsEntity(Item item, World &world, WorldLockManager &lock_manager, Player &player) const = 0;
    virtual Item onUse(Item item, World &world, WorldLockManager &lock_manager, Player &player) const = 0;
    virtual Item onDispenseOrDrop(Item item, World &world, WorldLockManager &lock_manager, PositionI dispensePosition, VectorF dispenseDirection, bool useSpecialAction) const = 0;
};

inline bool Item::dataEqual(const Item &rt) const
{
    return descriptor->dataEqual(data, rt.data);
}

class ItemDescriptors_t final
{
    friend class ItemDescriptor;
private:
    typedef linked_map<std::wstring, ItemDescriptorPointer> MapType;
    static MapType *itemsMap;
    void add(ItemDescriptorPointer bd) const;
    void remove(ItemDescriptorPointer bd) const;
public:
    constexpr ItemDescriptors_t() noexcept
    {
    }
    ItemDescriptorPointer operator [](std::wstring name) const
    {
        if(itemsMap == nullptr)
        {
            return nullptr;
        }

        auto iter = itemsMap->find(name);

        if(iter == itemsMap->end())
        {
            return nullptr;
        }

        return std::get<1>(*iter);
    }
    ItemDescriptorPointer at(std::wstring name) const
    {
        ItemDescriptorPointer retval = operator [](name);

        if(retval == nullptr)
        {
            throw std::out_of_range("BlockDescriptor not found");
        }

        return retval;
    }
    class iterator final : public std::iterator<std::forward_iterator_tag, const ItemDescriptorPointer>
    {
        friend class ItemDescriptors_t;
        MapType::const_iterator iter;
        bool empty;
        explicit iterator(MapType::const_iterator iter)
            : iter(iter), empty(false)
        {
        }
    public:
        iterator()
            : iter(), empty(true)
        {
        }
        bool operator ==(const iterator &r) const
        {
            if(empty)
            {
                return r.empty;
            }

            if(r.empty)
            {
                return false;
            }

            return iter == r.iter;
        }
        bool operator !=(const iterator &r) const
        {
            return !operator ==(r);
        }
        const ItemDescriptorPointer &operator *() const
        {
            return std::get<1>(*iter);
        }
        const iterator &operator ++()
        {
            ++iter;
            return *this;
        }
        iterator operator ++(int)
        {
            return iterator(iter++);
        }
    };
    iterator begin() const
    {
        if(itemsMap == nullptr)
        {
            return iterator();
        }

        return iterator(itemsMap->cbegin());
    }
    iterator end() const
    {
        if(itemsMap == nullptr)
        {
            return iterator();
        }

        return iterator(itemsMap->cend());
    }
    iterator cbegin() const
    {
        return begin();
    }
    iterator cend() const
    {
        return end();
    }
    iterator find(std::wstring name) const
    {
        if(itemsMap == nullptr)
        {
            return iterator();
        }

        return iterator(itemsMap->find(name));
    }
    std::size_t size() const
    {
        if(itemsMap == nullptr)
        {
            return 0;
        }

        return itemsMap->size();
    }
};

static constexpr ItemDescriptors_t ItemDescriptors;
}
}

#endif // ITEM_H_INCLUDED