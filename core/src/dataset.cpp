#include <boost/lexical_cast.hpp>
#include <utility>
#include <boost/variant.hpp>
#include <moveit_benchmark_suite/dataset.h>

using namespace moveit_benchmark_suite;

///
/// MetricVisitors
///

namespace {
class toMetricStringVisitor : public boost::static_visitor<std::string>
{
public:
  std::string operator()(int metric) const
  {
    return std::to_string(metric);
  }

  std::string operator()(double metric) const
  {
    // double v = boost::get<double>(metric);

    // [Bad Pun] No NaNs, Infs, or buts about it.
    return boost::lexical_cast<std::string>(  //
        (std::isfinite(metric)) ? metric : std::numeric_limits<double>::max());
  }

  std::string operator()(std::size_t metric) const
  {
    return std::to_string(metric);
  }

  std::string operator()(bool metric) const
  {
    return boost::lexical_cast<std::string>(metric);
  }

  std::string operator()(const std::string& metric) const
  {
    return metric;
  }

  template <typename T>
  std::string operator()(const T& metric) const
  {
    std::runtime_error("Cannot convert vector to string");
    return "";
  }
};

class toMetricDoubleVisitor : public boost::static_visitor<double>
{
public:
  double operator()(int metric) const
  {
    return static_cast<double>(metric);
  }

  double operator()(double metric) const
  {
    return metric;
  }

  double operator()(std::size_t metric) const
  {
    return static_cast<double>(metric);
  }

  double operator()(bool metric) const
  {
    return static_cast<double>(metric);
  }

  double operator()(const std::string& metric) const
  {
    return boost::lexical_cast<double>(metric);
  }

  template <typename T>
  double operator()(const T& metric) const
  {
    throw std::runtime_error("Cannot convert vector to double");
    // return std::numeric_limits<double>::quiet_NaN();
  }
};
}  // namespace

std::string moveit_benchmark_suite::toMetricString(const Metric& metric)
{
  return boost::apply_visitor(toMetricStringVisitor(), metric);
}

double moveit_benchmark_suite::toMetricDouble(const Metric& metric)
{
  return boost::apply_visitor(toMetricDoubleVisitor(), metric);
}

///
/// DataSet
///

void DataSet::addDataPoint(const std::string& query_name, const DataPtr& run)
{
  auto it = data.find(query_name);
  if (it == data.end())
  {
    DataCollection dc;
    dc.query_id = run->query->getID();

    for (const auto pair : run->metrics)
      dc.metrics.insert({ pair.first, std::vector<Metric>{ pair.second } });

    data.emplace(query_name, dc);
  }
  else
  {
    for (const auto pair : run->metrics)
    {
      auto it2 = it->second.metrics.find(pair.first);
      if (it2 == it->second.metrics.end())
        it->second.metrics.insert({ pair.first, std::vector<Metric>{ pair.second } });
      else
        it2->second.push_back(pair.second);
    }
  }
}

std::vector<DataContainer> DataSet::getFlatData() const
{
  // std::vector<DataPtr> r;
  // for (const auto& query : data)
  //   r.insert(r.end(), query.second);

  // return r;
}

void DataSet::eraseMetric(const std::string& metric)
{
  // for (const auto& data_map : data)
  //   for (const auto& d : data_map.second)
  //     d->metrics.erase(metric);
}
