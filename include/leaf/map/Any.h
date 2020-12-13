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

    // Template Constructor
    template <typename _TypeT>
    Any(const _TypeT &val)
    {
        Cast<_TypeT>(*this) = val;
    }

    // Reset content
    void reset()
    {
        _content.reset();
    }

    // Initialize Status
    bool empty()
    {
        return (_content == nullptr);
    }

    // Template Operator =
    template <typename _TypeT>
    Any &operator=(const _TypeT &val)
    {
        Cast<_TypeT>(*this) = val;
        return *this;
    }

    // Template Operator _TypeT Casting
    template <typename _TypeT>
    operator _TypeT &()
    {
        return Cast<_TypeT>(*this);
    }

    // Template Get _TypeT Value
    template <typename _TypeT>
    static _TypeT &Cast(const Any &any)
    {
        if (any._content == nullptr)
            any._content.reset(new AnyType<_TypeT>);

        return dynamic_cast<AnyType<_TypeT> &>(*any._content).data;
    }

private:
    // Base for Pointer
    struct AnyTypeBase
    {
        virtual ~AnyTypeBase() = default;
    };

    // Store Actual Data
    template <typename _TypeT>
    struct AnyType : public AnyTypeBase
    {
        _TypeT data;
        AnyType() = default;
        explicit AnyType(const _TypeT &d) : data(d) {}
    };

    // Pointer to Base
    mutable std::shared_ptr<AnyTypeBase> _content;
};

} // namespace leaf

#endif /* _LEAF_ANY_H_ */
