/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tcpServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jspitz <jspitz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 12:41:50 by jspitz            #+#    #+#             */
/*   Updated: 2025/07/30 14:16:18 by jspitz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "tcpServer.hpp"
#include "main.hpp"

const int BUFFER_SIZE = 30720;

void log(const std::string & message)
{
	std::cout << message << std::endl;
}

void exitWithError(const std::string & errorMessage)
{
	log("ERROR: " + errorMessage);
	exit(1);
}

int TcpServer::startServer()
{
	m_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket < 0)
	{
		std::cerr << "line 34 errror called" << std::endl;
		exitWithError("Cannot create socket");
		return 1;
	}

	//int opt = 1;
	//if (setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt) < 0))
	//{
	//	std::cerr << "line 42 error called" << std::endl;
	//	exitWithError("cannot create socket");
	//	return 1;
	//}
	std::cerr <<  " socket = " << m_socket << std::endl;
	if (bind(m_socket, (sockaddr*)&m_socketAddress, m_socketAddress_len) < 0)
	{
		std::cout << "yes it is bind error " << std::endl;
		exitWithError("Cannot connect socket to address");
		return 1;
	}

	return 0;
}

TcpServer::TcpServer(std::string ip_address, int port) : m_ip_addr(ip_address), m_port(port), m_socket(), m_new_socket(), m_socketAddress(), m_socketAddress_len(sizeof(m_socketAddress)), m_serverMessage(buildResponse())
{
	std::memset(&m_socketAddress, 0, sizeof(m_socketAddress));
	std::memset(&csin, 0, sizeof(csin));

	m_socketAddress.sin_family = AF_INET;
	m_socketAddress.sin_port = htons(m_port);
	m_socketAddress.sin_addr.s_addr = inet_addr(m_ip_addr.c_str());

	if (startServer() != 0)
	{
		std::ostringstream ss;

		ss << "Failed to start server with PORT:" << ntohs(m_socketAddress.sin_port);
		log(ss.str());
	}

}

TcpServer::~TcpServer()
{
	closeServer();
}

void TcpServer::closeServer( void )
{
	close(m_socket);
	close(m_new_socket);
}

#include <fcntl.h>

void setnonblocking(int fd)
{
	int flags = fcntl(fd, F_GETFL, 0);
  	fcntl(fd, F_SETFL, flags | O_NONBLOCK); //error checking to be done here
}

void TcpServer::startListen()
{
	if (listen(m_socket, 20) < 0)
	{
		exitWithError("Socket listen failed");
	}

	std::ostringstream ss;
	ss << std::endl <<  "*** Listening on ADDRESS: " << inet_ntoa(m_socketAddress.sin_addr) << " PORT: "  << ntohs(m_socketAddress.sin_port) << " ***" << std::endl << std::endl;
	log(ss.str());

	_efd = epoll_create1(0);

	if (_efd < 0) {
		perror("epoll_create1");
		return ;
	}

	_ev.events = EPOLLIN;
	_ev.data.fd = m_socket;
	_ret = epoll_ctl(_efd, EPOLL_CTL_ADD, m_socket, &_ev);

	if (_ret < 0) {
		perror("eppoll_ctl");
		return ;
	}

	int i, buflen;
	char	buf[BUFFER_SIZE];

	while (1)
	{
		_nfds = epoll_wait(_efd, _evlist, MAX, -1);
		
		if (_ret == -1) {
			perror("epoll_wait");
			break ;
		}

		for (i = 0 ; i <= _nfds ; i++) {

				if (_evlist[i].data.fd == m_socket) {
				
				
					_cfd = accept(m_socket, (struct sockaddr *)&csin, &m_socketAddress_len);
				
					if (_cfd == -1) {
						perror("accept");
						return ;
					}
				
					std::cout << "\x1b[0;32m[*] accept\x1b[0m" << std::endl;
					
					setnonblocking(_cfd);
					_ev.events = EPOLLIN;
					_ev.data.fd = _cfd;
					_ret = epoll_ctl(_efd, EPOLL_CTL_ADD, _cfd, &_ev);
				
					if (_ret == -1) {
						perror("epoll_ctl");
						return ;
					}
					
					
					// add the accepted client to a list of connected clients

				} else  { // check if its a connected client
					_cfd = _evlist[i].data.fd;
					
					std::cout << "client trying to send something" << std::endl;

					buflen = recv(_cfd, buf, BUFSIZ-1, MSG_DONTWAIT);
					// buflen = read(_cfd, buf, BUFSIZ-1);
					
					std::cout << "Client sent: " << buflen << " bytes of data" << std::endl;
					std::cout << buf << std::endl;
					// remember to check for error if buflen == -1
					if (buflen ==  0) {
						if (close(_cfd) == -1) {
							perror("close");
							return ;
						}
					
						std::cout << "\x1b[0;31m[*] close \x1b[0m" << std::endl;
					
						epoll_ctl(_efd, EPOLL_CTL_DEL, _cfd, &_evlist[i]);
					
						if (_ret == -1) {
							perror("epoll_ctl");
							return ;
						}
						// remove the client from the list
		
					
					} else {
						buf[buflen] = '\0';
						std::string msgPrefix = "prevent-arbitrary-connection";
						std::string msg = buf;


						if (msgPrefix.length() > msg.length()) continue ;

						if (msgPrefix == msg.substr(0, msgPrefix.length()).c_str()) {
							msg = msg.substr(msgPrefix.length(), msg.length());
							std::cout << msg << std::endl;
						}
						send(_cfd, buildResponse().c_str(), buildResponse().size(), 0);
					}
				}
		}
	}
}

void TcpServer::AcceptConnection(int & new_socket)
{
	new_socket = accept(m_socket, (sockaddr *)&m_socketAddress, &m_socketAddress_len);
	if (new_socket < 0)
	{
		std::ostringstream ss;
		ss << "Server failed to accept incoming connection from ADDRESS: " << inet_ntoa(m_socketAddress.sin_addr) << "; PORT: " << ntohs(m_socketAddress.sin_port);
		exitWithError(ss.str());
	}
}

std::string TcpServer::buildResponse( void )
{
	std::string htmlFile ;//= "<!DOCTYPE html><html lang=\"en\"><body><h1> HOME </h1><p> Hello from your Server :) </p></body></html>";
	std::ifstream myFile;
	
	myFile.open("html_file/test.html");
	for ( ; myFile ;) {
		std::string temp;
		myFile >> temp;
		htmlFile += temp;
		htmlFile += " ";
	}
	myFile.close();
	std::ostringstream ss;
	ss << "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: " << htmlFile.size() << "\n\n"
       << htmlFile;

	return ss.str();
}

void TcpServer::sendResponse( void )
{
	long bytesSent;

	bytesSent = write(m_new_socket, m_serverMessage.c_str(), m_serverMessage.size());
	
	if (bytesSent == static_cast<long>(m_serverMessage.size())) {
		log("------ Server Response sent to client ------\n\n");
	} else {
		log("Error sending response to client");
	}
}
