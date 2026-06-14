#include "metric_accumulator_impl/categorical_accumulator.hpp"
#include "metric.hpp"

#include <gtest/gtest.h>

namespace analyzer::metric_accumulator::metric_accumulator_impl::tests {

class CategoricalAccumulatorTest : public ::testing::Test {
protected:
    CategoricalAccumulator accumulator;

    metric::MetricResult CreateMetricResult(const std::string &name, const std::string &value) {
        return metric::MetricResult{.metric_name = name, .value = value};
    }
};

TEST_F(CategoricalAccumulatorTest, ThrowsIfGetCalledBeforeFinalize) {
    accumulator.Accumulate(CreateMetricResult("naming_style", "Snake Case"));

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

TEST_F(CategoricalAccumulatorTest, AccumulatesFrequenciesCorrectly) {
    accumulator.Accumulate(CreateMetricResult("naming_style", "Snake Case"));
    accumulator.Accumulate(CreateMetricResult("naming_style", "Camel Case"));
    accumulator.Accumulate(CreateMetricResult("naming_style", "Snake Case"));
    accumulator.Accumulate(CreateMetricResult("naming_style", "Pascal Case"));
    accumulator.Accumulate(CreateMetricResult("naming_style", "Snake Case"));
    accumulator.Accumulate(CreateMetricResult("naming_style", "Camel Case"));

    accumulator.Finalize();

    const auto &freqs = accumulator.Get();

    ASSERT_EQ(freqs.size(), 3);
    EXPECT_EQ(freqs.at("Snake Case"), 3);
    EXPECT_EQ(freqs.at("Camel Case"), 2);
    EXPECT_EQ(freqs.at("Pascal Case"), 1);
}

TEST_F(CategoricalAccumulatorTest, HandlesEmptyAccumulation) {
    accumulator.Finalize();

    const auto &freqs = accumulator.Get();
    EXPECT_TRUE(freqs.empty());
}

TEST_F(CategoricalAccumulatorTest, ResetsStateSuccessfully) {
    accumulator.Accumulate(CreateMetricResult("naming_style", "Snake Case"));
    accumulator.Finalize();

    ASSERT_FALSE(accumulator.Get().empty());

    accumulator.Reset();

    EXPECT_THROW(accumulator.Get(), std::runtime_error);

    accumulator.Finalize();
    EXPECT_TRUE(accumulator.Get().empty());
}

TEST_F(CategoricalAccumulatorTest, DoubleFinalizeDoesNotCorruptData) {
    accumulator.Accumulate(CreateMetricResult("naming_style", "Lower Case"));
    accumulator.Finalize();

    EXPECT_NO_THROW(accumulator.Finalize());

    const auto &freqs = accumulator.Get();
    EXPECT_EQ(freqs.at("Lower Case"), 1);
}

}  // namespace analyzer::metric_accumulator::metric_accumulator_impl::tests
