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

#ifndef _LEAF_PIPELINE_H_
#define _LEAF_PIPELINE_H_

#include <vector>
#include <string>

#include <tbb/flow_graph.h>
#include <tbb/concurrent_unordered_map.h>

#include "Module.h"
#include "Statistic.h"

namespace leaf {

template <class _FrameT>
class Pipeline
{
public:
    Pipeline(int32_t max) : _max_capacity(max), _current_load(0)
    {
        // Pipline input node
        _graph_input_node.reset(new tbb::flow::broadcast_node<typename _FrameT::ptr>(_process_graph));
        // Pipline end node
        _datafrm_eol_node.reset(new tbb::flow::function_node<typename _FrameT::ptr, tbb::flow::continue_msg>(_process_graph, tbb::flow::serial, DataFrameEndOfLife(_current_load)));
    }

    virtual ~Pipeline()
    {
        // Wait for process graph finish existing pipeline works
        wait_finish();
        // Terminate all process nodes
        reset();
        // Terminate all io nodes
        _graph_input_node.reset();
        _datafrm_eol_node.reset();
    }

    // Wait for pipeline finish
    void wait_finish()
    {
        _process_graph.wait_for_all();
    }

    // Push frame into pipeline
    bool push_frame(typename _FrameT::ptr frame)
    {
        // Start Pipeline
        bool success = (frame != nullptr) && _graph_input_node->try_put(frame);
        if (success)
        {
            _current_load++;
        }
        return success;
    }

    // Check if pipeline overloaded
    bool overload()
    {
        return _current_load > _max_capacity;
    }

protected:
    // Get graph input node name
    std::string graph_input_node()
    {
        return _graph_input_name;
    }

    // Get graph end node name
    std::string dataframe_eol_node()
    {
        return _datafrm_eol_name;
    }

    // Add process node into node map
    template <class _MdulT>
    bool add_module(std::string module_name, size_t concurrency)
    {
        _module_map[module_name] = std::make_shared<_MdulT>();
        _concurrency_map[module_name] = concurrency;

        return true;
    }

    // Connect nodes in process garph
    bool connect_module(std::string module_from, std::string module_to)
    {
        if (module_from == _graph_input_name && _module_map[module_to] != nullptr)
        {
            _connection_map[_graph_input_name] = module_to;
            return true;
        }
        else if (_module_map[module_from] != nullptr && module_to == _datafrm_eol_name)
        {
            _connection_map[module_from] = _datafrm_eol_name;
            return true;
        }
        else if (_module_map[module_from] != nullptr && _module_map[module_to] != nullptr)
        {
            _connection_map[module_from] = module_to;
            return true;
        }
        else
        {
            return false;
        }
    }

    // Initialize nodes in process garph
    bool construct()
    {
        // Pipeline must connect head to tail, _connection_list.size() must greater than 2
        if (check_connection() == false)
        {
            return false;
        }

        // Initialize all module, create node
        for (auto &name : _connection_list)
        {
            if (name == _graph_input_name || name == _datafrm_eol_name)
            {
                continue;
            }

            size_t concurrency = _concurrency_map[name];
            std::shared_ptr<node_module> module = _module_map[name];

            module->post_initialize();

            _node_map[name].reset(new process_node(_process_graph, concurrency, ModuleWrapper(module, name)));
        }

        // Connect all node
        // _connection_list.size() must greater than 2
        for (size_t i = 1; i < _connection_list.size(); i++)
        {
            std::string node_from = _connection_list[i - 1];
            std::string node_to = _connection_list[i];

            if (i == 1)
            {
                tbb::flow::make_edge(*_graph_input_node, *_node_map[node_to]);
            }
            else if (i == _connection_list.size() - 1)
            {
                tbb::flow::make_edge(*_node_map[node_from], *_datafrm_eol_node);
            }
            else
            {
                tbb::flow::make_edge(*_node_map[node_from], *_node_map[node_to]);
            }
        }

        return true;
    }

    // Reset process pipeline
    void reset()
    {
        _node_map.clear();
        _module_map.clear();
        _connection_map.clear();
        _connection_list.clear();
    }

    // Check if garph node connections completed
    bool check_connection()
    {
        std::string prev;
        std::string next = _graph_input_name;

        _connection_list.clear();

        while (!next.empty())
        {
            prev = next;
            _connection_list.push_back(prev);

            next = _connection_map[next];
        }

        return (prev == _datafrm_eol_name) && (_connection_list.size() > 2);
    }

    // Get garph node connections list in ordrer
    std::vector<std::string> &connection_list()
    {
        return _connection_list;
    }

private:
    // Process node in pipeline
    typedef tbb::flow::function_node<typename _FrameT::ptr, typename _FrameT::ptr> process_node;
    // Node Module in pipeline
    typedef Module<_FrameT> node_module;

    // Data frame endpoint in pipeline. End data life cycle, release memory resources
    class DataFrameEndOfLife
    {
    private:
        tbb::atomic<int32_t> &_current_load;

    public:
        DataFrameEndOfLife(tbb::atomic<int32_t> &cpl) : _current_load(cpl) {}

        tbb::flow::continue_msg operator()(typename _FrameT::ptr frame)
        {
            _FrameT::Dispose(frame);
            --_current_load;
            return tbb::flow::continue_msg();
        }
    };

    // Wrapper class for node modules
    class ModuleWrapper
    {
    private:
        std::string _module_name;
        std::shared_ptr<node_module> _module_body;

    public:
        ModuleWrapper(std::shared_ptr<node_module> body, std::string name)
            : _module_body(body), _module_name(name)
        {
        }

        typename _FrameT::ptr operator()(typename _FrameT::ptr frame)
        {
            #ifdef MODULE_TIMMING
            tbb::tick_count t0 = tbb::tick_count::now();
            #endif /* MODULE_TIMMING */

            _module_body->update(frame);

            #ifdef MODULE_TIMMING
            float interval = 1000 * (tbb::tick_count::now() - t0).seconds();
            Statistic::RecordRuntime(_module_name, interval);
            #endif /* MODULE_TIMMING */

            return frame;
        }
    };

    // Process graph of pipeline
    tbb::flow::graph _process_graph;

    // Max capacity for pipeline to process Data frames
    int32_t _max_capacity;
    // Current load of pipeline when process Data frames
    tbb::atomic<int32_t> _current_load;

    // Map of node modules
    tbb::concurrent_unordered_map<std::string, std::shared_ptr<node_module>> _module_map;
    // Map of process nodes
    tbb::concurrent_unordered_map<std::string, std::unique_ptr<process_node>> _node_map;
    // Map of process nodes concurrency
    tbb::concurrent_unordered_map<std::string, size_t> _concurrency_map;

    // Map of process nodes connection
    tbb::concurrent_unordered_map<std::string, std::string> _connection_map;
    // List of process connection
    std::vector<std::string> _connection_list;

    // Input node of process graph
    std::unique_ptr<tbb::flow::broadcast_node<typename _FrameT::ptr>> _graph_input_node;
    // End node of process graph
    std::unique_ptr<tbb::flow::function_node<typename _FrameT::ptr, tbb::flow::continue_msg>> _datafrm_eol_node;

    // IO nodes name
    const char *_graph_input_name = "[Graph_Input]";
    const char *_datafrm_eol_name = "[DataFrame_EOL]";
};

} // namespace leaf

#endif /* _LEAF_PIPELINE_H_ */
