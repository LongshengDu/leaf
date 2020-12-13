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

#ifndef _LEAF_UTILITY_H_
#define _LEAF_UTILITY_H_

#include <memory>

#include <tbb/concurrent_unordered_map.h>

namespace leaf {

class Utility
{
public:
    virtual ~Utility() = default;

private:
    friend class Application;

    static tbb::concurrent_unordered_map<int32_t, std::unique_ptr<Utility>> &Register()
    {
        static tbb::concurrent_unordered_map<int32_t, std::unique_ptr<Utility>> reg;
        return reg;
    }

    template <class _UtlT>
    static _UtlT &Call(int32_t key)
    {
        return dynamic_cast<_UtlT &>(*(Register()[key]));
    }

    template <class _UtlT>
    static void Initialize(int32_t key, _UtlT *obj)
    {
        Register()[key].reset(obj);
    }

    static void Terminate()
    {
        Register().clear();
    }
};

} // namespace leaf

#endif /* _LEAF_UTILITY_H_ */
