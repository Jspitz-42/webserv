/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Error.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jspitz <jspitz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/08 09:10:17 by jspitz            #+#    #+#             */
/*   Updated: 2025/08/17 08:59:24 by jspitz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once


/**
 * error message for Socket class error handling
 */
# define SOCKET_FAILURES "ERROR: [SOCKET] [socket returned -1] [ Socket attribution failed]"
# define INADDR_NONE_ERR_MSG "ERROR: [SOCKET] [_address.sin_addr.s_addr == INADDR_NON] [inet_addr failure | Ip invalid]"
# define SETSOCKOPT_ERR_MSG "ERROR: [SOCKET] [setsockopt returned -1]"
# define BIND_ERR_MSG "ERROR: [SOCKET] [can't bind socket]"
# define LISTEN_ERR_MSG "ERROR: [SOCKET] [can't bind socket]"

/**
 * error message for TcpServer class error handling
 */
# define TCPSERVER_ERR_MSG "ERROR: [TCPServer::TCPServer] [epoll_create] [Failed to return a file descriptor]"
# define ACCEPTCONNEXTIONAT_ERR_MSG "ERROR: [TCPServer::acceptConnection] [_epoll_ctl] [Epoll Failed to add a file descriptor]"
# define ADDSOCKET_ERR_MSG "ERROR: [TCPServer::addSocket] [Epoll_ctl] [Failed to add a file descriptor]"
# define ACCEPTCONNECTION_ERR_MSG "ERROR: [TCPServer::acceptConnection] [Failed to accept connection from socket]"
# define TCPSERVER_RUN_ERR_MSG "ERROR: [TCPServer::run] [Epoll Failed to wait and return the events]"
# define TCPSERVER_PRINTCONFING_ERR "ERROR: [TCPSERVER::printConfig] : No Configuration detected"
# define TCPSERVER_DUP_CONF "ERROR: [TCPServer] [duplicate configuration]"
# define TCPSERVER_DUP_LOC "ERROR: [TCPServer::_config] [duplicate location]"

/**
 * error message for Config class error handling
 */

# define CONFIG_INVALID_FILE "ERROR: [Config::Config] [Invalid file]"
# define CONFIG_INVALID_DIR_EMPTY "ERROR: [Config::Config] [Invalid Directive] : Directive is empty"
# define CONFIG_CASE_0_ERR "ERROR: [Config::Config: switch] [case 0] : invalid Directive"
# define CONFIG_CASE_SERVER_CONTEXT_ERR_1 "ERROR: [Config::Config] [switch :case SERVER_CONTEXT] : Wrong Syntax"
# define CONFIG_CASE_SERVER_CONTEXT_ERR_2 "ERROR: [Config::Config] [switch: case SERVER_CONTEXT] : Invalid Directive"
# define CONFIG_CASE_LOCATION_CONTEXT_ERR_1 "ERROR: [Config::Config] [switch : case LOCATION_CONTEXT] : wrong Syntax"
# define CONFIG_CASE_LOCATION_CONTEXT_ERR_2 "ERROR: [Config::Config] [switch: case LOCATION_CONTEXT] : Invalid directive"
# define CONFIG_CASE_LOCATION_CONTEXT_ERR_3 "ERROR: [Config::Config] [switch: case default] : Invalid Directive"

# define AUTOINDEX_INVALID_DIR "ERROR: [Config::ServerConfig::AutoIndex::AutoIndex] : Invalid Directive"

# define CLIENTMAXBODYSIZE_WRONG_SYNTAX "ERROR: [Config::ServerConfig::ClientMaxBodySize::ClientMaxBodySize] : Wrong Syntax"
# define CLIENTMAXBODYSIZE_INVALID_DIR	"ERROR: [Config::ServerConfig::ClientMaxBodySize::ClientMaxBodySize]: Invalid Directive"

# define CGI_WRONG_SYNTAX "ERROR: [Config::ServerConfig::Cgi::Cgi] : Wrong Syntax"
# define CGI_PARSCONTENT_INVALID_DIR  "ERROR: [Config::ServerConfig::Cgi::_parseCgiContent] : Invalid Directive"
# define CGI_SETDIRECTIVE_ERR "Error: [CGI] [NO lOC] : cgi out of location context"

# define CGIBIN_WRONG_SYNTAX "ERROR: [Config::ServerConfig::CgiBin::CgiBin] : Wrong syntax"

# define ERRORCODEPAGE_WRONG_SYNTAX "ERROR: [Config::ServerConfig::ErrorCodePage::ErrorCodePage] : Wrong Syntax"
# define ERRORCODEPAGE_INVALID_DIR "ERROR: [Config::ServerConfig::ErrorCodePage::ErrorCodePage] : Invalid Directive"

# define INDEX_WRONG_SYNTAX "ERROR: [Config::ServerConfig::Index::Index] : Wrong syntax"

# define METHODS_INVALID_DIR_1 "ERRROR: [Config::ServerConfig::Methods::Methods] [_validMethod false ] : Invalid Directive"
# define METHODS_INVALID_DIR_2 "ERROR: [Config::ServerConfig::Methods::Methods] [double Methods]: Invalid Directive"

# define LISTEN_WRONG_SYNTAX "ERROR: [Config::ServerConfig::Listen::Listen] : Wrong syntax"
# define LISTEN_INVALID_IP "ERROR: [Config::ServerConfig::Listen::Listen] [ip invalid or port invalid]: Invalid Directive"
# define LISTEN_OUT_OF_RANGE "ERROR: [Config::ServerConfig::Listen::Listen] [Port out of range] : Invalid Directive"

# define LOCATION_WRONG_SYNTAX_1 "ERROR: [Config::ServerConfig::Location::Location] [content] : Wrong Syntax"
# define LOCATION_WRONG_SYNTAX_2 "ERROR: [Config::ServerConfig::Location::Location] [_target] : Wrong Syntax"

# define REDIRECT_WRONG_SYNTAX_1 "ERROR: [Config::ServerConfig::Redirect::Redirect] [_redirect_status_code] : Wrong Syntax"
# define REDIRECT_WRONG_SYNTAX_2 "ERROR: [Config::ServerConfig::Redirect::Redirect] [tmp.empty | unwanted Separators] : Wrong Syntax"

# define ROOT_WRONG_SYNTAX "ERROR: [Config::ServerConfig::Root::Root] : Wrong Syntax"

# define UPLOAD_WRONG_SYNTAX "ERROR: [Config::ServerConfig::Upload::Upload] : Wrong Syntax"

# define LISTING_INVALID_DIR "ERROR: [Directory Listing] [INVALID DIRECTIVE]"

// main file error msg
# define MAIN_ARGUMENT_ERRROR "ERROR: [Too many arguments] [please: only one is required] [tips: one is less than two]"
# define SIGINT_QUIT_MSG "Server: quit with ctrl + c (SIGINT)"
# define SIGINT_ERR_MSG "ERROR: [set_sig_handler] [SIGACTION FAILURES]"
