#pragma once

#include <cstddef>
#include <functional>
#include <type_traits>
#include <utility>
#include <vector>

class CleanupStack {
public:
    class Transaction {
    public:
        Transaction(const Transaction &) = delete;
        Transaction &operator=(const Transaction &) = delete;
        Transaction(Transaction &&) = delete;
        Transaction &operator=(Transaction &&) = delete;

        ~Transaction() noexcept
        {
            if (!m_committed) {
                m_owner.rollbackTo(m_checkpoint);
            }
        }

        template<typename F>
            requires std::is_nothrow_invocable_v<std::decay_t<F> &>
        void defer(F &&action)
        {
            m_owner.defer(std::forward<F>(action));
        }

        void commit() noexcept
        {
            m_committed = true;
        }

    private:
        friend class CleanupStack;

        explicit Transaction(CleanupStack &owner) noexcept
            : m_owner{owner},
              m_checkpoint{owner.m_actions.size()}
        {
        }

        CleanupStack &m_owner;
        std::size_t m_checkpoint{};
        bool m_committed{};
    };

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
        rollbackTo(0);
    }

    [[nodiscard]] Transaction transaction() noexcept
    {
        return Transaction{*this};
    }

    template<typename F>
        requires std::is_nothrow_invocable_v<std::decay_t<F> &>
    void defer(F &&action)
    {
        m_actions.emplace_back(std::forward<F>(action));
    }

private:
    void rollbackTo(std::size_t checkpoint) noexcept
    {
        while (m_actions.size() > checkpoint) {
            m_actions.back()();
            m_actions.pop_back();
        }
    }

    std::vector<Action> m_actions{};
};
