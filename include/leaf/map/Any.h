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

#ifndef _LEAF_ANY_H_
#define _LEAF_ANY_H_

#include <memory>

namespace leaf
{

class Any
{
public:
    // Constructor
    Any() = default;

    // Template constructor
    template <typename _TypeT>
    Any(const _TypeT &val)
    {
        reset<_TypeT>(val);
    }

    // Content empty
    bool empty()
    {
        return !_content;
    }

    // Clear content
    void clear()
    {
        _content.reset();
    }

    // Template reset content
    template <typename _TypeT>
    void reset()
    {
        _content.reset(new AnyType<_TypeT>);
    }

    // Template reset content with value
    template <typename _TypeT>
    void reset(const _TypeT &val)
    {
        _content.reset(new AnyType<_TypeT>(val));
    }

    // Template operator =
    template <typename _TypeT>
    Any &operator=(const _TypeT &val)
    {
        reset<_TypeT>(val);
        return *this;
    }

    // Template operator cast to _TypeT
    template <typename _TypeT>
    operator _TypeT &()
    {
        return Cast<_TypeT>(*this);
    }

    // Template cast to _TypeT
    template <typename _TypeT>
    static _TypeT &Cast(Any &any)
    {
        auto ptr = std::dynamic_pointer_cast<AnyType<_TypeT>>(any._content);

        if (!ptr)
        {
            any.reset<_TypeT>();
        }

        return ptr -> data;
    }

    // Template cast to _TypeT const
    template <typename _TypeT>
    static _TypeT &Cast(const Any &any)
    {
        auto ptr = std::dynamic_pointer_cast<AnyType<_TypeT>>(any._content);

        if (!ptr)
        {
            throw std::bad_cast();
        }

        return ptr -> data;
    }

private:
    // Base for pointer
    struct AnyTypeBase
    {
        virtual ~AnyTypeBase() = default;
    };

    // Store actual data
    template <typename _TypeT>
    struct AnyType : public AnyTypeBase
    {
        _TypeT data;
        AnyType() = default;
        explicit AnyType(const _TypeT &d) : data(d) {}
    };

    // Pointer to base
    std::shared_ptr<AnyTypeBase> _content;
};

} // namespace leaf

#endif /* _LEAF_ANY_H_ */
