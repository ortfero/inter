// This file is part of inter library
// Copyright 2023 Andrei Ilin <ortfero@gmail.com>
// SPDX-License-Identifier: MIT

# pragma once


#include <charconv>
#include <optional>
#include <string_view>
#include <unordered_map>

#include <inter/http_headers.hpp>


namespace inter {


    enum class http_method {
        GET, HEAD, POST, PUT, DELETE,
        CONNECT, OPTIONS, TRACE, PATCH    
    }; // http_method

    inline constexpr std::string_view entitle(http_method m) noexcept {
        switch(m) {
            case http_method::GET:
                return "GET";
            case http_method::HEAD:
                return "HEAD";
            case http_method::POST:
                return "POST";
            case http_method::PUT:
                return "PUT";
            case http_method::DELETE:
                return "DELETE";
            case http_method::CONNECT:
                return "CONNECT";
            case http_method::OPTIONS:
                return "OPTIONS";
            case http_method::TRACE:
                return "TRACE";
            case http_method::PATCH:
                return "PATCH";
        }
        return "UNKNOWN";
    }

    inline std::optional<http_method> parse_method(std::string_view text) {
        switch(text.size()) {
            case 3:
                if(text == "GET")
                    return http_method::GET;
                if(text == "PUT")
                    return http_method::PUT;
                return std::nullopt;
            case 4:
                if(text == "POST")
                    return http_method::POST;
                if(text == "HEAD")
                    return http_method::HEAD;
                return std::nullopt;
            case 5:
                if(text == "PATCH")
                    return http_method::PATCH;
                if(text == "TRACE")
                    return http_method::TRACE;
                return std::nullopt;
            case 6:
                if(text == "DELETE")
                    return http_method::DELETE;
                return std::nullopt;
            case 7:
                if(text == "OPTIONS")
                    return http_method::OPTIONS;
                if(text == "CONNECT")
                    return http_method::CONNECT;
                return std::nullopt;
            default:
                return std::nullopt;
        }
    }


    struct http_request {
        http_method method;
        std::string_view uri;
        int major_version;
        int minor_version;
        std::string_view headers[http_header::count];
        dynamic_headers dynamic_headers;
        std::string_view body;
    }; // http_request


    namespace detail {

        char const letters[256] = {
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
           0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
           1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
           0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
           1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        }; // letters
    
        char const digits[256] = {
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
           1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        }; // digits

        char const uri_characters[256] = {
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
           0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
           1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1,
           1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
           1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1,
           0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
           1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0,
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        }; // uri_characters

        char const header_characters[256] = {
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
           1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
           0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
           1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1,
           0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
           1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        }; // header_characters

    } // namespace detail

    inline std::optional<http_request> parse_request(char const* text) {
        if(text == nullptr)
            return std::nullopt;
        auto const* method_marker = text;
        while(detail::letters[*text])
            ++text;
        auto const method_view = std::string_view{method_marker, text - method_marker};
        auto const maybe_method = parse_method(method_view);
        if(!maybe_method)
            return std::nullopt;
        auto const method = *maybe_method;
        if(*text != ' ')
            return std::nullopt;
        auto const* uri_marker = ++text;
        while(detail::uri_characters[*text])
            ++text;
        auto const uri = std::string_view{uri_marker, text - uri_marker};
        if(*text != ' ')
            return std::nullopt;
        ++text;
        if(*text != 'H')
            return std::nullopt;
        ++text;
        if(*text != 'T')
            return std::nullopt;
        ++text;
        if(*text != 'T')
            return std::nullopt;
        ++text;
        if(*text != 'P')
            return std::nullopt;
        auto const* major_version_marker = ++text;
        while(detail::digits[*text])
            ++text;
        int major_version;
        auto const major_version_parsed = std::from_chars(major_version_marker,
                                                          text,
                                                          major_version);
        if(major_version_parsed.ec != std::errc{})
            return std::nullopt;
        if(*text != '.')
            return std::nullopt;
        auto const* minor_version_marker = ++text;
        while(detail::digits[*text])
            ++text;
        int minor_version;
        auto const minor_version_parsed = std::from_chars(minor_version_marker,
                                                          text,
                                                          minor_version);
        if(minor_version_parsed.ec != std::errc{})
            return std::nullopt;
        if(*text == '\r')
            ++text;
        if(*text != '\n')
            return std::nullopt;
        ++text;
        auto request = struct request{
            .method = method,
            .uri = uri,
            .major_version = major_version,
            .minor_version = minor_version
        };
        for(;;) {
            if(*text == '\r')
                ++text;
            if(*text == '\n')
                break;
            auto const* header_marker = text;
            while(detail::header_characters[*text])
                ++text;
            auto const header_view = std::string_view{header_marker,
                                                      text - header_marker};
            if(*text != ':')
                return std::nullopt;
            ++text;
            while(*text == ' ')
                ++text;
            auto const* value_marker = text;
            while(*text != '\0' && *text != '\n')
                ++text;
            auto const* eol = text;
            --text;
            if(*text == '\r')
                --text;
            while(*text == ' ')
                --text;
            ++text;
            auto const value = std::string_view{value_marker,
                                                text - value_marker};
            if(value.empty())
                return std::nullopt;
            auto const maybe_header = parse_header(header_view);
            if(maybe_header) {
                request.headers[*maybe_header] = value;
            } else {
                request.dynamic_headers[header_view] = value;
            }
        }
        return request;
    }   
    
} // namespace inter::http
