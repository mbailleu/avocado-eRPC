#include "common.h"

#include <string>

namespace erpc {

size_t const kHugepageSize
{[] () -> size_t {
    if (auto huge_size = getenv("Hugepagesize"); huge_size != nullptr) {
        return std::stoi(huge_size);
    }
    return GB(1);
}()};

} //namespace erpc
