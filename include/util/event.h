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
#ifndef UTIL_EVENT_H_INCLUDED
#define UTIL_EVENT_H_INCLUDED

#include <functional>
#include <list>
#include "util/enum_traits.h"

namespace programmerjake
{
namespace voxels
{
struct EventArguments
{
    virtual ~EventArguments()
    {
    }
};

class Event
{
public:
    enum ReturnType
    {
        Discard,
        Propagate,
        DEFINE_ENUM_LIMITS(Discard, Propagate)
    };
    enum class Priority
    {
        Low,
        High,
        Default = Low,
        DEFINE_ENUM_LIMITS(Low, High)
    };
private:
    std::list<std::function<ReturnType (EventArguments &args)>> functions;
public:
    explicit Event(std::function<ReturnType (EventArguments &args)> fn = nullptr)
    {
        bind(fn);
    }
    Event(Event &&rt)
        : functions(std::move(rt.functions))
    {
    }
    Event(const Event &) = delete;
    const Event &operator =(Event &&rt)
    {
        functions = std::move(rt.functions);
        return *this;
    }
    const Event &operator =(const Event &) = delete;
    void bind(std::function<ReturnType (EventArguments &args)> fn, Priority p = Priority::Default)
    {
        if(fn != nullptr)
        {
            if(p == Priority::High)
                functions.push_front(fn);
            else
                functions.push_back(fn);
        }
    }
    ReturnType operator ()(EventArguments &args)
    {
        for(auto fn : functions)
        {
            ReturnType retval = fn(args);
            if(retval != Propagate)
                return retval;
        }
        return Propagate;
    }
};

}
}

#endif // UTIL_EVENT_H_INCLUDED
