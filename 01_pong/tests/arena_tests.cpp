#include "arena.h"

static constexpr void arenaT_test_ctor()
{
    Arena<800, 600> arena{};
}

static constexpr void arena_test_ctor()
{
    Arena arena{};
}

static constexpr bool arena_tests()
{
    arenaT_test_ctor();
    arena_test_ctor();
    return true;
}

static_assert(arena_tests());