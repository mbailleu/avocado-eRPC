#include "common.h"

extern "C" {
#include <hugetlbfs.h>
}
#include <algorithm>

namespace erpc {

size_t const kHugepageSize = [] {
    int const n = getpagesizes(nullptr, 0);
    exit_assert(n != -1, "Could not get number of page sizes");
    exit_assert(n > 0, "No number given");
    long pagesizes[n];
    memset(pagesizes, 0, sizeof(long) * n);
    int k = getpagesizes(pagesizes, n);
    exit_assert(k != -1, "Could not get page sizes");
    exit_assert(k == n, "WTF");
    fprintf(stdout, "Found page sizes:\n");
    auto max_page_size = pagesizes[0];
    for (auto i = 0ULL; i < n; ++i) {
        fprintf(stdout, "%llu: %ld B\n", i, pagesizes[n]);
        max_page_size = std::max(max_page_size, pagesizes[i]);
    }
    return max_page_size;
}();

} //namespace erpc
