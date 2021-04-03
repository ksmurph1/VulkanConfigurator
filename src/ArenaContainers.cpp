#include "ArenaContainers.hpp"

folly::SysArena aa::ARENA(ARENA_SIZE);
aa::CxxAllocatorAdaptorArena<char> aa::ArenaPoolAlloc::arenaAlloc;
