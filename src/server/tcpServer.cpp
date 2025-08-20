/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tcpServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: altheven <altheven@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 10:39:40 by altheven          #+#    #+#             */
/*   Updated: 2025/08/20 20:00:55 by tlonghin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "tcpServer.hpp"
#include "Config.hpp"
#include "main.hpp"


TCPServer::TCPServer(std::string const & file) throw (std::exception) : _config(file)
{
	std::vector<Config::ServerConfig>::iterator it;

	if (_config._servers.size() > 1) {
		throw TCPServer::ErrorMessage(TCPSERVER_DUP_CONF);
	}

	const Config::ServerConfig &server = _config._servers.front();
	
	bool found_standard = false;
	if (!server._locations.empty()) {

		std::set<std::string> seen;
		std::vector<Config::ServerConfig::Location>::const_iterator it_loc = server._locations.begin();

		for ( ; it_loc != server._locations.end() ; it_loc++) {
			const std::string & target = it_loc->_target;

			if (it_loc->_target == "/")
				found_standard = true;
			if (!seen.insert(target).second) {
				throw TCPServer::ErrorMessage(TCPSERVER_DUP_LOC);
			}
			bool isRedir = (it_loc->_target == "/redirect");
			if (isRedir == true) {
				continue ;
			}
			if (isRedir == false && it_loc->_root_found != true) {
				throw TCPServer::ErrorMessage("Error: [TCPServer] [LOCATION VECTOR] [" + it_loc->_target + "] [INVALID DIRECTIVE] : No root path set");
			}
		}
	}

	if (!found_standard) {
		throw TCPServer::ErrorMessage("ERROR: [LOCATION] [STANDARD \"/\"] : not found");
	}

	_epollfd = epoll_create1(0);
  g_epoll_fd = _epollfd;
	if (_epollfd == -1)
		throw TCPServer :: ErrorMessage(TCPSERVER_ERR_MSG);
	try {
		createSocket(server.getIp(), server.getPort());
	} catch (std :: exception & e) {
		std :: cout << "[FAILED]" << e.what() << std :: endl;
		std::cout << " Server Configuration contains errors, or an invalid [ip_address:port]. Please review the configuration file" << std::endl;
	}
}


void TCPServer :: createSocket(std :: string const & ip, int port)
{
	Socket s(ip, port);
	addSocket(s);
}

void	TCPServer :: addSocket(Socket & s) throw (std :: exception)
{
	struct epoll_event ev;

	ev.events = EPOLLIN;
	ev.data.fd = s.getSocketFd();
	if (epoll_ctl(_epollfd, EPOLL_CTL_ADD, s.getSocketFd(), &ev) == -1)
		throw TCPServer :: ErrorMessage(ADDSOCKET_ERR_MSG);
	_sockets = s;
}

int TCPServer::getEpollFd() const 
{
	return (_epollfd);
}

bool TCPServer::isSocketFd(int fd)
{
	if (_sockets.getSocketFd() == fd)
		return (true);
	return (false);
}
void TCPServer::acceptConnectionAt(int fd) throw (std::exception)
{
    int conn_sock = -1;
    struct epoll_event ev;

    if (_sockets.getSocketFd() == fd) {
        conn_sock = _sockets.acceptConnection();
        if (conn_sock == -1)
            throw TCPServer::ErrorMessage(ACCEPTCONNECTION_ERR_MSG);

        ev.events = EPOLLIN | EPOLLRDHUP | EPOLLERR | EPOLLHUP;
        ev.data.fd = conn_sock;
        if (epoll_ctl(_epollfd, EPOLL_CTL_ADD, conn_sock, &ev) == -1)
            throw TCPServer::ErrorMessage(ACCEPTCONNEXTIONAT_ERR_MSG);

        Client c(conn_sock, _sockets);
        _clients.push_back(c);
    }
}

void TCPServer::cleanEpollAndClientsList()
{
    UINT64_T timestamp = timestamp_in_ms();
    std::vector<Client>::iterator it;

    for (it = _clients.begin(); it != _clients.end();) {
        if (it->timeToDie() < timestamp || !(it->keepAlive())) {
            epoll_ctl(_epollfd, EPOLL_CTL_DEL, it->getFd(), NULL);
            shutdown(it->getFd(), SHUT_RDWR);
            close(it->getFd());
            it = _clients.erase(it);
        } else {
            ++it;
        }
    }
}

void TCPServer::run()
{
    int nfds, n;
    struct epoll_event events[1024];

    while (1) {
        nfds = epoll_wait(_epollfd, events, 1024, -1);
        if (nfds == -1)
            throw TCPServer::ErrorMessage(TCPSERVER_RUN_ERR_MSG);

        for (n = 0; n < nfds; ++n) {
            int fd = events[n].data.fd;

            if (isSocketFd(fd)) {
                acceptConnectionAt(fd);
            } else if (events[n].events & (EPOLLIN | EPOLLRDHUP | EPOLLERR | EPOLLHUP)) {
                std::vector<Client>::iterator cl_it;
                for (cl_it = _clients.begin(); cl_it != _clients.end(); ++cl_it) {
                    if (cl_it->getFd() == fd) {
                        cl_it->handleRequest(_config._servers.front());
                        break;
                    }
                }
            }
        }
        cleanEpollAndClientsList();
    }
}

TCPServer::~TCPServer()
{
	std::vector<Client>::iterator					cl_it;

	for (cl_it = _clients.begin(); cl_it != _clients.end(); ++cl_it) 
	{
			shutdown(cl_it->getFd(), SHUT_RDWR);
			close(cl_it->getFd());
			std::cout << " * Closing Connection: " << cl_it->getFd() << std::endl;
	}
	shutdown(_sockets.getSocketFd(), SHUT_RDWR);
	close(_sockets.getSocketFd());
	std::cout << " * Closing Socket: " << _sockets.getSocketFd() << std::endl;
}
