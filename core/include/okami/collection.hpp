#pragma once

namespace okami {
    template <typename It>
    struct Collection {
        It mBegin;
        It mEnd;

        using item_t = std::remove_reference_t<decltype(*mBegin)>;
        using iterator_t = It;

        It begin() {
            return mBegin;
        }
        It end() { 
            return mEnd;
        }

        bool IsEmpty() const {
            return mBegin == mEnd;
        }

        size_t Count() const {
            size_t count = 0;
            for (It it = mBegin; it != mEnd; ++it, ++count);
            return count;
        }

        auto ToVector() {
            std::vector<item_t> vec;
            for (auto it = mBegin; it != mEnd; ++it) {
                vec.emplace_back(*it);
            }
            return vec;
        }

        std::optional<item_t> First() {
            if (mBegin != mEnd) {
                return *mBegin;
            } else {
                std::optional<item_t>{};
            }
        }

        std::optional<item_t> Last() {
            if (mBegin == mEnd)
                return std::optional<item_t>{};
            auto it = mBegin;
            while (true) {
                auto next = it;
                ++next;
                if (next == mEnd) {
                    return *it;
                }
                it = next;
            }
        }

        Collection(It begin, It end) : mBegin(begin), mEnd(end) {}
    };
}