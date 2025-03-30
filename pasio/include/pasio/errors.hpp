#ifndef PASIO_ERRORS_HPP
#define PASIO_ERRORS_HPP

#include <pasio/export/export.hpp>
#include <system_error>

namespace pasio {
    /**
     * @brief error to throw on os error
     * @details e.g. os error while failed to create serial_port
     */
    struct PASIO_EXPORT os_error final : std::system_error {
        using std::system_error::system_error;

        os_error(const os_error&) noexcept = default;
        os_error(os_error&&) noexcept = default;
        auto operator=(const os_error&) noexcept -> os_error& = default;
        auto operator=(os_error&&) noexcept -> os_error& = default;

        ~os_error() noexcept override = default;
    };
} // namespace pasio

#endif // PASIO_ERRORS_HPP