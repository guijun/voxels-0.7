#ifndef BLOCK_CHUNK_H_INCLUDED
#define BLOCK_CHUNK_H_INCLUDED

#include "util/position.h"
#include "stream/stream.h"
#include "util/variable_set.h"
#include "stream/compressed_stream.h"
#include "util/basic_block_chunk.h"
#include <array>

using namespace std;

template <typename BT, typename SCT, typename CVT, size_t ChunkShiftXV = 4, size_t ChunkShiftYV = 8, size_t ChunkShiftZV = 4, size_t SubchunkShiftXYZV = 2, bool TransmitCompressedV = true>
struct BasicBlockChunk
{
    typedef BT BlockType;
    typedef SCT SubchunkType;
    typedef CVT ChunkVariablesType;
    const PositionI basePosition;
    static constexpr size_t chunkShiftX = ChunkShiftXV;
    static constexpr size_t chunkShiftY = ChunkShiftYV;
    static constexpr size_t chunkShiftZ = ChunkShiftZV;
    static constexpr size_t subchunkShiftXYZ = SubchunkShiftXYZV;
    static constexpr int32_t chunkSizeX = (int32_t)1 << chunkShiftX;
    static constexpr int32_t chunkSizeY = (int32_t)1 << chunkShiftY;
    static constexpr int32_t chunkSizeZ = (int32_t)1 << chunkShiftZ;
    static constexpr int32_t subchunkSizeXYZ = (int32_t)1 << subchunkShiftXYZ;
    static_assert(chunkSizeX > 0, "chunkSizeX must be positive");
    static_assert(chunkSizeY > 0, "chunkSizeY must be positive");
    static_assert(chunkSizeZ > 0, "chunkSizeZ must be positive");
    static_assert(subchunkSizeXYZ > 0, "subchunkSizeXYZ must be positive");
    static_assert((chunkSizeX & (chunkSizeX - 1)) == 0, "chunkSizeX must be a power of 2");
    static_assert((chunkSizeY & (chunkSizeY - 1)) == 0, "chunkSizeY must be a power of 2");
    static_assert((chunkSizeZ & (chunkSizeZ - 1)) == 0, "chunkSizeZ must be a power of 2");
    static_assert((subchunkSizeXYZ & (subchunkSizeXYZ - 1)) == 0, "subchunkSizeXYZ must be a power of 2");
    static_assert(subchunkSizeXYZ <= chunkSizeX && subchunkSizeXYZ <= chunkSizeY && subchunkSizeXYZ <= chunkSizeZ, "subchunkSizeXYZ must not be bigger than the chunk size")
    constexpr BasicBlockChunk(PositionI basePosition)
        : basePosition(basePosition)
    {
    }
    static constexpr PositionI getChunkBasePosition(PositionI pos)
    {
        return PositionI(pos.x & ~(chunkSizeX - 1), pos.y & ~(chunkSizeY - 1), pos.z & ~(chunkSizeZ - 1), pos.d);
    }
    static constexpr VectorI getChunkBasePosition(VectorI pos)
    {
        return VectorI(pos.x & ~(chunkSizeX - 1), pos.y & ~(chunkSizeY - 1), pos.z & ~(chunkSizeZ - 1));
    }
    static constexpr PositionI getChunkRelativePosition(PositionI pos)
    {
        return PositionI(pos.x & (chunkSizeX - 1), pos.y & (chunkSizeY - 1), pos.z & (chunkSizeZ - 1), pos.d);
    }
    static constexpr VectorI getChunkRelativePosition(VectorI pos)
    {
        return VectorI(pos.x & (chunkSizeX - 1), pos.y & (chunkSizeY - 1), pos.z & (chunkSizeZ - 1));
    }
    static constexpr int32_t getSubchunkBaseAbsolutePosition(int32_t v)
    {
        return v & ~(subchunkSizeXYZ - 1);
    }
    static constexpr VectorI getSubchunkBaseAbsolutePosition(VectorI pos)
    {
        return VectorI(getSubchunkBaseAbsolutePosition(pos.x), getSubchunkBaseAbsolutePosition(pos.y), getSubchunkBaseAbsolutePosition(pos.z));
    }
    static constexpr PositionI getSubchunkBaseAbsolutePosition(PositionI pos)
    {
        return PositionI(getSubchunkBaseAbsolutePosition(pos.x), getSubchunkBaseAbsolutePosition(pos.y), getSubchunkBaseAbsolutePosition(pos.z), pos.d);
    }
    static constexpr VectorI getSubchunkBaseRelativePosition(VectorI pos)
    {
        return VectorI(pos.x & ((chunkSizeX - 1) & ~(subchunkSizeXYZ - 1)), pos.y & ((chunkSizeY - 1) & ~(subchunkSizeXYZ - 1)), pos.z & ((chunkSizeZ - 1) & ~(subchunkSizeXYZ - 1)));
    }
    static constexpr VectorI getSubchunkIndexFromChunkRelativePosition(VectorI pos)
    {
        return VectorI(pos.x >> subchunkShiftXYZ, pos.y >> subchunkShiftXYZ, pos.z >> subchunkShiftXYZ);
    }
    static constexpr VectorI getSubchunkIndexFromPosition(VectorI pos)
    {
        return getSubchunkIndexFromChunkRelativePosition(getChunkRelativePosition(pos));
    }
    static constexpr PositionI getSubchunkBaseRelativePosition(PositionI pos)
    {
        return PositionI(pos.x & ((chunkSizeX - 1) & ~(subchunkSizeXYZ - 1)), pos.y & ((chunkSizeY - 1) & ~(subchunkSizeXYZ - 1)), pos.z & ((chunkSizeZ - 1) & ~(subchunkSizeXYZ - 1)), pos.d);
    }
    static constexpr int32_t getSubchunkRelativePosition(int32_t v)
    {
        return v & (subchunkSizeXYZ - 1);
    }
    static constexpr VectorI getSubchunkRelativePosition(VectorI pos)
    {
        return VectorI(getSubchunkRelativePosition(pos.x), getSubchunkRelativePosition(pos.y), getSubchunkRelativePosition(pos.z));
    }
    static constexpr PositionI getSubchunkRelativePosition(PositionI pos)
    {
        return PositionI(getSubchunkRelativePosition(pos.x), getSubchunkRelativePosition(pos.y), getSubchunkRelativePosition(pos.z), pos.d);
    }
    array<array<array<BlockType, chunkSizeZ>, chunkSizeY>, chunkSizeX> blocks;
    array<array<array<SubchunkType, subchunkSizeXYZ>, subchunkSizeXYZ>, subchunkSizeXYZ> subchunks;
    ChunkVariablesType chunkVariables;
    BasicBlockChunk(const BasicBlockChunk & rt)
        : basePosition(rt.basePosition), blocks(rt.blocks), subchunks(rt.subchunks), chunkVariables(chunkVariables)
    {
    }
};

namespace stream
{
template <typename T, size_t ChunkSizeXV, size_t ChunkSizeYV, size_t ChunkSizeZV, bool TransmitCompressedV>
struct is_value_changed<BlockChunk<T, ChunkSizeXV, ChunkSizeYV, ChunkSizeZV, TransmitCompressedV>>
{
    bool operator ()(std::shared_ptr<const BlockChunk<T, ChunkSizeXV, ChunkSizeYV, ChunkSizeZV, TransmitCompressedV>> value, VariableSet &variableSet) const
    {
        if(value == nullptr)
            return false;
        return value->changeTracker.getChanged(variableSet);
    }
};
}

#endif // BLOCK_CHUNK_H_INCLUDED
