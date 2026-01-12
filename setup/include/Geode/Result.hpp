#pragma once
#include <string>
#include <utility>

namespace geode {
    template <typename T>
    class Result {
    public:
        bool ok;
        T value;
        std::string error;

        Result(T v) : ok(true), value(std::move(v)) {}
        Result(std::string e) : ok(false), error(std::move(e)) {}

        bool isOk() const { return ok; }
        T unwrap() const { return value; }
    };

    template <typename T>
    Result<T> Ok(T v) { return Result<T>(v); }
    inline Result<void> Err(std::string e) { return Result<void>(e); }
}
