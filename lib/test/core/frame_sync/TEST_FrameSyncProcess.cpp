#include <gtest/gtest.h>

#include <iomanip>
#include <iostream>

#include "FrameSyncProcess.hpp"
#include "FrameSyncProcessConfig.hpp"
#include "Strategies/HannWindow.hpp"
#include "Strategies/RectangleOverlapAdder.hpp"
#include "Strategies/SineGenerator.hpp"

TEST(FrameSync, InputEqualOutput) {
    static constexpr std::size_t kDisplaySamples = 8U;
    static constexpr int kPrecision = 6;
    static constexpr float kTolerance = 1e-5F;

    SineGenerator sine_gen_inputer(SineGenerator::Params{SineGenerator::kDefaultFrequency,
                                                         SineGenerator::kDefaultAmplitude});
    SineGenerator sine_gen_validator(SineGenerator::Params{SineGenerator::kDefaultFrequency,
                                                           SineGenerator::kDefaultAmplitude});
    std::size_t frame_counter = 0U;

    class OutputValidator {
    public:
        OutputValidator(SineGenerator* validator, std::size_t* counter)
            : validator_(validator), counter_(counter) {}

        auto Exec(const FrameSyncProcess::AudioHop& hop) -> void {
            if (first_call_) {
                first_call_ = false;
                return;
            }

            auto validator_hop = validator_->Exec();
            EXPECT_EQ(hop.size(), validator_hop.size());
            for (std::size_t idx = 0; idx < hop.size(); ++idx) {
                EXPECT_NEAR(hop[idx], validator_hop[idx], kTolerance);
            }

            std::cout << "\n[Frame " << *counter_ << "] First " << kDisplaySamples
                      << " samples:\n";
            for (std::size_t idx = 0;
                 idx < std::min(kDisplaySamples, hop.size()) && idx < validator_hop.size();
                 ++idx) {
                std::cout << "  [" << idx << "] output=" << std::fixed
                          << std::setprecision(kPrecision) << hop[idx]
                          << ", validator=" << std::fixed << std::setprecision(kPrecision)
                          << validator_hop[idx] << "\n";
            }

            ++(*counter_);
        }

        auto Reset() -> void { first_call_ = true; }

    private:
        SineGenerator* validator_;
        std::size_t* counter_;
        bool first_call_{true};
    };

    HannWindow hann_window;
    RectangleOverlapAdder rectangle_overlap_adder;
    OutputValidator output_validator{&sine_gen_validator, &frame_counter};

    FrameSyncProcessConfig config;
    config.audio_aquire_strategy.bind(&sine_gen_inputer);
    config.window_strategy.bind(&hann_window);
    config.overlap_add_strategy.bind(&rectangle_overlap_adder);
    config.audio_output_strategy.bind(&output_validator);

    FrameSyncProcess proc{config};

    std::cout << "\n========== Frame Processing Started ==========\n";

    for (int idx = 0; idx < 2; ++idx) {
        proc.ProcessFrame();
    }
    std::cout << "\n";
    std::cout << "========== Frame Processing Complete ==========\n";
    std::cout << "Total frames processed: " << frame_counter << "\n\n";
}
