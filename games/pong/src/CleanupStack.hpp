#pragma once

#include <functional>
#include <type_traits>
#include <utility>
#include <vector>

class CleanupStack {
public:
    // std::move_only_function exprime précisément la propriété unique des
    // actions. Le repli conserve la compatibilité avec les versions de libc++
    // qui ne fournissent pas encore cette partie de C++23.
#if defined(__cpp_lib_move_only_function) && __cpp_lib_move_only_function >= 202110L
    using Action = std::move_only_function<void() noexcept>;
#else
    using Action = std::function<void()>;
#endif

    CleanupStack()
    {
        // Effectue l'allocation principale avant l'acquisition des ressources C.
        m_actions.reserve(8);
    }

    CleanupStack(const CleanupStack &) = delete;
    CleanupStack &operator=(const CleanupStack &) = delete;
    CleanupStack(CleanupStack &&) = delete;
    CleanupStack &operator=(CleanupStack &&) = delete;

    ~CleanupStack() noexcept
    {
        for (auto action{m_actions.rbegin()}; action != m_actions.rend(); ++action) {
            (*action)();
        }
    }

    template <typename F>
        requires std::is_nothrow_invocable_v<std::decay_t<F> &>
    void defer(F &&action)
    {
        m_actions.emplace_back(std::forward<F>(action));
    }

private:
    std::vector<Action> m_actions{};
};
