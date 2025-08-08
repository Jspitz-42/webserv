/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tcpServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jspitz <jspitz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 12:41:50 by jspitz            #+#    #+#             */
/*   Updated: 2025/08/08 13:23:38 by jspitz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "tcpServer.hpp"
#include "main.hpp"


bool webserv_run = true;

void exit_webserv(int param)
{
	(void)param;
	webserv_run = false;
	std::cout << std::endl <<  "Exit webserv..." << std::endl;
}

TCPServer::TCPServer(std::string const & file) throw (std::exception) : _config(file)
{
	std::cout << "constructor TCPServer called" << std::endl;
	std::vector<Config::ServerConfig>::iterator it;
	std::vector<Config::ServerConfig> servers = _config._servers;
	_epollfd = epoll_create(10);
	if (_epollfd == -1)
	   throw TCPServer::ErrorMessage(TCPSERVER_ERR_MSG);
	for (it = servers.begin(); it != servers.end(); ++it) {

		try {
			getOrCreateSocket(it->getIp(), it->getPort()).addServerConf(*it);
		} catch (std::exception & e) {
			std::cout << "[FAILED] " << e.what() << std::endl;
			std::cout << "This Server Configuration contains errors, or an invalid [ip_address:port]. Please review the configuration file" << std::endl;
		}
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

	it = _sockets.begin();
	conn_sock = (*it).acceptConnection();
	if (conn_sock == -1) {
		throw TCPServer::ErrorMessage(ACCEPTCONNECTION_ERR_MSG);
	}
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
	Client c(conn_sock, *it);
	if (_clients.find(fd) != _clients.end())
		_clients.find(fd)->second.push_back(c);
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

	signal(SIGINT, exit_webserv);
	initMsg();
	timestamp_in_ms();
	
	while (1)
	{
		nfds = epoll_wait(_epollfd, events, MAX_EVENTS, 2000);
		if (!webserv_run)
			break;
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
	uint64_t										timestamp;
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

/*
const int BUFFER_SIZE = 30720;

void log(const std::string & message)
{
	std::cout << message << std::endl;
}

int TCPServer::startServer()
{
	m_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket < 0)
	{
		throw TCPServer::ErrorMessage("socket: creation failed");
	}

	std::cerr <<  " socket = " << m_socket << std::endl;
	if (bind(m_socket, (sockaddr*)&m_socketAddress, m_socketAddress_len) < 0)
	{
		throw TCPServer::ErrorMessage("bind: cannot connect socket");
	}

	return 0;
}

//TCPServer::TCPServer(std::string & file) : _config(file)
//{
//	std::vector<Config::ServerConfig>::iterator it;
//	std::vector<Config::ServerConfig> servers = _config._servers;
//	
//	_efd = epoll_create(10);
//	if (_efd < 0)
//		throw TCPServer::ErrorMessage("ERROR: [TCPServer::TCPServer] [epoll_create] : failed to create epoll fd");
//	
//	for (it = servers.begin() ; it != servers.end() ; ++ it) {
//		
//	}
//	
//	return ;
//}

TCPServer::TCPServer(std::string ip_address, int port) :	m_ip_addr(ip_address),
															m_port(port), m_socket(),
															m_new_socket(), m_socketAddress(),
															m_socketAddress_len(sizeof(m_socketAddress)),
															m_serverMessage(buildResponse())
{
	std::memset(&_ev, 0, sizeof(_ev));
	std::memset(&_evlist, 0, sizeof(_evlist));
	std::memset(&m_socketAddress, 0, sizeof(m_socketAddress));
	std::memset(&csin, 0, sizeof(csin));

	m_socketAddress.sin_family = AF_INET;
	m_socketAddress.sin_port = htons(m_port);
	m_socketAddress.sin_addr.s_addr = inet_addr(m_ip_addr.c_str());

	if (startServer() != 0)
	{
		std::ostringstream ss;

		ss << "Failed to start server with PORT:" << ntohs(m_socketAddress.sin_port);
		throw TCPServer::ErrorMessage(ss.str());
	}

}

TCPServer::~TCPServer()
{
	closeServer();
}

void TCPServer::closeServer( void )
{
	close(m_socket);
	close(m_new_socket);
}

void setnonblocking(int fd)
{
	int flags = fcntl(fd, F_GETFL, 0);
	
  	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0)
		throw TCPServer::ErrorMessage("fcntl: failures");//error checking to be done here
}

bool TCPServer::findClientFile( void )
{
	std::vector<int>::iterator it = _cfds.begin();

	for ( ; it != _cfds.end() ; it++) {
		if (*it == _cfd)
			return true;
	}
	return false;
}

void TCPServer::startListen()
{
	if (listen(m_socket, 20) < 0)
	{
		throw TCPServer::ErrorMessage("Socket: listen: failed");
	}

	std::ostringstream ss;
	ss << std::endl <<  "*** Listening on ADDRESS: " << inet_ntoa(m_socketAddress.sin_addr) << " PORT: "  << ntohs(m_socketAddress.sin_port) << " ***" << std::endl << std::endl;
	log(ss.str());

	_efd = epoll_create1(0);

	if (_efd < 0) {
		perror("epoll_create1");
		throw TCPServer::ErrorMessage("epoll_create1: failed");
	}

	_ev.events = EPOLLIN;
	_ev.data.fd = m_socket;
	_ret = epoll_ctl(_efd, EPOLL_CTL_ADD, m_socket, &_ev);

	if (_ret < 0) {
		throw TCPServer::ErrorMessage("epoll_ctl: failed");
	}

	int i, buflen;
	char	buf[BUFFER_SIZE];

	while (1)
	{
		_nfds = epoll_wait(_efd, _evlist, MAX, -1);
		
		if (_ret == -1) {
			perror("epoll_wait");
			throw TCPServer::ErrorMessage("epoll_wait: failed");
		}
		for (i = 0 ; i <= _nfds ; i++) {

				if (_evlist[i].data.fd == m_socket) {
				
				
					_cfd = accept(m_socket, (struct sockaddr *)&csin, &m_socketAddress_len);
				
					if (_cfd == -1) {
						throw TCPServer::ErrorMessage("accept: failed");
						return ;
					}
				
					std::cout << "\x1b[0;32m[*] accept\x1b[0m" << std::endl;
					
					setnonblocking(_cfd);
					_ev.events = EPOLLIN;
					_ev.data.fd = _cfd;
					_ret = epoll_ctl(_efd, EPOLL_CTL_ADD, _cfd, &_ev);
				
					if (_ret == -1) {
						perror("epoll_ctl");
						throw TCPServer::ErrorMessage("epoll_ctl: failed");
					}
					
					
					// add the accepted client to a list of connected clients
					_cfds.push_back(_cfd);

				}

				_cfd = _evlist[i].data.fd;
				if (findClientFile()) { // check if its a connected client
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
							throw TCPServer::ErrorMessage("close: failed");
						}
					
						std::cout << "\x1b[0;31m[*] close \x1b[0m" << std::endl;
					
						epoll_ctl(_efd, EPOLL_CTL_DEL, _cfd, &_evlist[i]);
					
						if (_ret == -1) {
							perror("epoll_ctl");
							throw TCPServer::ErrorMessage("epoll_ctl: failed");
						}
						// remove the client from the list
						_cfds.erase(std::remove(_cfds.begin(), _cfds.end(), _cfd), _cfds.end());
		
					
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

void TCPServer::AcceptConnection(int & new_socket)
{
	new_socket = accept(m_socket, (sockaddr *)&m_socketAddress, &m_socketAddress_len);
	if (new_socket < 0)
	{
		std::ostringstream ss;
		ss << "Server failed to accept incoming connection from ADDRESS: " << inet_ntoa(m_socketAddress.sin_addr) << "; PORT: " << ntohs(m_socketAddress.sin_port);
		throw TCPServer::ErrorMessage(ss.str());
	}
}

std::string TCPServer::buildResponse( void )
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

void TCPServer::sendResponse( void )
{
	long bytesSent;

	bytesSent = write(m_new_socket, m_serverMessage.c_str(), m_serverMessage.size());
	
	if (bytesSent == static_cast<long>(m_serverMessage.size())) {
		log("------ Server Response sent to client ------\n\n");
	} else {
		log("Error sending response to client");
	}
}
*/