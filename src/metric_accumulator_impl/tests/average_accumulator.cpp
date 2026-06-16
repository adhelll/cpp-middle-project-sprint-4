#include "metric_accumulator_impl/average_accumulator.hpp"
#include "metric.hpp"

#include <cmath>
#include <gtest/gtest.h>

namespace analyzer::metric_accumulator::metric_accumulator_impl::tests {

class AverageAccumulatorTest : public ::testing::Test {
protected:
    AverageAccumulator accumulator;

    metric::MetricResult CreateMetricResult(const std::string &name, int value) {
        return metric::MetricResult{.metric_name = name, .value = value};
    }
};

TEST_F(AverageAccumulatorTest, ThrowsIfGetCalledBeforeFinalize) {
    accumulator.Accumulate(CreateMetricResult("cyclomatic_complexity", 4));

    EXPECT_THROW(
        {
            try {
                accumulator.Get();
            } catch (const std::runtime_error &e) {
                EXPECT_STREQ(e.what(), "AverageAccumulator::Get() called before Finalize()");
                throw;
            }
        },
        std::runtime_error);
}

TEST_F(AverageAccumulatorTest, CalculatesAverageCorrectly) {
    accumulator.Accumulate(CreateMetricResult("cyclomatic_complexity", 3));
    accumulator.Accumulate(CreateMetricResult("cyclomatic_complexity", 4));
    accumulator.Accumulate(CreateMetricResult("cyclomatic_complexity", 5));
    accumulator.Accumulate(CreateMetricResult("cyclomatic_complexity", 5));  // Сумма = 17, Кол-во = 4

    accumulator.Finalize();

    EXPECT_DOUBLE_EQ(accumulator.Get(), 4.25);
}

TEST_F(AverageAccumulatorTest, HandlesEmptyAccumulation) {
    accumulator.Finalize();

    EXPECT_TRUE(std::isnan(accumulator.Get()));
}

TEST_F(AverageAccumulatorTest, ResetsStateSuccessfully) {
    accumulator.Accumulate(CreateMetricResult("cyclomatic_complexity", 10));
    accumulator.Finalize();
    ASSERT_DOUBLE_EQ(accumulator.Get(), 10.0);

    accumulator.Reset();

    EXPECT_THROW(accumulator.Get(), std::runtime_error);

    accumulator.Accumulate(CreateMetricResult("cyclomatic_complexity", 2));
    accumulator.Accumulate(CreateMetricResult("cyclomatic_complexity", 4));  // Среднее = 3.0
    accumulator.Finalize();

    EXPECT_DOUBLE_EQ(accumulator.Get(), 3.0);
}

TEST_F(AverageAccumulatorTest, DoubleFinalizeDoesNotChangeResult) {
    accumulator.Accumulate(CreateMetricResult("cyclomatic_complexity", 6));
    accumulator.Accumulate(CreateMetricResult("cyclomatic_complexity", 8));

    accumulator.Finalize();
    double first_get = accumulator.Get();

    EXPECT_NO_THROW(accumulator.Finalize());
    EXPECT_DOUBLE_EQ(accumulator.Get(), first_get);
}

}  // namespace analyzer::metric_accumulator::metric_accumulator_impl::tests
