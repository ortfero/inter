// This file is part of inter library
// Copyright 2023 Andrei Ilin
// SPDX-License-Identifier: MIT

#pragma once


#include <errno.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <poll.h>
#include <unistd.h>

#include <cstdint>
#include <expected>
#include <functional>
#include <span>
#include <system_error>
#include <vector>


namespace inter {


    namespace detail {

        inline std::unexpected<std::error_code> make_unexpected_from_errno() noexcept {
            return std::unexpected(std::error_code{errno, std::system_category()});
        }

    } // namespace detail


    class tcp_rx_buffer {
        std::vector<char> buffer_;
        std::size_t received_{0u};

    public:

        tcp_rx_buffer() = default;
        tcp_rx_buffer(tcp_rx_buffer const&) = delete;
        tcp_rx_buffer& operator = (tcp_rx_buffer const&) = delete;
        tcp_rx_buffer(tcp_rx_buffer&&) = default;
        tcp_rx_buffer& operator = (tcp_rx_buffer&&) = default;
        void clear() { buffer_.clear(); received_ = 0u; }

    }; // tcp_rx_buffer




    enum class tcp_response {
        close_connection = 1, await_next_data
    }; // tcp_response


    class tcp_server_observer {
    public:
        virtual void on_connected(int client_socket, sockaddr_in const&) = 0;
        virtual void on_disconnected(int client_socket) = 0;
        virtual void on_data_ready(int client_socket) = 0;
    }; // tcp_server_observer


    class tcp_server {
        using descriptors = std::vector<pollfd>;

        bool stopping_{false};
        tcp_server_observer* observer_{nullptr};

    public:

        static constexpr auto default_buffer_size = 4096;
        static constexpr auto default_connection_requests_limit = 64;

        tcp_server() = default;
        tcp_server(tcp_server const&) = delete;
        tcp_server& operator = (tcp_server const&) = delete;
        tcp_server(tcp_server&&) = default;
        tcp_server& operator = (tcp_server&&) = default;
        ~tcp_server() { stop(); }

        
        void stop() noexcept {
            stopping_ = true;
        }

        
        void observe(tcp_server_observer* observer) noexcept {
            observer_ = observer;
        }


        void ignore() noexcept {
            observer_ = nullptr;
        }
        

        std::expected<void, std::error_code>
        listen(std::int16_t port,
               tcp_server_observer& observer,
               int connection_requests_limit = default_connection_requests_limit) {
            auto const server_socket = ::socket(AF_INET, SOCK_STREAM, 0);
            if(server_socket == -1)
                return detail::make_unexpected_from_errno();
            int const reuse = 1;
            if(setsockopt(server_socket, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse)) < 0)
                return detail::make_unexpected_from_errno();
            auto addr = sockaddr_in{};
            addr.sin_family = AF_INET;
            addr.sin_port = htons(port);
            addr.sin_addr.s_addr = htonl(INADDR_ANY);
            auto const binded = ::bind(server_socket, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
            if(binded == -1)
                return detail::make_unexpected_from_errno();
            auto const listened = ::listen(server_socket, connection_requests_limit);
            if(listened == -1)
                return detail::make_unexpected_from_errno();
            poll_ds = descriptors{};
            poll_ds.push_back(pollfd {
                .fd = server_socket,
                .events = POLLIN
            });
            auto rx_buffer = std::vector<char>{buffer_size};
            while(!stopping_) {
                auto const polled_count = poll(poll_ds_.data(), poll_ds_.size(), 1000);
                if(polled_count <= 0)
                    continue;
                auto handled_count = 0;
                if(poll_ds[0].revents & POLLIN) {
                    ++handled_count;
                    auto client_addr = sockaddr_in{};
                    auto const client_socket = ::accept(server_socket,
                                                        reinterpret_cast<sokaddr*>(&client_addr),
                                                        sizeof(client_addr));
                    auto const accepted = observer.on_connected(client_socket, client_addr);
                    if(accepted)
                        poll_ds.push_back(pollfd {
                            .fd = client_socket,
                            .events = POLLIN
                        });
                    else
                        ::close(client_socket);
                }
                auto it = poll_ds_.begin() + 1;
                while(handled_count != polled_count && it != poll_ds_.end()) {
                    handle_socket(handled_count, rx_buffer, observer, poll_ds, it);
                }
            }
            stopping_ = false;
            for(auto& poll_dr: poll_ds) {
                ::close(poll_dr.fd);
                observer.on_disconnected(poll_dr.fd);
            }
        }


    private:

        static void
        handle_socket(int& handled_count,
                      tcp_server_observer& observer,
                      descriptors& poll_ds,
                      descriptors::iterator& it) {
            if(!(it->revents & POLLIN))
                return void(++it);
            ++handled_count;
            auto const tcp_response = observer.on_data_ready(it->fd);
            switch(tcp_response) {
                case tcp_response::close_connection:
                    return close_connection(observer, poll_ds, it);
                case tcp_response::await_next_data:
                    return void(++it);
            }
        }


        static void
        send_response(std::span<char> tx_buffer,
                      tcp_server_observer& observer,
                      descriptors& poll_ds,
                      descriptors::iterator& it) {
            auto* tx_data = tx_buffer.data();
            auto tx_size= tx_buffer.size();
            for(;;) {
                tx_result = ::write(it->fd, tx_data, tx_size);
                if(tx_result == -1)
                    return close_connection(observer, poll_ds, it);
                tx_size -= tx_result;
                if(tx_size == 0)
                    break;
                tx_data += tx_result;
            }
            ++it;
        }


        static void
        close_connection(tcp_server_observer& observer,
                         descriptors& poll_ds,
                         descriptors::iterator& it) {
            ::close(it->fd);
            observer.on_disconnected(it->fd);
            it = poll_ds.erase(it);
        }

    }; // tcp_server

} // namespace inter
