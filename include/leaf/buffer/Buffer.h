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

#ifndef _LEAF_BUFFER_H_
#define _LEAF_BUFFER_H_

#include <tbb/concurrent_queue.h>

namespace leaf {

template <class _FrameT>
class Buffer
{
public:
    Buffer(size_t capacity) : _capacity(capacity)
    {
        _buffer.set_capacity(_capacity);
    }

    virtual ~Buffer()
    {
        typename _FrameT::ptr frame;

        while (try_pop(frame))
        {
            _FrameT::Dispose(frame);
        }
    }

    virtual typename _FrameT::ptr pop_frame() = 0;

    virtual bool frame_available() = 0;

    virtual bool pull_source() = 0;

    virtual bool active() = 0;

protected:
    bool try_push(typename _FrameT::ptr &frame)
    {
        return _buffer.try_push(frame);
    }

    bool try_pop(typename _FrameT::ptr &frame)
    {
        return _buffer.try_pop(frame);
    }

    size_t size()
    {
        return _buffer.size();
    }

    size_t capacity()
    {
        return _capacity;
    }

    void dispose(typename _FrameT::ptr &frame)
    {
        _FrameT::Dispose(frame);
    }

private:
    size_t _capacity;
    tbb::concurrent_bounded_queue<typename _FrameT::ptr> _buffer;
};

} // namespace leaf

#endif /* _LEAF_BUFFER_H_ */
