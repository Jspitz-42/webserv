/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlonghin <tlonghin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 11:54:23 by jspitz            #+#    #+#             */
/*   Updated: 2025/08/12 07:22:06 by tlonghin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include "Socket.hpp"
#include "Config.hpp"
Request::Request(std::string const & request, Config::ServerConfig const & sc) : _lock(NULL), _error_code(0),_server_config(sc)
{
	std::stringstream	ss(request);
	std::string			line;

	std::getline(ss, line);
	line = strtrim(line, " \r\t");

	std :: cout << line << std :: cout ;
	_method = line.substr(0, line.find_first_of(" \r\t"));
	_uri_target = line.substr(_method.length(), line.find_last_of(" \r\t") - _method.length());
	_http_version = line.substr(_method.length() + _uri_target.length());
	std::transform(_method.begin(), _method.end(), _method.begin(), ::toupper);
	std::transform(_http_version.begin(), _http_version.end(), _http_version.begin(), ::tolower);
	_uri_target = strtrim(_uri_target, " \r\t");
	_http_version = strtrim(_http_version, " \r\t");
	while (std::getline(ss, line) && line != "\r\n")
	{
		if (line.find(':') != std::string::npos) {
			std::string name(line.substr(0, line.find(':')));
			std::string	content(line.substr(line.find(':')));
			std::cout << "HEader : " << name << std::endl;
			std::transform(name.begin(), name.end(), name.begin(), ::tolower);
			_headers[name] = strtrim(content, ": \t");
		} else {
			break ;
		}
	}
	if (_headers.find("content-length") != _headers.end()) {
		int valread(0);
		std::stringstream num_ss(_headers["content-length"]);
		num_ss >> valread;

		while (std::getline(ss, line)) {
			_content += line;
		}
		_content = _content.substr(0, valread);	
	}
	if (_uri_target.length() > 1024) {
		_error_code = 414;
	} else {
		size_t q_start = _uri_target.find("?");
		size_t q_end = _uri_target.find("#");
		if (q_start != std::string::npos) {
			_query = _uri_target.substr(q_start, q_end - q_start);
			_uri_target = _uri_target.substr(0, q_start);
		}
		_lock = _server_config.findLocation(_uri_target);
		if (!_lock) {
			_error_code = 404;
			std::cerr << "Wrong target: [" << _uri_target << "] not found" << std::endl;
		} else {
			_final_path = _lock->_root_path + _uri_target.substr(_lock->_target.length());
			std::string tmp("Started: " + _method + " \"" + _uri_target + "\" ");
			std::cout.width(35);
			std::cout << std::left << tmp << "=>" << " Target Path [" << _final_path << "]" << std::endl;
		}
		if (!_lock->findMethod(_method)) {
			_error_code = 405;
		} else if (_http_version == "http/1.0") {
			_error_code = 505;
		} else if (_http_version != "http/1.1") {
			_error_code = 400;
		}
	}
	if (!_server_config.checkMaxBody(request.length()) && (_lock && !_lock->checkMaxBody(request.length())))
		_error_code = 413;
}

Request::~Request( void )
{
	if (_lock) delete _lock;

	return ;
}

int Request::getErrorCode( void ) const { return _error_code ;}

bool Request::isTargetDir( void ) const 
{
	if (_lock) {
		if (_lock->_target == _uri_target || (*(_uri_target.end() - 1)) == '/')
			return true;
	}

	return false;
}

bool Request::isTargetCGI( void ) const
{
	size_t pos(_uri_target.find('.'));
	
	if (_lock && !isTargetDir() && pos != std::string::npos) {
		std::string ext(_uri_target.substr(pos + 1));
		if (!(_lock->_cgi_bin.empty()) && (_lock->_cgi_map.find(ext) != _lock->_cgi_map.end()))
			return true;
	}

	return false;
}

bool Request::isTargetRedirect( void ) const
{
	if (_lock)
		if (_lock->_target == _uri_target && _lock->_redirect_status != 0 && !_lock->_redirect_uri.empty())
			return true;

	return false;
}

std::string const & Request::getContent( void ) const { return _content ;}
std::string const & Request::getQuery( void) const { return _query ;}
std::string const & Request::getMethod( void ) const { return _method ;}
std::string const & Request::getUriTarget( void ) const { return _uri_target;}
std::string const & Request::getFinalPath( void ) const { return _final_path ;}

const std::string Request::getIndex( void ) const 
{
	std::string location;
	std::vector<std::string>::iterator i_it;
	std::vector<std::string>::const_iterator c_it;

	for (i_it = _lock->_indexes.begin() ; i_it != _lock->_indexes.end() ; i_it++) {
		location = getFinalPath() + *i_it;
		if (ifFile(location)) 
			return location;
	}

	return ("");
}

const std::string Request::getCGIBindPath( void ) const
{
	if (_lock)
		if (!_lock->_cgi_bin.empty())
			return (_lock->_cgi_bin);
	
	return ("");
}

const std::string Request::getCGIFile( void ) const 
{
	std::vector<std::string>::iterator	v_it;
	size_t								pos(_uri_target.find_last_of('.'));

	if (_lock && isTargetCGI() && !_lock->_cgi_map.empty() && pos != std::string::npos) {
		std::string ext(_uri_target.substr(pos + 1));
		std::string file_no_ext(_uri_target.substr(0, pos));
		if (_lock->_cgi_map.find(ext) != _lock->_cgi_map.end()) { 
			for (v_it = _lock->_cgi_map[ext].begin() ; v_it != _lock->_cgi_map[ext].end() ; v_it++) {
				size_t v_pos(v_it->find_last_of('.'));
				std::string v_it_no_ext(*v_it);
				if (pos != std::string::npos) {
					v_it_no_ext = v_it_no_ext.substr(0, v_pos);
				}
				if (v_it_no_ext.find(file_no_ext) != std::string::npos || file_no_ext.find(v_it_no_ext) != std::string::npos)
					return *v_it;
			}
			return _lock->_cgi_map.find(ext)->second.front();
		}
	}
	return "";
}

const std::string Request::getAcceptEncoding( void ) const
{
	if (_headers.find("accept-encoding") != _headers.end()) {
		return  (_headers.find("accept-encoding")->second);
	} else {
		return ("");
	}
}

const std::string Request::getUserAgent( void ) const 
{
	if (_headers.find("user-agent") != _headers.end()) {
		return (_headers.find("user-agent")->second);
	} else {
		return ("");
	}
}

const std::string Request::getContentType( void ) const 
{
	if (_headers.find("content-type") != _headers.end()) {
		return (_headers.find("content-type")->second);
	} else {
		return ("");
	}
}

const std::string Request::getCookies( void ) const 
{
	if (_headers.find("Cookie") != _headers.end()) {
		return (_headers.find("Cookie")->second);
	} else {
		return ("");
	}
}

bool	Request::getClientId(std::string uuid) const {
	std::cout << uuid << std::endl;
	for (int i = 0; i < 2048; i++) {
		if (const_cast<SessionClient&>(this->_clientList[i]).getUUID() == uuid)
			return (true);
	}
	return (false);
}

std::string generate_uuid_v4() {
	std::stringstream ss;

	for (int i = 0; i < 16; ++i) {
		int byte = std::rand() % 256;
		if (i == 6) {
			byte = (byte & 0x0F) | 0x40;
		}
		else if (i == 8) {
			byte = (byte & 0x3F) | 0x80;
		}
		ss << std::setw(2) << std::setfill('0') << std::hex << byte;
		if (i == 3 || i == 5 || i == 7 || i == 9)
			ss << "-";
	}
	return ss.str();
}



const std::string Request::createClientId() {
	std::string uuidV4 = generate_uuid_v4();
	for (int i = 0; i < 2048; i++) {
		if (this->_clientList[i].getUUID().empty()) {
			this->_clientList[i].setUUID(uuidV4);
			return (uuidV4);
		}
	}
	return ("");
}