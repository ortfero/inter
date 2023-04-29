
// This file is part of inter library
// Copyright 2023 Andrei Ilin <ortfero@gmail.com>
// SPDX-License-Identifier: MIT

# pragma once


#include <optional>
#include <string_view>
#include <unordered_map>


namespace inter {

    struct http_header {
        enum code {
            connection, content_length, content_type, host, user_agent,
            count
        };
    }; // http_header

    inline std::string_view entitle(http_header::code h) noexcept {
        switch(h) {
            case http_header::connection:
                return {"Connection"};
            case http_header::content_length:
                return {"Content-Length"};
            case http_header::content_type:
                return {"Content-Type"};
            case http_header::host:
                return {"Host"};
            case http_header::user_agent:
                return {"User-Agent"};
            case http_header::count:
                return {"Unknown"};
        }
        return {"Unknown"};
    }

    inline std::optional<http_header::code> parse_header(std::string_view text) noexcept {
        switch(text.size()) {
            case 4:
                if(text == "Host")
                    return {http_header::host};
                return std::nullopt;
            case 10:
                if(text == "User-Agent")
                    return {http_header::user_agent};
                if(text == "Connection")
                    return {http_header::connection};
                return std::nullopt;
            case 12:
                if(text == "Content-Type")
                    return {http_header::content_type};
                return std::nullopt;
            case 14:
                if(text == "Content-Length")
                    return {http_header::content_length};
                return std::nullopt;
            default:
                return std::nullopt;
        }
    }
    

    using dynamic_headers = std::unordered_map<std::string_view, std::string_view>;
    
} // namespace inter
