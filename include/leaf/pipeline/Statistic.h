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

#ifndef _LEAF_STATISTIC_H_
#define _LEAF_STATISTIC_H_

#include <deque>
#include <vector>
#include <string>
#include <limits>
#include <sstream>
#include <typeinfo>

#include <math.h>
#include <stdint.h>
#include <inttypes.h>

#include <tbb/tick_count.h>
#include <tbb/concurrent_unordered_map.h>

// #define MODULE_TIMMING

namespace leaf {

struct ModuleStatistic
{
    std::string name;
    float avg;
    float std;
    float min;
    float max;
    int64_t count;
};

class Statistic
{
public:
    static int32_t &MaxRecordLength()
    {
        static int32_t max_record_length = 500;
        return max_record_length;
    }

    static void StartRecording()
    {
        RuntimeMap().clear();
        Recording() = true;
    }

    static void StopRecording()
    {
        Recording() = false;
    }

    static void RecordRuntime(std::string name, float interval)
    {
        if (Recording())
        {
            RuncountMap()[name] = RuncountMap()[name] + 1;

            std::deque<float> &q = RuntimeMap()[name];

            q.push_back(interval);

            if (q.size() > MaxRecordLength())
                q.pop_front();
        }
    }

    static std::vector<ModuleStatistic> &GetStatistic()
    {
        static std::vector<ModuleStatistic> stats;

        stats.clear();

        for (auto &pair : RuntimeMap())
        {
            auto &q = pair.second;

            float sum = 0;
            float sum2 = 0;
            float min = std::numeric_limits<float>::max();
            float max = -std::numeric_limits<float>::max();

            for (size_t i = 0; i < q.size(); i++)
            {
                sum += q[i];
                sum2 += q[i] * q[i];

                if (q[i] > max)
                    max = q[i];
                if (q[i] < min)
                    min = q[i];
            }

            float avg = sum / q.size();
            float avg2 = sum2 / q.size();
            float std = sqrt(avg2 - avg * avg);

            int64_t count = RuncountMap()[pair.first];

            stats.push_back({pair.first, avg, std, min, max, count});
        }
        return stats;
    }

    static void PrintStatistic()
    {
        auto &stats = GetStatistic();

        printf("================= Module Runtime (ms) ====================\n");
        printf("Name                  Average   StdDev   Min   Max   Count\n");
        printf("----------------------------------------------------------\n");
        for (auto &module : stats)
        {
            printf("%s  %.4f  %.4f  %.4f  %.4f  %" PRId64 "\n", module.name.c_str(),
                    module.avg, module.std, module.min, module.max, module.count);
        }
        printf("================= Module Runtime (ms) ====================\n");
    }

private:
    static tbb::concurrent_unordered_map<std::string, std::deque<float>> &RuntimeMap()
    {
        static tbb::concurrent_unordered_map<std::string, std::deque<float>> runtime_map;
        return runtime_map;
    }

    static tbb::concurrent_unordered_map<std::string, int64_t> &RuncountMap()
    {
        static tbb::concurrent_unordered_map<std::string, int64_t> runcount_map;
        return runcount_map;
    }

    static bool &Recording()
    {
        static bool recording = true;
        return recording;
    }
};

} // namespace leaf

#endif /* _LEAF_STATISTIC_H_ */
