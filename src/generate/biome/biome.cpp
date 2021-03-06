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
#include "generate/biome/biome_descriptor.h"
#include "generate/decorator/mineral_vein.h"
#include "generate/decorator/tree.h"
#include <algorithm>
#include <cmath>

namespace programmerjake
{
namespace voxels
{
linked_map<std::wstring, BiomeDescriptorPointer> *BiomeDescriptors_t::pBiomeNameMap = nullptr;
std::vector<BiomeDescriptorPointer> *BiomeDescriptors_t::pBiomeVector = nullptr;

BiomeProperties::BiomeProperties(BiomeWeights weights)
    : weights(weights)
{
    weights.normalize();
    grassColor = calcColorInternal<&BiomeDescriptor::grassColor>();
    leavesColor = calcColorInternal<&BiomeDescriptor::leavesColor>();
    waterColor = calcColorInternal<&BiomeDescriptor::waterColor>();
    float maxWeight = -1;
    dominantBiome = nullptr;
    for(const BiomeWeights::value_type &v : weights)
    {
        float weight = std::get<1>(v);
        BiomeDescriptorPointer biome = std::get<0>(v);
        if(weight > maxWeight)
        {
            dominantBiome = biome;
            maxWeight = weight;
        }
    }
    good = true;
}

BiomeIndex getBiomeIndex(BiomeDescriptorPointer biome)
{
    return biome->getIndex();
}

void BiomeDescriptors_t::addBiome(BiomeDescriptor *biome)
{
    if(pBiomeVector == nullptr)
    {
        pBiomeNameMap = new linked_map<std::wstring, BiomeDescriptorPointer>;
        pBiomeVector = new std::vector<BiomeDescriptorPointer>;
    }
    biome->index = pBiomeVector->size();
    pBiomeVector->push_back(biome);
    assert(0 == pBiomeNameMap->count(biome->name));
    pBiomeNameMap->insert(linked_map<std::wstring, BiomeDescriptorPointer>::value_type(biome->name, biome));
}


BiomeWeights BiomeDescriptors_t::getBiomeWeights(float temperature, float humidity, PositionI pos, RandomSource &randomSource) const
{
    BiomeWeights retval;
    for(BiomeWeights::value_type &v : retval)
    {
        std::get<1>(v) = std::max<float>(0, std::get<0>(v)->getBiomeCorrespondence(temperature, humidity, pos, randomSource));
    }
    retval.normalize();
    for(BiomeWeights::value_type &v : retval)
    {
        std::get<1>(v) = std::pow(std::get<1>(v), 20);
    }
    retval.normalize();
    return std::move(retval);
}

float BiomeDescriptor::getChunkDecoratorCount(DecoratorDescriptorPointer descriptor) const
{
    const Decorators::builtin::MineralVeinDecorator *mineralVeinDecorator = dynamic_cast<const Decorators::builtin::MineralVeinDecorator *>(descriptor);
    if(mineralVeinDecorator != nullptr)
        return mineralVeinDecorator->defaultPreChunkGenerateCount;
    const Decorators::builtin::TreeDecorator *treeDecorator = dynamic_cast<const Decorators::builtin::TreeDecorator *>(descriptor);
    if(treeDecorator != nullptr)
        return treeDecorator->getChunkDecoratorCount(this);
    return 0;
}
}
}
