// This file is part of inter library
// Copyright 2023 Andrei Ilin <ortfero@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once


#include <netinet/in.h>

#include <memory>
#include <string>
#include <vector>


namespace inter {

    struct http_session {

        using size_type = std::size_t;

        int socket;
        sockaddr_in address;
        std::string rx_buffer;
        std::string tx_buffer;

        http_session(size_type rx_buffer_capacity,
                     size_type tx_buffer_capacity) {
            rx_buffer.reserve(rx_buffer_capacity);
            tx_buffer.reserve(tx_buffer_capacity);
        }

        http_session(http_session const&) = delete;
        http_session& operator = (http_session const&) = delete;
        http_session(http_session&&) = default;
        http_session& operator = (http_session&&) = default;

    }; // http_session

    using http_session_ptr = std::unique_ptr<http_session>;


    class http_sessions_pool {

        std::vector<http_session_ptr> sessions_;

    public:

        using size_type = std::size_t;

        http_sessions_pool() = default;
        http_sessions_pool(http_sessions_pool const&) = delete;
        http_sessions_pool& operator = (http_sessions_pool const&) = delete;
        http_sessions_pool(http_sessions_pool&&) = default;
        http_sessions_pool& operator = (http_sessions_pool&&) = default;


        http_session_ptr use(size_type rx_buffer_capacity,
                             size_type tx_buffer_capacity) {
            if(sessions_.empty())
                return std::make_unique<http_session>(rx_buffer_capacity,
                                                      tx_buffer_capaciy);
            auto ptr = std::move(sessions_.back());
            sessions_.pop_back();
            return ptr;
        }


        void recycle(http_session_ptr ptr) {
            sessions_.push_back(std::move(ptr));
        }

    }; // http_sessions_pool

} // namespace inter
