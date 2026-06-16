#include "metric_accumulator_impl/categorical_accumulator.hpp"

#include <unistd.h>

#include <algorithm>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <ranges>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

namespace analyzer::metric_accumulator::metric_accumulator_impl {

void CategoricalAccumulator::Accumulate(const metric::MetricResult &metric_result) {
    if (!std::holds_alternative<std::string>(metric_result.value)) {
        throw std::runtime_error("CategoricalAccumulator::Accumulate() error: Expected std::string inside "
                                 "metric_result.value for metric '" +
                                 metric_result.metric_name + "', but got another type.");
    }
    categories_freq[std::get<std::string>(metric_result.value)]++;
}

void CategoricalAccumulator::Finalize() { is_finalized = true; }

void CategoricalAccumulator::Reset() {
    is_finalized = false;
    categories_freq.clear();
}

const std::unordered_map<std::string, int> &CategoricalAccumulator::Get() const {
    if (!is_finalized)
        throw std::runtime_error("CategoricalAccumulator::Get() called before Finalize()");
    return categories_freq;
}
}  // namespace analyzer::metric_accumulator::metric_accumulator_impl
