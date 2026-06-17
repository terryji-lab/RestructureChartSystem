#pragma once
#include "chart/Chart.h"
#include <vector>
#include <algorithm>

// 数据分析器，对 ChartItem 数据进行聚合统计和排序
// 提供计数、求和、最大/最小值、平均值、中位数等统计方法，
// 以及按值或按名称排序的实用函数
class DataAnalyzer
{
private:
    const std::vector<ChartItem>& m_data;  // 关联的数据引用（不拷贝）

public:
    DataAnalyzer(const std::vector<ChartItem>& data) : m_data(data) {}

    // 数据条目数
    int count() const { return (int)m_data.size(); }

    // 所有值的总和
    double sum() const
    {
        double s = 0;
        for (size_t i = 0; i < m_data.size(); i++)
            s += m_data[i].value;
        return s;
    }

    // 最大值
    double max() const
    {
        if (m_data.empty()) return 0;
        double m = m_data[0].value;
        for (size_t i = 1; i < m_data.size(); i++)
            if (m_data[i].value > m) m = m_data[i].value;
        return m;
    }

    // 最小值
    double min() const
    {
        if (m_data.empty()) return 0;
        double m = m_data[0].value;
        for (size_t i = 1; i < m_data.size(); i++)
            if (m_data[i].value < m) m = m_data[i].value;
        return m;
    }

    // 算术平均值
    double average() const
    {
        if (m_data.empty()) return 0;
        return sum() / m_data.size();
    }

    // 中位数
    double median() const
    {
        if (m_data.empty()) return 0;
        std::vector<double> vals;
        vals.reserve(m_data.size());
        for (size_t i = 0; i < m_data.size(); i++)
            vals.push_back(m_data[i].value);
        std::sort(vals.begin(), vals.end());
        int n = (int)vals.size();
        if (n % 2 == 1)
            return vals[n / 2];
        else
            return (vals[n / 2 - 1] + vals[n / 2]) / 2.0;
    }

    // 按值排序（asc=true 升序，asc=false 降序）
    std::vector<ChartItem> sortByValue(bool asc = true) const
    {
        std::vector<ChartItem> result = m_data;
        if (asc)
            std::sort(result.begin(), result.end(),
                [](const ChartItem& a, const ChartItem& b) { return a.value < b.value; });
        else
            std::sort(result.begin(), result.end(),
                [](const ChartItem& a, const ChartItem& b) { return a.value > b.value; });
        return result;
    }

    // 按名称排序（asc=true 字母升序，asc=false 字母降序）
    std::vector<ChartItem> sortByName(bool asc = true) const
    {
        std::vector<ChartItem> result = m_data;
        if (asc)
            std::sort(result.begin(), result.end(),
                [](const ChartItem& a, const ChartItem& b) { return a.name < b.name; });
        else
            std::sort(result.begin(), result.end(),
                [](const ChartItem& a, const ChartItem& b) { return a.name > b.name; });
        return result;
    }
};
