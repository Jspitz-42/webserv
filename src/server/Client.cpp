/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: altheven <altheven@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/07 12:58:15 by jspitz            #+#    #+#             */
/*   Updated: 2025/08/23 13:21:46 by altheven         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Socket.hpp"

Client :: Client(int fd, Socket & s): _fd(fd), _keep_alive(true), _time_to_die(timestamp_in_ms() + TIME_TO_DIE), _socket(s)
{
	return;
}

Client::Client(const Client & param): _fd(param._fd), _keep_alive(param._keep_alive), _time_to_die(param._time_to_die), _socket(param._socket)
{
	return ;
}

Client & Client :: operator=(const Client & other)
{
	if (this != &other)
	{
		this->_fd = other._fd;
		this->_keep_alive = other._keep_alive;
		this->_time_to_die = other._time_to_die;
		this->_socket = other._socket;
	}
	return(*this);
}

Client :: ~Client(void)
{
	return ;
}

int	Client :: getFd(void) const
{
	return (_fd);
}

Socket const & Client :: getSocket(void) const
{
	return (_socket);
}

void Client :: handleRequest(Config :: ServerConfig conf)
{
	char			buffer[8192] = {0};
	int				valread;
	std :: string	buf;
	UINT64_T		ms = timestamp_in_ms();
	size_t			maxSize = 30 * 1024 * 1024;
	size_t			expectedContentLength = 0;
	bool			hasContentLength = false;
	bool			headersComplete = false;
	while (buf.size() < maxSize) {
		valread = read(_fd, buffer, 8192);
		if (valread <= 0)
			break;
		
		std::string tmp(buffer, valread);
		buf += tmp;
		
		if (!headersComplete) {
			size_t headerEnd = buf.find("\r\n\r\n");
			if (headerEnd == std::string::npos) {
				headerEnd = buf.find("\n\n");
			}
			
			if (headerEnd != std::string::npos) {
				headersComplete = true;
				std::string headers = buf.substr(0, headerEnd);
				std::transform(headers.begin(), headers.end(), headers.begin(), ::tolower);
				
				size_t clPos = headers.find("content-length:");
				if (clPos != std::string::npos) {
					size_t lineEnd = headers.find('\n', clPos);
					if (lineEnd != std::string::npos) {
						std::string clLine = headers.substr(clPos + 15);
						size_t lineEndPos = clLine.find('\n');
						if (lineEndPos != std::string::npos) {
							clLine = clLine.substr(0, lineEndPos);
						}
						std::stringstream ss(clLine);
						ss >> expectedContentLength;
						hasContentLength = true;
					}
				}
			}
		}
		
		if (headersComplete && hasContentLength) {
			size_t headerSize = buf.find("\r\n\r\n");
			if (headerSize == std::string::npos) {
				headerSize = buf.find("\n\n") + 2;
			} else {
				headerSize += 4;
			}
			
			size_t totalExpected = headerSize + expectedContentLength;
			if (buf.size() >= totalExpected) {
				break;
			}
		}
		if (headersComplete && !hasContentLength && valread < 8192) {
			break;
		}
	}
	// std :: cout << buf << std :: endl;
	if (buf.length() > 0)
	{
		_time_to_die = ms + TIME_TO_DIE;
		Request req(buf, conf);
		Response res(req, conf);
		std :: string full_response(res.createResponse());
		write(_fd, full_response.c_str(), full_response.length());
		_keep_alive = res.getKeepAlive();
	} else 
	{
		_keep_alive = false;
	}	
}


bool Client::keepAlive() const
{
	return (_keep_alive);
}

UINT64_T	const & Client :: timeToDie() const
{
	return (_time_to_die);
}