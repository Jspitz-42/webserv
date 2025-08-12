/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tcpServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: altheven <altheven@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 12:41:50 by jspitz            #+#    #+#             */
/*   Updated: 2025/08/12 14:17:04 by altheven         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "tcpServer.hpp"
# include "Config.hpp"
#include "main.hpp"

TCPServer::TCPServer(std::string const & file) throw (std::exception) : _config(file)
{
	std::cout << "constructor TCPServer called" << std::endl;
	std::vector<Config::ServerConfig>::iterator it;
	std::vector<Config::ServerConfig> servers = _config._servers;
	for (it = servers.begin(); it != servers.end(); ++it) {

		try {
			getOrCreateSocket(it->getIp(), it->getPort()).addServerConf(*it);
		} catch (std::exception & e) {
			std::cout << "[FAILED] " << e.what() << std::endl;
			std::cout << "This Server Configuration contains errors, or an invalid [ip_address:port]. Please review the configuration file" << std::endl;
		}
	}
	_epollfd = epoll_create(10);
	if (_epollfd == -1)
	   throw TCPServer::ErrorMessage(TCPSERVER_ERR_MSG);
}

void	TCPServer::printConfig( void ) const
{
	if (_config._servers.begin()->_autoindex == true)
		std::cout << "Autoindex = ON" << std::endl;
	else 
		std::cout << "Autoindex = OFF" << std::endl;
	std::cout << "Listing = " << _config._servers.begin()->_listing << std::endl;
	std::cout << "_config._servers.begin()->_max_body_size = " << _config._servers.begin()->_max_body_size << std::endl;
	std::cout << "_config._servers.begin()->ip = " << _config._servers.begin()->_ip << std::endl;
	std::cout << "_config._servers.begin()->_port = " << _config._servers.begin()->_port << std::endl;
	std::cout << "_config._servers.begin()->_root path = " << _config._servers.begin()->_root_path << std::endl;

	std::vector<std::string>::const_iterator it = _config._servers.begin()->_approved_methods.begin();
	for (size_t i = 0 ; i < _config._servers.begin()->_approved_methods.size() ; i++)
	{
	std::cout << "_config._servers.begin()->_approved_methods it			= " << *it << std::endl;
		it++;
	}
}

Socket & TCPServer::getOrCreateSocket(std::string const & ip, int port)
{
	std::vector<Socket>::iterator it;

	for (it = _sockets.begin(); it != _sockets.end(); it++) {
		if (it->getPort() == port && ip == it->getIpAdress()) {
 			return (*it);
		}
	}
	
	Socket s(ip, port);
	addSocket(s);
	
	std::pair<int, std::vector<Client> > p(s.getSocketFd(), std::vector<Client>());
	
	_clients.insert(p);

	return (_sockets.back());
}

TCPServer::~TCPServer()
{
	std::vector<Socket>::iterator					it;
	std::vector<Client>::iterator					v_it;
	std::map<int, std::vector<Client> >::iterator	m_it;

	for (it = _sockets.begin(), m_it = _clients.begin(); it != _sockets.end() && m_it != _clients.end(); it++, ++m_it) {
		for (v_it = m_it->second.begin(); v_it != m_it->second.end(); ++v_it) {
			shutdown(v_it->getFd(), SHUT_RDWR);
			close(v_it->getFd());
			std::cout << " * Closing Connection: " << v_it->getFd() << std::endl;
		}
	}
	for (it = _sockets.begin(); it != _sockets.end(); it++) {
		shutdown(it->getSocketFd(), SHUT_RDWR);
		close(it->getSocketFd());
		std::cout << " * Closing Socket: " << it->getSocketFd() << std::endl;
	}
}


void TCPServer::acceptConnectionAt(int fd) throw (std::exception)
{
	int								conn_sock;
	struct epoll_event				ev;
	std::vector<Socket>::iterator	it;

	conn_sock = -2;
	for (it = _sockets.begin(); it != _sockets.end(); ++it) {
		if (it->getSocketFd() == fd) {
			conn_sock = (*it).acceptConnection();
			if (conn_sock == -1)
			   throw TCPServer::ErrorMessage(ACCEPTCONNECTION_ERR_MSG);
			
			ev.events = EPOLLIN | EPOLLET;
			ev.data.fd = conn_sock;

			if (epoll_ctl(_epollfd, EPOLL_CTL_ADD, conn_sock, &ev) == -1)
			   throw TCPServer::ErrorMessage(ACCEPTCONNEXTIONAT_ERR_MSG);
			break ;
		}
	}
	if (conn_sock != -2)
	{
		Client c(conn_sock, *it);
		if (_clients.find(fd) != _clients.end())
			_clients.find(fd)->second.push_back(c);
	}
}


void TCPServer::addSocket(Socket & s) throw (std::exception)
{
	struct epoll_event	ev;

	ev.events = EPOLLIN;
	ev.data.fd = s.getSocketFd();
	if (epoll_ctl(_epollfd, EPOLL_CTL_ADD, s.getSocketFd(), &ev) == -1)
		throw TCPServer::ErrorMessage(ADDSOCKET_ERR_MSG);
	_sockets.push_back(s);
}

int TCPServer::getEpollFd() const 
{
	return (_epollfd);
}

bool TCPServer::isSocketFd(int fd)
{
	std::vector<Socket>::iterator it;
	std::vector<Socket>::iterator end = _sockets.end();
	for (it = _sockets.begin(); it != end; it++) {
		if (it->getSocketFd() == fd)
			return (true);
	}
	return (false);
}

int TCPServer::numSockets() const
{
	return (_sockets.size());
}

void TCPServer::initMsg()
{
	std::vector<Socket>::iterator	it;
	std::map<int, std::vector<Client> >::iterator	m_it;

	std::cout << "=> Booting webserv" << std::endl;
	std::cout << "=> HTTP server starting" << std::endl;
	std::cout << "=> Run `./webserv server --help` for more startup options" << std::endl;
	std::cout << "Starting with single thread mode..." << std::endl;
	std::cout << "* Version: 1.0 (c++98) (\"TBP's Version\")" << std::endl;
	std::cout << "*          PID: " << getpid() << std::endl;
	for (it = _sockets.begin(), m_it = _clients.begin(); it != _sockets.end() && m_it != _clients.end(); it++, ++m_it)
		std::cout << "* Listening on " << *it << " " << m_it->first << " => #Clients: " << m_it->second.size() << std::endl;
	std::cout << "Use Ctrl-C to stop" << std::endl;
}

void TCPServer::run()
{
	int												n;
	int												nfds;
	struct epoll_event								events[MAX_EVENTS];
	std::vector<Client>::iterator					v_it;
	std::map<int, std::vector<Client> >::iterator	m_it;

	initMsg();
	timestamp_in_ms();
	while (1)
	{
		nfds = epoll_wait(_epollfd, events, MAX_EVENTS, 2000);
		if (nfds == -1)
		   throw TCPServer::ErrorMessage(TCPSERVER_RUN_ERR_MSG);
		for (n = 0; n < nfds; ++n) {
			if (isSocketFd(events[n].data.fd)) {
				acceptConnectionAt(events[n].data.fd);
			} else if (events[n].events == EPOLLIN) {
				for (m_it = _clients.begin(); m_it != _clients.end(); ++m_it) {
					for (v_it = m_it->second.begin(); v_it != m_it->second.end(); ++v_it) {
						if (v_it->getFd() == events[n].data.fd) {
							v_it->handleRequest();
							break;
					 	}
					}
				}
		   }
		}
		cleanEpollAndClientsList();
	}
}

void TCPServer::removeClient(std::pair<int, int> & pair)
{
	std::map<int, std::vector<Client> >::iterator	m_it;
	std::vector<Client>::iterator					v_it;

	m_it = _clients.find(pair.first);
	if (m_it == _clients.end())
		return ;
	for (v_it = m_it->second.begin(); v_it != m_it->second.end(); ++v_it) {
		if (v_it->getFd() == pair.second) {

			epoll_ctl(_epollfd, EPOLL_CTL_DEL, v_it->getFd(), 0);
			shutdown(v_it->getFd(), SHUT_RDWR);
			close(v_it->getFd());
			_clients[pair.first].erase(v_it);
			return ;
		}
	}
}

void TCPServer::cleanEpollAndClientsList()
{
	UINT64_T										timestamp;
	std::vector<Client>::iterator					v_it;
	std::vector<std::pair<int, int> >				vi_it;
	std::map<int, std::vector<Client> >::iterator	m_it;

	timestamp = timestamp_in_ms();
	for (m_it = _clients.begin(); m_it != _clients.end() ; ++m_it) {
		for (v_it = m_it->second.begin(); v_it != m_it->second.end(); ++v_it) {

			if (v_it->timeToDie() < timestamp || !(v_it->keepAlive())) {
				std::pair<int, int> p(m_it->first, v_it->getFd());
				vi_it.push_back(p);
			}
		}
	}
	for (std::vector<std::pair<int, int> >::iterator it = vi_it.begin(); it < vi_it.end(); ++it) {
		removeClient(*it);
	}
}
