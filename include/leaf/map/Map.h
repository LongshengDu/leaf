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

#ifndef _LEAF_MAP_H_
#define _LEAF_MAP_H_

#include <tbb/concurrent_unordered_map.h>

#include "Any.h"

namespace leaf {

template <typename _KeyT>
class LeafMap
{
public:
    // Clear
    void clear()
    {
        _map.clear();
    }

    // Count
    size_t size()
    {
        return _map.size();
    }

    // Set
    template <typename _TypeT>
    void set(const _KeyT &key, const _TypeT &val)
    {
        _map[key] = val;
    }

    // Get
    template <typename _TypeT>
    bool get(const _KeyT &key, _TypeT &val, const _TypeT &default_val)
    {
        Any &v = _map[key];
        if (v.empty())
        {
            val = default_val;
            return false;
        }
        val = Any::Cast<_TypeT>(v);
        return true;
    }

    template <typename _TypeT>
    bool get(const _KeyT &key, _TypeT &val)
    {
        Any &v = _map[key];
        if (v.empty())
        {
            return false;
        }
        val = Any::Cast<_TypeT>(v);
        return true;
    }

    // Bind
    template <typename _TypeT>
    _TypeT *unsafe_bind(const _KeyT &key)
    {
        return &(Any::Cast<_TypeT>(_map[key]));
    }

private:
    tbb::concurrent_unordered_map<_KeyT, Any> _map;
};

} // namespace leaf

#endif /* _LEAF_MAP_H_ */
