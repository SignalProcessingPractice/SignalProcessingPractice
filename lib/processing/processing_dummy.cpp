#include <stdint.h>

#include "processing_dummy.hpp"

uint32_t DummyProcessing::dummy_calc(uint32_t a, uint32_t b) const {
    return a + b;
}
