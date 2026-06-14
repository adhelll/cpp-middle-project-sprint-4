#include "metric_accumulator_impl/sum_average_accumulator.hpp"
#include "metric.hpp"

#include <cmath>
#include <gtest/gtest.h>

namespace analyzer::metric_accumulator::metric_accumulator_impl::tests {

class SumAverageAccumulatorTest : public ::testing::Test {
protected:
    SumAverageAccumulator accumulator;

    metric::MetricResult CreateMetricResult(const std::string &name, int value) {
        return metric::MetricResult{.metric_name = name, .value = value};
    }
};

TEST_F(SumAverageAccumulatorTest, ThrowsIfGetCalledBeforeFinalize) {
    accumulator.Accumulate(CreateMetricResult("code_lines", 15));

    EXPECT_THROW(
        {
            try {
                accumulator.Get();
            } catch (const std::runtime_error &e) {
                EXPECT_STREQ(e.what(), "CategoricalAccumulator::Get() called before Finalize()");
                throw;
            }
        },
        std::runtime_error);
}

TEST_F(SumAverageAccumulatorTest, CalculatesSumAndAverageCorrectly) {
    accumulator.Accumulate(CreateMetricResult("code_lines", 10));
    accumulator.Accumulate(CreateMetricResult("code_lines", 20));
    accumulator.Accumulate(CreateMetricResult("code_lines", 15));

    accumulator.Finalize();

    auto result = accumulator.Get();

    EXPECT_EQ(result.sum, 45);

    EXPECT_DOUBLE_EQ(result.average, 15.0);
}

TEST_F(SumAverageAccumulatorTest, HandlesEmptyAccumulation) {
    accumulator.Finalize();

    auto result = accumulator.Get();

    EXPECT_EQ(result.sum, 0);
    EXPECT_TRUE(std::isnan(result.average));
}

TEST_F(SumAverageAccumulatorTest, ResetsStateSuccessfully) {
    accumulator.Accumulate(CreateMetricResult("code_lines", 100));
    accumulator.Finalize();

    auto first_result = accumulator.Get();
    ASSERT_EQ(first_result.sum, 100);

    accumulator.Reset();

    EXPECT_THROW(accumulator.Get(), std::runtime_error);

    accumulator.Accumulate(CreateMetricResult("code_lines", 5));
    accumulator.Accumulate(CreateMetricResult("code_lines", 5));
    accumulator.Finalize();

    auto second_result = accumulator.Get();
    EXPECT_EQ(second_result.sum, 10);
    EXPECT_DOUBLE_EQ(second_result.average, 5.0);
}

TEST_F(SumAverageAccumulatorTest, DoubleFinalizeDoesNotCorruptData) {
    accumulator.Accumulate(CreateMetricResult("code_lines", 30));
    accumulator.Accumulate(CreateMetricResult("code_lines", 60));

    accumulator.Finalize();
    auto res1 = accumulator.Get();

    EXPECT_NO_THROW(accumulator.Finalize());
    auto res2 = accumulator.Get();

    EXPECT_EQ(res1.sum, res2.sum);
    EXPECT_DOUBLE_EQ(res1.average, res2.average);
}

}  // namespace analyzer::metric_accumulator::metric_accumulator_impl::tests
