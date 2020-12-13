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

#ifndef _LEAF_APPLICATION_H_
#define _LEAF_APPLICATION_H_

#include <memory>

#include "Utility.h"
#include "Daemon.h"

namespace leaf {

class Application
{
public:
    virtual ~Application() = default;

    // Interact with Utility
    template <class _UtlT>
    static void InitializeUtility(int32_t key, _UtlT *obj)
    {
        Utility::Initialize(key, obj);
    }

    template <class _UtlT>
    static _UtlT &CallUtility(int32_t key)
    {
        return Utility::Call(key);
    }

    // Interact with Daemon
    template <class _DmnT>
    static void InitializeDaemon(int32_t key, _DmnT *obj)
    {
        Daemon::Initialize<_DmnT>(key, obj);
    }

    template <class _DmnT>
    static _DmnT &CallDaemon(int32_t key)
    {
        return Daemon::Call(key);
    }

    // Interact with Application
    template <class _AppT>
    static void Run()
    {
        Application::Initialize<_AppT>();

        Daemon::Start();

        CurrentApplication()->run();
        CurrentApplication()->end();

        Daemon::Stop();

        Application::Terminate();
        Daemon::Terminate();
        Utility::Terminate();
    }

protected:
    virtual void run() = 0;
    virtual void end() = 0;

private:
    static std::unique_ptr<Application> &CurrentApplication()
    {
        static std::unique_ptr<Application> reg;
        return reg;
    }

    template <class _AppT>
    static void Initialize()
    {
        CurrentApplication().reset(new _AppT());
    }

    static void Terminate()
    {
        CurrentApplication().reset();
    }
};

} // namespace leaf

#endif /* _LEAF_APPLICATION_H_ */
