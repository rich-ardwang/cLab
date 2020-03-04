//
// async_client.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <thread>
#include <chrono>
#include <memory>
#include <asio.hpp>

using asio::ip::tcp;

class client {
public:
    client(const std::string& server, const std::string& path) : 
        io_context_(std::make_shared<asio::io_context>()),
        resolver_(*io_context_), socket_(*io_context_), work_thread_(nullptr) {
    // Form the request. We specify the "Connection: close" header so that the
    // server will close the socket after transmitting the response. This will
    // allow us to treat all data up until the EOF as the content.
    std::ostream request_stream(&request_);
    request_stream << "POST " << path << " HTTP/1.0\r\n";
    request_stream << "Host: " << server << "\r\n";
    request_stream << "Content-Type: application/json; charset=utf-8 \r\n";
    std::string json_data = "{\"http_server_name\":\"loginsrv.http_server\",\"http_auth_pwd\":\"123.com\",\"user_name\":\"id00011\",\"token\":\"336df8681794f308f244da4532ad07efe73ee39f3f4fd5c42177288914da70e0\"}";
    request_stream << "Content-Length: " << json_data.length() << "\r\n";
    request_stream << "Accept: */*\r\n";
    request_stream << "Connection: close\r\n\r\n";
    request_stream << json_data << "\r\n";

    // Start an asynchronous resolve to translate the server and service names
    // into a list of endpoints.
    asio::ip::tcp::resolver::query query(server, "8080");
    /*resolver_.async_resolve(server, "http",
        boost::bind(&client::handle_resolve, this,
          asio::placeholders::error,
          asio::placeholders::results));*/
    //resolver_.async_resolve(query, std::bind(&client::handle_resolve, this,
    //    asio::placeholders::error, asio::placeholders::iterator));
    resolver_.async_resolve(query, std::bind(&client::handle_resolve, this, 
        std::placeholders::_1, std::placeholders::_2));
    }

    ~client() {
        if (io_context_) {
            io_context_->stop();
            work_thread_->join();
        }
        std::cout << "release" << std::endl;
    }

    void start() {
        if (nullptr == work_thread_)
            work_thread_ = std::make_shared<std::thread>(std::bind(&client::run, this));
    }

private:
  void handle_resolve(const asio::error_code& err,
      const tcp::resolver::results_type& endpoints)
  {
    if (!err)
    {
      // Attempt a connection to each endpoint in the list until we
      // successfully establish a connection.
      asio::async_connect(socket_, endpoints,
          std::bind(&client::handle_connect, this,
          std::placeholders::_1));
    }
    else
    {
      std::cout << "Error: " << err.message() << "\n";
    }
  }

  void handle_connect(const asio::error_code& err)
  {
    if (!err)
    {
      // The connection was successful. Send the request.
      asio::async_write(socket_, request_,
          std::bind(&client::handle_write_request, this,
            std::placeholders::_1));
    }
    else
    {
      std::cout << "Error: " << err.message() << "\n";
    }
  }

  void handle_write_request(const asio::error_code& err)
  {
    if (!err)
    {
      // Read the response status line. The response_ streambuf will
      // automatically grow to accommodate the entire line. The growth may be
      // limited by passing a maximum size to the streambuf constructor.
      asio::async_read_until(socket_, response_, "\r\n",
          std::bind(&client::handle_read_status_line, this,
            std::placeholders::_1));
    }
    else
    {
      std::cout << "Error: " << err.message() << "\n";
    }
  }

  void handle_read_status_line(const asio::error_code& err)
  {
    if (!err)
    {
      //std::this_thread::sleep_for(std::chrono::seconds(10));
      // Check that response is OK.
      std::istream response_stream(&response_);
      std::string http_version;
      response_stream >> http_version;
      unsigned int status_code;
      response_stream >> status_code;
      std::string status_message;
      std::getline(response_stream, status_message);
      if (!response_stream || http_version.substr(0, 5) != "HTTP/")
      {
        std::cout << "Invalid response\n";
        return;
      }
      if (status_code != 200)
      {
        std::cout << "Response returned with status code ";
        std::cout << status_code << "\n";
        return;
      }

      // Read the response headers, which are terminated by a blank line.
      asio::async_read_until(socket_, response_, "\r\n\r\n",
          std::bind(&client::handle_read_headers, this,
            std::placeholders::_1));
    }
    else
    {
      std::cout << "Error: " << err << "\n";
    }
  }

  void handle_read_headers(const asio::error_code& err)
  {
    if (!err)
    {
      // Process the response headers.
      std::istream response_stream(&response_);
      std::string header;
      while (std::getline(response_stream, header) && header != "\r")
        std::cout << header << "\n";
      std::cout << "\n";

      // Write whatever content we already have to output.
      if (response_.size() > 0)
        std::cout << &response_;

      // Start reading remaining data until EOF.
      /*
      asio::async_read(socket_, response_,
          asio::transfer_at_least(1),
          std::bind(&client::handle_read_content, this,
            std::placeholders::_1));
            */
    }
    else
    {
      std::cout << "Error: " << err << "\n";
    }
  }

  void handle_read_content(const asio::error_code& err)
  {
    if (!err)
    {
      // Write all of the data that has been read so far.
      std::cout << &response_;

      // Continue reading remaining data until EOF.
      asio::async_read(socket_, response_,
          asio::transfer_at_least(1),
          std::bind(&client::handle_read_content, this,
            std::placeholders::_1));
    }
    else if (err != asio::error::eof)
    {
      std::cout << "Error: " << err << "\n";
    }
  }

  void run() {
      if (io_context_)
          io_context_->run();
  }

private:
  std::shared_ptr<asio::io_context> io_context_;
  tcp::resolver resolver_;
  tcp::socket socket_;
  asio::streambuf request_;
  asio::streambuf response_;
  std::shared_ptr<std::thread> work_thread_;
};

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
      std::cout << "Usage: async_client <server> <path>\n";
      std::cout << "Example:\n";
      std::cout << "  async_client 172.16.73.169 /loginservice/login-auth\n";
      return 1;
    }

  client c(argv[1], argv[2]);
  try
  {
    c.start();
  }
  catch (std::exception& e)
  {
    std::cout << "Exception: " << e.what() << "\n";
  }

  std::cout << "enter to exit." << std::endl;
  std::cin.get();
  return 0;
}
