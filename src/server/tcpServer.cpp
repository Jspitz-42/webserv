/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tcpServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jspitz <jspitz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 10:39:40 by altheven          #+#    #+#             */
/*   Updated: 2025/08/17 09:14:40 by jspitz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "tcpServer.hpp"
#include "Config.hpp"
#include "main.hpp"


TCPServer::TCPServer(std::string const & file) throw (std::exception) : _config(file)
{
	std::vector<Config::ServerConfig>::iterator it;
	std::vector<Config::ServerConfig> servers = _config._servers;

	if (_config._servers.size() > 1) {
		throw TCPServer::ErrorMessage(TCPSERVER_DUP_CONF);
	}

	const Config::ServerConfig &server = _config._servers.front();
	
	if (!server._locations.empty()) {

		std::set<std::string> seen;
		std::vector<Config::ServerConfig::Location>::const_iterator it_loc = server._locations.begin();

		for ( ; it_loc != server._locations.end() ; it_loc++) {
			const std::string & target = it_loc->_target;

			if (!seen.insert(target).second) {
				throw TCPServer::ErrorMessage(TCPSERVER_DUP_LOC);
			}
			
		}
	}

	_epollfd = epoll_create(10);

	if (_epollfd == -1)
		throw TCPServer :: ErrorMessage(TCPSERVER_ERR_MSG);
	try {
		createSocket(server.getIp(), server.getPort());
	} catch (std :: exception & e) {
		std :: cout << "[FAILED]" << e.what() << std :: endl;
		std::cout << "This Server Configuration contains errors, or an invalid [ip_address:port]. Please review the configuration file" << std::endl;
	}
}

void	TCPServer::printConfig( void ) const throw (std::exception)
{
	if (_config._servers.empty()) {
		throw TCPServer::ErrorMessage(TCPSERVER_PRINTCONFING_ERR);
	} else {
		std::vector<Config::ServerConfig>::const_iterator it_Conf = _config._servers.begin();
		int i = 1;
		for (  ; it_Conf != _config._servers.end() ; it_Conf++) {
	
			if (it_Conf->_autoindex) {
				std::cout << "Server configuration number [" << i << "] Autoindex : ON" << std::endl;
			} else {
				std::cout << "Server configuration number [" << i << "] Autoindex : OFF" << std::endl;
			}

			if (it_Conf->_listing) {
				std::cout << "Server configuration number [" << i << "] directory listing : ON" << std::endl;
			} else {
				std::cout << "Server configuration number [" << i << "] directory listing : OFF" << std::endl;
			}
			
			std::cout << "Server configuration number [" << i << "] Max body size : " << it_Conf->_max_body_size << std::endl; 
				
			std::cout << "Server configuration number [" << i << "] host ip : " << it_Conf->_ip << std::endl;
				
			std::cout << "Server configuration number [" << i << "] port : " << it_Conf->_port << std::endl;

			std::cout << "Server configuration number [" << i << "] Root : " << it_Conf->_root_path << std::endl;
	
			if (!it_Conf->_indexes.empty()) {
				std::vector<std::string>::const_iterator it_index = it_Conf->_indexes.begin();
				for ( ; it_index != it_Conf->_indexes.end() ; it_index++) {
					std::cout << "Server configuration number [" << i << "] indexes : " << *it_index << std::endl;
				}
			} else {
				std::cout << "Server configuration number [" << i << "] no index detected" << std::endl;
			}

			if (!it_Conf->_names.empty()) {
				std::vector<std::string>::const_iterator it_names = it_Conf->_names.begin();
				for ( ; it_names != it_Conf->_names.end() ; it_names++) {
					std::cout << "Server configuration number [" << i << "] names : " << *it_names << std::endl;
				}
			} else {
				std::cout << "Server configuration number [" << i << "] no name detected" << std::endl;
			}

			if (!it_Conf->_server_error_maps.empty()) {
				std::map<std::string, std::vector<int> >::const_iterator it_map = it_Conf->_server_error_maps.begin();
				for ( ; it_map != it_Conf->_server_error_maps.end() ; it_map++) {
					if (!it_map->first.empty() && !it_map->second.empty()) {
						std::cout << "Server configuration number [" << i << "] servers errors map first : " << it_map->first.c_str() << std::endl;
						std::vector<int>::const_iterator it_vector = it_map->second.begin();
						for ( ; it_vector != it_map->second.end() ; it_vector++) {
							std::cout << "Server configuration number [" << i << "] servers errors codes : " << *it_vector << std::endl;
						}
					}
				}
			} else {
				std::cout << "Server configuration number [" << i << "] no servers error code" << std::endl;
			}
			if (!it_Conf->_approved_methods.empty()) {
				std::vector<std::string>::const_iterator it_methods = it_Conf->_approved_methods.begin();
				for ( ; it_methods != it_Conf->_approved_methods.end() ; it_methods++) {
					std::cout << "Server configuration number [" << i << "] approved methods : " << *it_methods << std::endl;
				}
			} else {
				std::cout << "Server configuration number [" << i << "] no methods detected" << std::endl;
			}

			if (!it_Conf->_locations.empty()) {
				std::vector<Config::ServerConfig::Location>::const_iterator it_loc = it_Conf->_locations.begin();

				for (int a = 1; it_loc != it_Conf->_locations.end(); it_loc++) {

					if (!it_loc->_methods.empty()){
						std::vector<std::string>::const_iterator it_loc_methods = it_loc->_methods.begin();
						for ( ; it_loc_methods != it_loc->_methods.end() ; it_loc_methods++) {
							std::cout << "Server configuration number [" << i <<"] location number [" << a  << "] methods : " << *it_loc_methods << std::endl;
						}
					} else {
						std::cout <<  "Server configuration number [" << i <<"] location number [" << a << "] methods : none" << std::endl; 
					}

					if (!it_loc->_root_path.empty()) {
						std::cout << "Server configuration number [" << i <<"] location number [" << a  << "] root path : " << it_loc->_root_path << std::endl;
					} else {
						std::cout << "Server configuration number [" << i <<"] location number [" << a  << "] root path : none " << std::endl;
					}

					if (!it_loc->_target.empty()) {
						std::cout << "Server configuration number [" << i <<"] location number [" << a  << "] target : " << it_loc->_target << std::endl;
					} else {
						std::cout << "Server configuration number [" << i <<"] location number [" << a  << "] target : none " << std::endl;
					}

					if (!it_loc->_indexes.empty()) {
						std::vector<std::string>::const_iterator it_loc_indexes = it_loc->_indexes.begin();
						for ( ; it_loc_indexes != it_loc->_indexes.end() ; it_loc_indexes++) {

								std::cout << "Server configuration number [" << i 
								<< "] Location number [" << a << "] indexes : " << *it_loc_indexes << std::endl;
						}
					} else {
						std::cout << "Server configuration numver [" << i << "] Location number [" << a << "] no index detected" << std::endl;
					}

					std::cout << "Server configuration number [" << i << "] Location number [" << a << "] Max Body size : " << it_loc->_max_body_size << std::endl;

					std::cout << "Server configuration number [" << i << "] Location number [" << a << "] redirect status : " << it_loc->_redirect_status << std::endl;

					if (it_loc->_autoindex) {
						std::cout << "Server configuration number [" << i << "] Location number [" << a << "] auto index : ON" << std::endl;
					} else {
						std::cout << "Server configuration number [" << i << "] Location number [" << a << "] auto index : OFF" << std::endl;						
					}

					if (it_loc->_listing) {
						std::cout << "Server configuration number [" << i << "] Location number [" << a << "] diretory listing : ON" << std::endl;
					} else {
						std::cout << "Server configuration number [" << i << "] Location number [" << a << "] directory listing : OFF" << std::endl;
					}

					if (!it_loc->_cgi_bin.empty()) {
						std::cout << "Server configuration number [" << i << "] Location number [" << a << "] cgi bin : " << it_loc->_cgi_bin << std::endl;
					} else {
						std::cout << "Server configuration number [" << i << "] Location number [" << a << "] cgi bin : none" << std::endl;
					}

					if (!it_loc->_upload_path.empty()) {
						std::cout << "Server configuration number [" << i << "] Location number [" << a << "] Upload path : " << it_loc->_upload_path << std::endl;
					} else {
						std::cout << "Server configuration number [" << i << "] Location number [" << a << "] Upload path : none " << std::endl;
					}

					if (!it_loc->_redirect_uri.empty()) {
						std::cout << "Server configuration number [" << i << "] Location number [" << a << "] redirect uri : " << it_loc->_redirect_uri << std::endl;
					} else {
						std::cout << "Server configuration number [" << i << "] Location number [" << a << "] redirect uri : none" << std::endl;
					}
					if (!it_loc->_cgi_map.empty()) {
						std::multimap<std::string, std::string>::const_iterator it = it_loc->_cgi_map.begin();
						for ( ; it != it_loc->_cgi_map.end() ; it ++) {
							if ( !it->first.empty() && !it->second.empty())
							{
								std::cout << "Server configuration number [" << i 
								<< "] Location number [" << a << "] cgi detected : " << it->first << " " << it->second << std::endl;
							}
						}
					}
					++a;
					std::cout << std::endl;
				}
			}	
			std::cout << std::endl;
			i++;
		}
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
void	TCPServer :: acceptConnectionAt(int fd) throw (std :: exception)
{
	int					conn_sock;
	struct	epoll_event	ev;

	conn_sock = -2;
	if (_sockets.getSocketFd() == fd)
	{
		conn_sock = _sockets.acceptConnection();
		if (conn_sock == -1)
			throw TCPServer :: ErrorMessage(ACCEPTCONNECTION_ERR_MSG);
		
		ev.events = EPOLLIN | EPOLLET;
		ev.data.fd = conn_sock;
		
		if (epoll_ctl(_epollfd, EPOLL_CTL_ADD, conn_sock, &ev) == -1)
			throw TCPServer :: ErrorMessage(ACCEPTCONNEXTIONAT_ERR_MSG);
	}
	if (conn_sock != -2)
	{
		Client c(conn_sock, _sockets);
		_clients.push_back(c);
	}
}

void	TCPServer :: run()
{
	int		n;
	int		nfds;
	struct epoll_event	events[MAX_EVENTS];
	std :: vector<Client>::iterator cl_it ;
	
	
	timestamp_in_ms();
	while(1)
	{
		nfds = epoll_wait(_epollfd, events, MAX_EVENTS, 2000);
		if (nfds == -1)
			throw TCPServer :: ErrorMessage(TCPSERVER_RUN_ERR_MSG);
		for (n = 0; n < nfds; ++n)
		{
			if (isSocketFd(events[n].data.fd))
				acceptConnectionAt(events[n].data.fd);
			else if (events[n].events == EPOLLIN)
			{
				for (cl_it = _clients.begin();cl_it != _clients.end(); cl_it++)
				{
					if (cl_it->getFd() == events[n].data.fd)
					{
						cl_it->handleRequest(_config._servers.front());
						break;
					}
				}
			}
		}
		cleanEpollAndClientsList();
	}
}

void TCPServer::cleanEpollAndClientsList()
{
	UINT64_T										timestamp;
	std::vector<Client>::iterator					cl_it;
	
	timestamp = timestamp_in_ms();
	for (cl_it = _clients.begin(); cl_it != _clients.end();)
	{
		if (cl_it->timeToDie() < timestamp || !(cl_it->keepAlive()))
		{
			epoll_ctl(_epollfd, EPOLL_CTL_DEL, cl_it->getFd(), 0);
			shutdown(cl_it->getFd(), SHUT_RDWR);
			close(cl_it->getFd());
			cl_it = _clients.erase(cl_it);
		}
		else
			cl_it++;
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
