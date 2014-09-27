/*
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
#ifndef BLOCK_H_INCLUDED
#define BLOCK_H_INCLUDED

#include <cstdint>
#include <memory>
#include <cwchar>
#include <string>
#include <cassert>
#include <tuple>
#include <stdexcept>
#include <iterator>
#include <utility>
#include "util/linked_map.h"
#include "util/vector.h"
#include "render/mesh.h"
#include "util/block_face.h"
#include "render/render_layer.h"

using namespace std;

class BlockDescriptor;

typedef const BlockDescriptor *BlockDescriptorPointer;

struct Block final
{
    BlockDescriptorPointer descriptor;
    shared_ptr<void> data;
    Block()
        : descriptor(nullptr), data(nullptr)
    {
    }
    Block(BlockDescriptorPointer descriptor, shared_ptr<void> data = nullptr)
        : descriptor(descriptor), data(data)
    {
    }
    bool good() const
    {
        return descriptor != nullptr;
    }
    explicit operator bool() const
    {
        return good();
    }
    bool operator !() const
    {
        return !good();
    }
    bool operator ==(const Block &r) const;
    bool operator !=(const Block &r) const
    {
        return !operator ==(r);
    }
};

struct BlockShape final
{
    VectorF offset, extents;
    BlockShape()
        : offset(0), extents(-1)
    {
    }
    BlockShape(std::nullptr_t)
        : BlockShape()
    {
    }
    BlockShape(VectorF offset, VectorF extents)
        : offset(offset), extents(extents)
    {
        assert(extents.x > 0 && extents.y > 0 && extents.z > 0);
    }
    bool empty() const
    {
        return extents.x <= 0;
    }
};

#include "util/block_iterator.h"

class BlockDescriptor
{
    BlockDescriptor(const BlockDescriptor &) = delete;
    void operator =(const BlockDescriptor &) = delete;
public:
    const wstring name;
protected:
    BlockDescriptor(wstring name, BlockShape blockShape, bool isStaticMesh, bool isFaceBlockedNX, bool isFaceBlockedPX, bool isFaceBlockedNY, bool isFaceBlockedPY, bool isFaceBlockedNZ, bool isFaceBlockedPZ, Mesh meshCenter, Mesh meshFaceNX, Mesh meshFacePX, Mesh meshFaceNY, Mesh meshFacePY, Mesh meshFaceNZ, Mesh meshFacePZ, RenderLayer staticRenderLayer);
    BlockDescriptor(wstring name, BlockShape blockShape, bool isFaceBlockedNX, bool isFaceBlockedPX, bool isFaceBlockedNY, bool isFaceBlockedPY, bool isFaceBlockedNZ, bool isFaceBlockedPZ)
        : BlockDescriptor(name, blockShape, false, isFaceBlockedNX, isFaceBlockedPX, isFaceBlockedNY, isFaceBlockedPY, isFaceBlockedNZ, isFaceBlockedPZ, Mesh(), Mesh(), Mesh(), Mesh(), Mesh(), Mesh(), Mesh(), RenderLayer::Opaque)
    {
    }
public:
    virtual ~BlockDescriptor();
    const BlockShape blockShape;
    const bool isStaticMesh;
    enum_array<bool, BlockFace> isFaceBlocked;
protected:
    /** generate dynamic mesh
     the generated mesh is at the absolute position of the block
     */
    virtual void renderDynamic(const Block &block, Mesh &dest, BlockIterator blockIterator, RenderLayer rl) const
    {
        assert(false); // shouldn't be called
    }
    Mesh meshCenter;
    enum_array<Mesh, BlockFace> meshFace;
    const RenderLayer staticRenderLayer;
public:
    /** generate mesh
     the generated mesh is at the absolute position of the block
     */
    void render(const Block &block, Mesh &dest, BlockIterator blockIterator, RenderLayer rl) const
    {
        if(isStaticMesh)
        {
            if(rl != staticRenderLayer)
                return;
            bool drewAny = false;
            Matrix tform = Matrix::translate((VectorF)blockIterator.position());
            for(BlockFace bf : enum_traits<BlockFace>())
            {
                BlockIterator i = blockIterator;
                i.moveToward(bf);
                if(!*i)
                    continue;
                if(i->descriptor->isFaceBlocked[getOppositeBlockFace(bf)])
                    continue;
                dest.append(transform(tform, meshFace[bf]));
                drewAny = true;
            }
            if(drewAny)
                dest.append(transform(tform, meshCenter));
        }
        else
        {
            renderDynamic(block, dest, blockIterator, rl);
        }
    }
    virtual Block moveStep(const Block &block, BlockIterator blockIterator) const
    {
        return block;
    }
    virtual bool isDataEqual(const shared_ptr<void> &a, const shared_ptr<void> &b) const
    {
        return a == b;
    }
    virtual size_t hashData(const shared_ptr<void> &data) const
    {
        return std::hash<shared_ptr<void>>()(data);
    }
};

inline bool Block::operator==(const Block &r) const
{
    if(descriptor != r.descriptor)
        return false;
    if(descriptor == nullptr)
        return true;
    if(data == r.data)
        return true;
    return descriptor->isDataEqual(data, r.data);
}

namespace std
{
template <>
struct hash<Block> final
{
    size_t operator ()(const Block &b) const
    {
        if(b.descriptor == nullptr)
            return 0;
        return std::hash<BlockDescriptorPointer>()(b.descriptor) + b.descriptor->hashData(b.data);
    }
};
}

class BlockDescriptors_t final
{
    friend class BlockDescriptor;
private:
    typedef linked_map<wstring, BlockDescriptorPointer> MapType;
    static MapType *blocksMap;
    void add(BlockDescriptorPointer bd) const;
    void remove(BlockDescriptorPointer bd) const;
public:
    BlockDescriptorPointer operator [](wstring name) const
    {
        if(blocksMap == nullptr)
            return nullptr;
        auto iter = blocksMap->find(name);
        if(iter == blocksMap->end())
            return nullptr;
        return std::get<1>(*iter);
    }
    BlockDescriptorPointer at(wstring name) const
    {
        BlockDescriptorPointer retval = operator [](name);
        if(retval == nullptr)
            throw out_of_range("BlockDescriptor not found");
        return retval;
    }
    class iterator final : public std::iterator<std::forward_iterator_tag, const BlockDescriptorPointer>
    {
        friend class BlockDescriptors_t;
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
                return r.empty;
            if(r.empty)
                return false;
            return iter == r.iter;
        }
        bool operator !=(const iterator &r) const
        {
            return !operator ==(r);
        }
        const BlockDescriptorPointer &operator *() const
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
        if(blocksMap == nullptr)
            return iterator();
        return iterator(blocksMap->cbegin());
    }
    iterator end() const
    {
        if(blocksMap == nullptr)
            return iterator();
        return iterator(blocksMap->cend());
    }
    iterator cbegin() const
    {
        return begin();
    }
    iterator cend() const
    {
        return end();
    }
    iterator find(wstring name) const
    {
        if(blocksMap == nullptr)
            return iterator();
        return iterator(blocksMap->find(name));
    }
    size_t size() const
    {
        if(blocksMap == nullptr)
            return 0;
        return blocksMap->size();
    }
};

static constexpr BlockDescriptors_t BlockDescriptors;

#endif // BLOCK_H_INCLUDED