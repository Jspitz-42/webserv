/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: altheven <altheven@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 11:54:23 by jspitz            #+#    #+#             */
/*   Updated: 2025/08/23 14:36:05 by altheven         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include "Socket.hpp"
#include "Config.hpp"

 Request :: Request(std :: string const & request, Config :: ServerConfig const & sc) : _lock(NULL), _error_code(0), _server_config(sc)
{
	size_t pos = 0;
	size_t lineEnd = request.find('\n', pos);
	
	std :: string	host;
	size_t			hpos;
	
	hpos = request.find("Host");
	if (hpos == std::string::npos)
	{
			_error_code = 400;
			return ;
	}
	host = request.substr(hpos);
	hpos = host.find("/n");
	if (hpos == std::string::npos || hpos <= 0)
	{
			_error_code = 400;
			return ;
	}
	host = request.substr(0, hpos - 1);
	
	if (lineEnd == std::string::npos) {
		_error_code = 400;
		return;
	}
	std::string firstLine = request.substr(pos, lineEnd - pos);
	std::cout << firstLine << std::endl;
	firstLine = strtrim(firstLine, " \r\t\n");
		
	_method = firstLine.substr(0, firstLine.find_first_of(" \r\t"));
	_uri_target = firstLine.substr(_method.length(), firstLine.find_last_of(" \r\t") - _method.length());
	_http_version = firstLine.substr(_method.length() + _uri_target.length());
	
	std::transform(_method.begin(), _method.end(), _method.begin(), ::toupper);
	std::transform(_http_version.begin(), _http_version.end(), _http_version.begin(), ::tolower);
	
	_uri_target = strtrim(_uri_target, " \r\t");
	_http_version = strtrim(_http_version, " \r\t");

	pos = lineEnd + 1;

	if (_method.empty() || _http_version.empty() || (_http_version != "http/1.1" && _http_version != "http/1.0")) {
    	_error_code = 400;
    	return;
	}

	size_t pos_q = _uri_target.find('?');
	size_t pos_frag = _uri_target.find('#');

	if (pos_q != std::string::npos)
	{
		size_t qbeg = pos_q + 1;
		size_t qlen = (pos_frag == std::string::npos) ? std::string::npos : (pos_frag - qbeg);

		_query = _uri_target.substr(qbeg, qlen);
		_uri_target.erase(pos_q);
	} 
	else if (pos_frag != std::string::npos) 
	{
		_uri_target.erase(pos_frag);
	}
	if (_method != "DELETE" && _uri_target != "/redirect")
		_lock = _server_config.findLocation(sc._root_path + _uri_target);
	else if (!_lock || _method == "DELETE") {
		_lock = _server_config.findLocation(_uri_target);
	}

	if (!_lock || !_lock->findMethod(_method))
		_error_code = 405;
	while (pos < request.length()) {
		lineEnd = request.find('\n', pos);
		if (lineEnd == std::string::npos) {
			lineEnd = request.length();
		}
		
		std::string line = request.substr(pos, lineEnd - pos);
		line = strtrim(line, " \r\t\n");
		
		if (line.empty()) {
			pos = lineEnd + 1;
			break;
		}
		
		if (line.find(':') != std::string::npos) {
			std::string name(line.substr(0, line.find(':')));
			std::string	content(line.substr(line.find(':')));
			std::transform(name.begin(), name.end(), name.begin(), ::tolower);
			_headers[name] = strtrim(content, ": \t");
		}
		
		pos = lineEnd + 1;
	}

	if (_headers.find("content-length") != _headers.end()) {
		size_t contentLength(0);
		std::stringstream num_ss(_headers["content-length"]);
		num_ss >> contentLength;

		if (pos < request.length() && contentLength > 0) {
			size_t availableContent = request.length() - pos;
			size_t actualLength = std::min(contentLength, availableContent);
			_content = request.substr(pos, actualLength);
			if (_lock && !_lock->checkMaxBody(_content.size())) {
				_error_code = 413;
			}
		}
	}
	if (_headers.find("cookies") != _headers.end()) 
	{
		;
	}

	if (_uri_target.length() > 1024) 
	{
		_error_code = 414;
	} else {
		if (!_lock)
		{
			_error_code = 404;
			std::cerr << "Wrong target: [" << _uri_target << "] not found" << std::endl;
		} else 
		{
			_final_path = _lock->_root_path + _uri_target;
	
		}
		if (_lock && !_lock->findMethod(_method)) 
		{
			_error_code = 405;
		} else if (_http_version == "http/1.0") 
		{
			_error_code = 505;
		} else if (_http_version != "http/1.1") 
		{
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
	if (_lock && _lock->_is_cgi) 
		return true;
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
std::string const & Request::getHttpVersion( void ) const { return _http_version ;}

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


const std::string Request::getCGIFile() const 
{
	size_t pos(_uri_target.find_last_of('.'));

	if (_lock && isTargetCGI() && !_lock->_cgi_map.empty() && pos != std::string::npos) {
		std::string ext(_uri_target.substr(pos + 1));
		std::string file_no_ext(_uri_target.substr(0, pos));

		std::string extention_path[] = {"py", "js", "php"};

		for (size_t i = 0; i < extention_path->size() ; i++) {
			if (ext == extention_path[i]) {
				ext = _lock->_root_path;
				break ;
			}
		}

		if (_lock->_cgi_map.find(ext) != _lock->_cgi_map.end()) 
		{
			std::multimap<std::string,std::string> :: iterator m_it = _lock->_cgi_map.find(ext);
			std :: string m_str = m_it->second;
			size_t v_pos(m_str.find_last_of('.'));
			std :: string m_str_no_ext(m_str);
			if (pos != std::string::npos)
					m_str_no_ext = m_str_no_ext.substr(0, v_pos);
			if (m_str_no_ext.find(file_no_ext) != std::string::npos 
					|| file_no_ext.find(m_str_no_ext) != std::string::npos)
					return (m_str);
			return (_lock->_cgi_map.find(ext)->second);
		}
	}
	return ("");
} 

const std::string Request::getAcceptEncoding( void ) const
{
	if (_headers.find("accept-encoding") != _headers.end()) {
		return  (_headers.find("accept-encoding")->second);
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

const std::string Request::getUserAgent( void ) const 
{
	if (_headers.find("user-agent") != _headers.end()) {
		return (_headers.find("user-agent")->second);
	} else {
		return ("");
	}
}

const std::string Request::getCookies( void ) const 
{
	if (_headers.find("cookie") != _headers.end()) {
		return (_headers.find("cookie")->second);
	} else {
		return ("");
	}
}

bool	Request::getClientId(std::string uuid) const {
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