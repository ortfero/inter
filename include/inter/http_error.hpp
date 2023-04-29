
// This file is part of inter library
// Copyright 2023 Andrei Ilin <ortfero@gmail.com>
// SPDX-License-Identifier: MIT

# pragma once


#include <system_error>


namespace inter {


    enum class http_error {
        bad_request = 400,
        unauthorized,
        forbidden,
        not_found,
        method_not_allowed
    }; // http_error


    class http_error_category : public std::error_category {
    public:

        virtual char const* name() const noexcept {
            return "http";
        }


        virtual string message(int ec) const {
            switch(error(ec)) {
                case error::bad_request:
                    return {"Bad request"};
                case error::unauthorized:
                    return {"Unauthorized"};
                case error::forbidden:
                    return {"Forbidden"};
                case error::not_found:
                    return {"Not found"};
                case error::method_not_allowed:
                    return {"Method not allowed"};
            }
            return {"Unknown"};
        }
    }; // error_category


    inline http_error_category const http_error_category;

    inline std::error_code make_error_code(error e) noexcept {
        return std::error_code{int(e), http_error_category};
    }


} // namespace inter


namespace std {

    template<> struct is_error_code_enum<inter::http_error>: true_type {};

} // std
