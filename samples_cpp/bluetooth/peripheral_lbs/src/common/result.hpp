/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#pragma once

#include <utility>

namespace nrf::common {

/**
 * @brief Result type for error handling
 * 
 * A simple Result type that encapsulates either a success value or an error code.
 * Follows the pattern of Rust's Result or C++ expected.
 * 
 * @tparam T The success value type (default: void for operations with no return value)
 */
template<typename T = void>
class Result {
public:
    /**
     * @brief Construct a success result with a value
     */
    explicit Result(T value) : value_(std::move(value)), error_code_(0) {}

    /**
     * @brief Construct an error result
     */
    static Result error(int error_code) {
        Result result;
        result.error_code_ = error_code;
        return result;
    }

    /**
     * @brief Construct a success result
     */
    static Result ok(T value) {
        return Result(std::move(value));
    }

    /**
     * @brief Check if the result is successful
     */
    [[nodiscard]] bool isOk() const { return error_code_ == 0; }

    /**
     * @brief Check if the result is an error
     */
    [[nodiscard]] bool isError() const { return error_code_ != 0; }

    /**
     * @brief Get the error code
     */
    [[nodiscard]] int errorCode() const { return error_code_; }

    /**
     * @brief Get the success value (undefined behavior if isError())
     */
    [[nodiscard]] const T& value() const & { return value_; }
    [[nodiscard]] T&& value() && { return std::move(value_); }

    /**
     * @brief Implicit conversion to bool (true if ok)
     */
    explicit operator bool() const { return isOk(); }

private:
    Result() = default;
    T value_{};
    int error_code_{0};
};

/**
 * @brief Specialization for void - operations that don't return a value
 */
template<>
class Result<void> {
public:
    /**
     * @brief Construct a success result
     */
    Result() : error_code_(0) {}

    /**
     * @brief Construct an error result
     */
    static Result error(int error_code) {
        Result result;
        result.error_code_ = error_code;
        return result;
    }

    /**
     * @brief Construct a success result
     */
    static Result ok() {
        return Result();
    }

    /**
     * @brief Check if the result is successful
     */
    [[nodiscard]] bool isOk() const { return error_code_ == 0; }

    /**
     * @brief Check if the result is an error
     */
    [[nodiscard]] bool isError() const { return error_code_ != 0; }

    /**
     * @brief Get the error code
     */
    [[nodiscard]] int errorCode() const { return error_code_; }

    /**
     * @brief Implicit conversion to bool (true if ok)
     */
    explicit operator bool() const { return isOk(); }

private:
    int error_code_{0};
};

} // namespace nrf::common
