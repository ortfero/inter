// This file is part of inter library
// Copyright 2023 Andrei Ilin <ortfero@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once


#include <cstdint>
#include <expected>

#include <inter/tcp_server.hpp>


namespace inter::http {


    class http_server : public tcp_server_observer {
        tcp_server tcp_server_;

    public:

        http_server() = default;
        http_server(http_server const&) = delete;
        http_server& operator = (http_server const&) = delete;
        http_server(http_server&&) = default;
        http_server& operator = (http_server&&) = default;

        void stop() { tcp_server_.stop(); }

        std::expected<void, std::error_code>
        listen(std::int16_t port,
               int connection_requests_limit = tcp_server::default_connection_requests_limit) {
            return tcp_server_.listen(port, *this, connection_requests_limit);
        }

    private:

        virtual void on_connected(int, sockaddr_in const&) override {

        }

        virtual void on_disconnected(int) override {

        }

        virtual tcp_response on_data_ready(int) override {
            return tcp_response::await_next_data;
        }
    }; // http_server

} // namespace inter::http

