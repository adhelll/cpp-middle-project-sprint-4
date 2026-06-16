#include "metric_impl/code_lines_count.hpp"
#include "function.hpp"

#include <gtest/gtest.h>
#include <variant>

namespace analyzer::metric::metric_impl {

struct CodeLinesTestCase {
    std::string filename;
    int expected_count;
};

class CodeLineCountTest : public ::testing::TestWithParam<CodeLinesTestCase> {};

TEST_P(CodeLineCountTest, ValidatesCount) {
    const auto &[filename, expected_count] = GetParam();

    file::File file(filename);
    function::FunctionExtractor extractor;
    auto functions = extractor.Get(file);

    ASSERT_FALSE(functions.empty()) << "No functions extracted from file: " << filename;

    CodeLinesCountMetric metric;
    MetricResult result = metric.Calculate(functions[0]);

    ASSERT_TRUE(std::holds_alternative<int>(result.value))
        << "Metric result value is not an int for file: " << filename;

    EXPECT_EQ(std::get<int>(result.value), expected_count) << "Failed for file: " << filename;
}

INSTANTIATE_TEST_SUITE_P(PythonFiles, CodeLineCountTest,
                         ::testing::Values(CodeLinesTestCase{"comments.py", 3}, CodeLinesTestCase{"exceptions.py", 7},
                                           CodeLinesTestCase{"if.py", 3}, CodeLinesTestCase{"loops.py", 6},
                                           CodeLinesTestCase{"many_lines.py", 11},
                                           CodeLinesTestCase{"many_parameters.py", 1},
                                           CodeLinesTestCase{"match_case.py", 7}, CodeLinesTestCase{"nested_if.py", 8},
                                           CodeLinesTestCase{"simple.py", 5}, CodeLinesTestCase{"ternary.py", 1}));

}  // namespace analyzer::metric::metric_impl
