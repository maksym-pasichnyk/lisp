#pragma once
#include <vector>

template <typename T>
struct stream  {
protected:
    std::vector<T> data;
    size_t index;

public:
    stream(const std::vector<T>& data) : data(std::move(data)), index(0) {}

    template<typename It>
    stream(const It& _begin, const It& _end) : data(_begin, _end), index(0) {}

    inline stream<T> operator++() {
        stream<T> prev = *this;
        index < data.size() ? index++ : 0;
        return prev;
    }

    inline stream<T> operator++(int) {
        index < data.size() ? index++ : 0;
        return *this;
    }

    T peek(const int& offset = 0) const {
        return index + offset < data.size() ? data[index + offset] : T{};
    }

    inline T& operator*() {
        return data[index];
    }

    operator bool() const {
        return index < data.size();
    }

    bool empty() {
        return data.empty();
    }

    virtual size_t position() const {
        return index;
    }

    void reset() {
        index = 0;
    }
};