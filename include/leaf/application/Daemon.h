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

#ifndef _LEAF_DAEMON_H_
#define _LEAF_DAEMON_H_

#include <memory>

#include <tbb/concurrent_vector.h>
#include <tbb/concurrent_unordered_map.h>

namespace leaf {

class Daemon
{
public:
    virtual ~Daemon() = default;

protected:
    virtual void start() = 0;
    virtual void stop() = 0;

private:
    friend class Application;

    static tbb::concurrent_vector<int32_t> &Indexer()
    {
        static tbb::concurrent_vector<int32_t> idx;
        return idx;
    }

    static tbb::concurrent_unordered_map<int32_t, std::unique_ptr<Daemon>> &Register()
    {
        static tbb::concurrent_unordered_map<int32_t, std::unique_ptr<Daemon>> reg;
        return reg;
    }

    template <class _DmnT>
    static _DmnT &Call(int32_t key)
    {
        return dynamic_cast<_DmnT &>(*(Register()[key]));
    }

    template <class _DmnT>
    static void Initialize(int32_t key, _DmnT *obj)
    {
        Indexer().push_back(key);
        Register()[key].reset(obj);
    }

    static void Terminate()
    {
        Register().clear();
        Indexer().clear();
    }

    static void Start()
    {
        for (auto iter = Indexer().begin(); iter != Indexer().end(); iter++)
        {
            Register().at(*iter)->start();
        }
    }

    static void Stop()
    {
        for (auto iter = Indexer().rbegin(); iter != Indexer().rend(); iter++)
        {
            Register().at(*iter)->stop();
        }
    }
};

} // namespace leaf

#endif /* _LEAF_DAEMON_H_ */
