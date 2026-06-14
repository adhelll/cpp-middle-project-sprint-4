#include "metric_impl/cyclomatic_complexity.hpp"
#include "function.hpp"

#include <gtest/gtest.h>
#include <variant>

namespace analyzer::metric::metric_impl {

struct CyclomaticComplexityTestCase {
    std::string filename;
    int expected_complexity;
};

class CyclomaticComplexityTest : public ::testing::TestWithParam<CyclomaticComplexityTestCase> {};

TEST_P(CyclomaticComplexityTest, ValidatesComplexity) {
    const auto &[filename, expected_complexity] = GetParam();

    file::File file(filename);
    function::FunctionExtractor extractor;
    auto functions = extractor.Get(file);

    ASSERT_FALSE(functions.empty()) << "No functions extracted from file: " << filename;

    CyclomaticComplexityMetric metric;
    MetricResult result = metric.Calculate(functions[0]);

    ASSERT_TRUE(std::holds_alternative<int>(result.value))
        << "Metric result value is not an int for file: " << filename;

    EXPECT_EQ(std::get<int>(result.value), expected_complexity) << "Complexity mismatch for file: " << filename;
}

INSTANTIATE_TEST_SUITE_P(
    PythonFiles, CyclomaticComplexityTest,
    ::testing::Values(CyclomaticComplexityTestCase{"comments.py", 1}, CyclomaticComplexityTestCase{"exceptions.py", 4},
                      CyclomaticComplexityTestCase{"if.py", 2}, CyclomaticComplexityTestCase{"loops.py", 4},
                      CyclomaticComplexityTestCase{"many_lines.py", 2},
                      CyclomaticComplexityTestCase{"many_parameters.py", 2},
                      CyclomaticComplexityTestCase{"match_case.py", 4}, CyclomaticComplexityTestCase{"nested_if.py", 4},
                      CyclomaticComplexityTestCase{"simple.py", 2}, CyclomaticComplexityTestCase{"ternary.py", 3}));

}  // namespace analyzer::metric::metric_impl
