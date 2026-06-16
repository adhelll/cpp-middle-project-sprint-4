#include "metric_impl/naming_style.hpp"
#include "function.hpp"

#include <gtest/gtest.h>
#include <variant>

namespace analyzer::metric::metric_impl {

struct NamingStyleTestCase {
    std::string filename;
    std::string expected_style;
};

class NamingStyleTest : public ::testing::TestWithParam<NamingStyleTestCase> {};

TEST_P(NamingStyleTest, ValidatesStyle) {
    const auto &[filename, expected_style] = GetParam();

    file::File file(filename);
    function::FunctionExtractor extractor;
    auto functions = extractor.Get(file);

    // Гарантируем, что парсер нашел хотя бы одну функцию
    ASSERT_FALSE(functions.empty()) << "No functions extracted from file: " << filename;

    NamingStyleMetric metric;
    MetricResult result = metric.Calculate(functions[0]);

    // Безопасно проверяем, что в variant лежит именно std::string, а не int
    ASSERT_TRUE(std::holds_alternative<std::string>(result.value))
        << "Metric result value is not a string for file: " << filename;

    // Сравниваем определенный стиль со строковым эталоном
    EXPECT_EQ(std::get<std::string>(result.value), expected_style) << "Style mismatch for file: " << filename;
}

INSTANTIATE_TEST_SUITE_P(
    PythonFiles, NamingStyleTest,
    ::testing::Values(NamingStyleTestCase{"comments.py", "Unknown"}, NamingStyleTestCase{"exceptions.py", "Unknown"},
                      NamingStyleTestCase{"if.py", "Camel Case"}, NamingStyleTestCase{"loops.py", "Pascal Case"},
                      NamingStyleTestCase{"many_lines.py", "Lower Case"},
                      NamingStyleTestCase{"many_parameters.py", "Snake Case"},
                      NamingStyleTestCase{"match_case.py", "Unknown"},
                      NamingStyleTestCase{"nested_if.py", "Pascal Case"},
                      NamingStyleTestCase{"simple.py", "Snake Case"}, NamingStyleTestCase{"ternary.py", "Unknown"}));

}  // namespace analyzer::metric::metric_impl
