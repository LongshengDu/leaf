/*
 * License Agreement
 * 
 * Copyright (c) 2020 Longsheng Du
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef _LEAF_SECTION_MAP_H_
#define _LEAF_SECTION_MAP_H_

#include <tbb/concurrent_unordered_map.h>

#include "Any.h"

namespace leaf {

template <typename _SecT, typename _KeyT>
class SectionMap
{
public:
    // Clear
    void clear()
    {
        _section_map.clear();
    }

    // Count
    size_t size()
    {
        return _section_map.size();
    }

    // Count All
    size_t size_sum()
    {
        size_t s = 0;
        for (auto &map_vk : _section_map)
        {
            s += map_vk.second.size();
        }
        return s;
    }

    // Set
    template <typename _TypeT>
    void set(const _SecT &section, const _KeyT &key, const _TypeT &val)
    {
        _section_map[section][key] = val;
    }

    // Get
    template <typename _TypeT>
    bool get(const _SecT &section, const _KeyT &key, _TypeT &val, const _TypeT &default_val)
    {
        Any &v = _section_map[section][key];
        if (v.empty())
        {
            val = default_val;
            return false;
        }
        val = Any::Cast<_TypeT>(v);
        return true;
    }

    template <typename _TypeT>
    bool get(const _SecT &section, const _KeyT &key, _TypeT &val)
    {
        Any &v = _section_map[section][key];
        if (v.empty())
        {
            return false;
        }
        val = Any::Cast<_TypeT>(v);
        return true;
    }

    // Bind
    template <typename _TypeT>
    _TypeT *unsafe_bind(const _SecT &section, const _KeyT &key)
    {
        return &(Any::Cast<_TypeT>(_section_map[section][key]));
    }

private:
    tbb::concurrent_unordered_map<_SecT, tbb::concurrent_unordered_map<_KeyT, Any>> _section_map;
};

} // namespace leaf

#endif /* _LEAF_SECTION_MAP_H_ */
