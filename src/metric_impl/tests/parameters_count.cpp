#include "metric_impl/parameters_count.hpp"
#include "function.hpp"

#include <gtest/gtest.h>
#include <variant>

namespace analyzer::metric::metric_impl {

struct ParametersCountTestCase {
    std::string filename;
    int expected_count;
};

class ParametersCountTest : public ::testing::TestWithParam<ParametersCountTestCase> {};

TEST_P(ParametersCountTest, ValidatesCount) {
    const auto &[filename, expected_count] = GetParam();

    file::File file(filename);
    function::FunctionExtractor extractor;
    auto functions = extractor.Get(file);

    ASSERT_FALSE(functions.empty()) << "No functions extracted from file: " << filename;

    CountParametersMetric metric;
    MetricResult result = metric.Calculate(functions[0]);

    ASSERT_TRUE(std::holds_alternative<int>(result.value))
        << "Metric result value is not an int for file: " << filename;

    EXPECT_EQ(std::get<int>(result.value), expected_count) << "Parameters count mismatch for file: " << filename;
}

INSTANTIATE_TEST_SUITE_P(
    PythonFiles, ParametersCountTest,
    ::testing::Values(ParametersCountTestCase{"comments.py", 3}, ParametersCountTestCase{"exceptions.py", 0},
                      ParametersCountTestCase{"if.py", 1}, ParametersCountTestCase{"loops.py", 1},
                      ParametersCountTestCase{"many_lines.py", 0}, ParametersCountTestCase{"many_parameters.py", 5},
                      ParametersCountTestCase{"match_case.py", 1}, ParametersCountTestCase{"nested_if.py", 2},
                      ParametersCountTestCase{"simple.py", 0}, ParametersCountTestCase{"ternary.py", 1}));

}  // namespace analyzer::metric::metric_impl
