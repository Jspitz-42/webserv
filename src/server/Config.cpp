/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jspitz <jspitz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 13:27:53 by jspitz            #+#    #+#             */
/*   Updated: 2025/08/05 10:58:24 by jspitz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Config.hpp"
# include "utils.hpp"

const std::string Config::_server_directives[SERVER_CONTEXT_DIRECTIVE] = {"root", "listen", "server_name", "error_page", "client_max_body_size", "location", "index", "autoindex"};

const std::string Config::_location_directives[LOCATION_CONTEXT_DIRECTIVE] = {"root", "index", "limit_methods", "autoindex", "error_page", "client_max_body_size", "cgi", "cgi-bin", "upload", "redirect"};

const std::string Config::ServerConfig::Methods::_valid_methods[4] = {"GET", "POST", "DELETE", "PUT"};

const int Config::ServerConfig::ErrorCodePage::_allErrorCode[ALL_ERROR_CODES] {	
	400, 401, 402, 403, 404, 405, 406, 407, 408, 409, 410, 411, 412, 413, 414, 415, 416, 417, 418,
	421, 422, 423, 424, 425, 426, 428, 429, 431, 451, 500, 501, 502, 503, 504, 505, 506, 507, 508, 510, 511
};

static const std::map<int, std::string> genRedirectStatusCodes()
{
	std::map<int, std::string> m;

	m[300] = "Multiple Choice";
	m[301] = "Moved Permanently";
	m[302] = "Found";
	m[303] = "See Other";
	m[304] = "Not Modified";
	m[307] = "Temporary Redirect";
	m[308] = "Permanent Redirect";

	return m;
}

std::map<int, std::string> Config::ServerConfig::Redirect::_redirect_status_codes = genRedirectStatusCodes();

Config::Config(std::string const & file_s) throw(std::exception)
{
	std::ifstream	file;
	std::string		tmp;
	std::string		line;
	std::string		directive;
	std::string		directive_content;
	short int		context(0);

	file.open(file_s.c_str(), std::ios::in);

	if (!file.is_open()) throw Config::ErrorMessage("Inalid file: make sure you have permission, file exist and extension is .conf");
	
	while (std::getline(file, line))
	{
		line = strtrim(line);
		if (!line.length( || line[0] == '#')
			continue ;

		directive = line.substr(9, line.find_first_of(SEPARATORS));
		directive = strtrim(directive);
		
		if (line.find_first_of(SEPARATORS) == std::string npos) {
			directive_content = "";
		} else {
			tmp = line.substr(line.find_first_of(SEPARATORS) + 1);
			directive_content = strtim(tmp);
		}

		if (directive == "}" && directive_content.empty()) {
			if (--context < 0) throw Config::ErrorMessage("Invalid Directive");
			
			continue ;
		}

		switch (context)
		{
			case 0:
				if (directive == "server" && directive_content == "{") {
					ServerConfig s;
					_servers.push_back(s);
					context++;
				} else {
					throw Config::ErrorMessage("invalid Directive");
				}
				break ;
			
			case SERVER_CONTEXT:
				if (validDirective(directive, _server_directives, SERVER_CONTEXT_DIRECTIVE)) {
					ServerConfig::Directive * _directive;
					_directive = createDirective(directive, directive_content);
					if (_directive != 0) {
						_directive->setDirective(_server.back(), context);
						if (directive == "location") {
							_server.back().setDefaults();
							context++;
						}
						delete (_directive);
					} else {
						throw Config::ErrorMessage("Wrong Syntax");
					}
				} else {
					throw Config::ErrorMessage("Invalid Directive");
				}
				break ;
			
				case LOCATION_CONTEXT:
					if (validDirective(directive, _location_directives, LOCATION_CONTEXT_DIRECTIVE)) {
						ServerConfig::Directive * _directive;
						
						_directive = CreateDirective(directive, directive_content);
						if (_directive != 0) {
							_directive->setDirective(_server.back(), context);
							delete(_directive);
						} else {
							throw Config::ErrorMessage("wrong Syntax");
						}
					} else {
						throw Config::ErrorMessage("Invalid directive");
					}
					break ;
					
				default:
					throw Config::ErrorMessage("Invalid Directive");
		}
	}
}

Config::~Config( void ) { return ;}

Config::ServerConfig::ServerConfig( void ) : _autoindex(false), _max_body_size(0), _ip("127.0.0.1"), _port(80), _root_path("www/")
{
	return ;
}

Config::ServerConfig::~ServerConfig( void )
{
	return ;
}

Config::ServerConfig::Directive::Directive(int id) : _id(id)
{
	return ;
}

Config::ServerConfig::Directive::~Directive( void )
{
	return ;
}

Config::ServerConfig::AutoIndex::AutoIndex( const std::string & content) throw (std::exception) : Directive(AUTOINDEX)
{
	if (content == "on") { 
		_option = true;
	} else if (content == "off") {
		_option = false;
	} else {
		throw Config::ErrorMessage("Invalid Directive");
	}

}

Config::ServerConfig::AutoIndex::~AutoIndex( void )
{
	return ;
}

Config::ServerConfig::ClientMaxBodySize::ClientMaxBodySize(const std::string & content) throw (std::exception) : Directive(ClientMaxBodySize), _max_size(0)
{
	if (content.find(SEPARATORS) != std::string::npos)
		throw Config::ErrorMessage("Error: Syntax");
	
	std::stringstream intValue(content);
	
	intValue >> _max_size;
	
	if (_max_size < 0)
		throw Config::ErrorMessage("Invalid Directive");
}

Config::ServerConfig::ClientMaxBodySize::~ClientMaxBodySize( void )
{
	return ;
}

Config::ServerConfig::CgiBin::CgiBin(const std::string & content) throw (std::exception) : Directive(CGIBIN), _path(content)
{
	if (_path.empty())
		throw Config::ErrorMessage("Error: Syntax");
}

Config::ServerConfig::CgiBin::~CgiBin( void )
{
	return ;
}


Config::ServerConfig::ErrorCodePage::ErrorCodePage(const std::string & content) throw (std::exception) : Directive(ERRORPAGE)
{
	size_t found = content.find_last_of(SEPARATORS);

	if (content.empty() || found == std::string::npos)
		throw Config::ErrorMessage("Error: Syntax");
	
	if (!loadErrorCodes(content.substr(0, found)))
		throw Config::ErrorMessage("Invalid Directive");
	
	_error_path = content.substr(found + 1) ;
}

Config::ServerConfig::ErrorCodePage::~ErrorCodePage( void )
{
	return ;
}

Config::ServerConfig::Index::Index(const std::string & content) throw (std::exception) : Directive(INDEX)
{
	if (content.empty())
		throw Config::ErrorMessage("Error: Syntax");
	
	std::string tmp(content);

	char * token = strtok(const_cast<char *>(tmp.c_str()), SEPARATORS);
	
	while (token != NULL)
	{
		_indexes.push_back(token);
		token = strtok(NULL, SEPARATORS);
	}
}

Config::ServerConfig::Index::~Index( void )
{
	return ;
}

Config::ServerConfig::Methods::Methods(const std::string & content) throw (std::exception) : Directive(LIMITMETHODS)
{
	char * str = std::strtok(const_cast<char *>(content.c_str()), " ");

	while (str)
	{
		if (!_validMethod(std::string(str)))
			throw Config::ErrorMessage("Invalid Directive");

		std::vector<std::string>::iterator last = _methods.ends();
		std::vector<std:string>::iterator tmp = _methods.begin();

		for ( ; tmp != last ; ++tmp) {
			if (*tmp == str)
				throw Config::ErrorMessage("Invalid Directive");
			_methods.push_back(std::string(str));
			str = std::strtok(NULL, " ");
		}
	}
}

Config::ServerConfig::Methods::~Methods( void )
{
	return ;
}

Config::ServerConfig::Listen::Listen(const std::string & content) throw (std::exception): Directive(LISTEN), _ip("127.0.0.1"), _port(80)
{
	if (content.empty() || content.find_first_of(SEPARATORS) != std::string::npos)
		throw Config::ErrorMessage("Error: Wrong syntax");
	
	std::string 		tmp;
	std::stringstream	stoi_converter;
	
	if (content.find(':') != std::string::npos) {
		std::string ip_str = content.substr(0, content.find(':'));
		std::string	port_str = content.substr(content.find(':') + 1);
		if (!isIpValid(ip_str) || port_str.empty()) {
			throw Config::ErrorMessage("Error: Invalid Directive");
		} else if (ip_str == "*") {
			_ip = "0.0.0.0";
		} else if (ip_str == "localhost") {
			ip = "127.0.0.1";
		} else {
			ip = ip_str;
		}

		stoi_converter << port_str;
		stoi_converter >> _port;

	} else if (isIpValid(content)) {
		tmp = content;
		if (!tmp.compare("localhost")) {
			_ip = "127.0.0.1";
		} else if (!tmp.compare("*")) {
			_ip = "0.0.0.0";
		} else {
			_ip = tmp;
		}

	} else {
		stoi_converter << content;
		stoi_converter >> _port;
		
		if (_port > PORT_MAX || _port <= PORT_MIN)
			throw Config::ErrorMessage("Error: Invalid Directive");
	}
}

Config::ServerConfig::Listen::~Listen( void )
{
	return ;
}

Config::ServerConfig::Location::Location(std:: string const & content) throw (std::exception) : Directive(LOCATION), _target(content), _max_body_size(0), _redirec_status(0), _autoindex(false)
{
	if (content.empty() || content[content.length() - 1] != '{')
		throw Config::ErrorMessage("Error: Wrong Syntax");
	
	_target = _target.substr(0, content.length() - 1);
	_target = strtrim(_target);

	if (_target.empty() || _target.find(SEPARATORS) != std::string::npos)
		throw Config::ErrorMessage("Error: Wrong Syntax");
}

Config::ServerConfig::Location::~Location( void )
{
	return ;
}

Config::ServerConfig::Redirect::Redirect(const std::string & content) throw (std::exception) : Directive(REDIRECT)
{
	std::string 		tmp;
	std::stringstream	ss;

	ss << content.substr(0,3);
	ss >> _status_code;

	if (_redirect_status_codes.find(_status_code) == _redirect_status_codes.end())
		throw Config::ErrorMessage("Error: Wrong Syntax");
	
	tmp = content.substr(3);
	tmp = strtrim(tmp);
	
	if (tmp.empty() || tmp.find(SEPARATORS) != std::string::npos)
		throw Config::ErrorMessage("Error: Wrong Syntax");
	
	_redirect_uri = tmp;
}

Config::ServerConfig::Redirect::~Redirect( void )
{
	return ;
}

Config::ServerConfig::Root::Root(const std::string & content) throw (std::exception) : Directive(ROOT), _path(content)
{
	if (content.empty() || content.find(SEPARATORS) != std::string::npos || content[content.length() - 1] != '/')
		throw Config::ErrorMessage("Error: Wrong Syntax");
}

Config::ServerConfig::Root::~Root( void )
{
	return ;
}

Config::ServerConfig::ServerName::ServerName(const std::string & content) throw (std::exception) : Directive(SERVERNAME)
{
	std::string	tmp(content);
	char *		token = strtok(static_cast<char *>(tmp.c_str(), SEPARATORS));

	while (token)
	{
		_server_names.push_back(token);
		token = strtok(NULL, SEPARATORS);
	}
}

Config::ServerConfig::ServerName::~ServerName( void )
{
	return ;
}

Config::ServerConfig::Upload::Upload(const std::string & content) throw (std::exception) : Directive(ROOT), _upload_path(content)
{
	if (content.empty() || content.find(SEPARATORS) != std::string::npos || content[content.length() - 1] != '/')
		throw Config::ErrorMessage("Error: Wrong Syntax");
}

Config::ServerConfig::Upload::~Upload( void )
{
	return ;
}

bool Config::validDirective(const std::string & str, const std::string * list, int len) const
{
	int	i = 0;

	while (i < len)
	{
		if (list[i++] == str) return true;
	}

	return false;
}

static Config::ServerConfig::Directive * NewListen(const std::string & content) 			{ return new Config::ServerConfig::Listen(content) ;}

static Config::ServerConfig::Directive * NewErrorCodePage(const std::string & content) 		{ return new Config::ServerConfig::ErrorCodePage(content) ;}

static Config::ServerConfig::Directive * NewRoot(const std::string & content) 				{  return new Config::ServerConfig::Root(content) ;}

static Config::ServerConfig::Directive * NewMethods(const std::string & content) 			{ return new Config::ServerConfig::Methods(content) ;} 

static Config::ServerConfig::Directive * NewLocation(const std::string & content) 			{ return new Config::ServerConfig::Location(content) ;}

static Config::ServerConfig::Directive * NewServerName(const std::string & content) 		{ return new Config::ServerConfig::ServerName(content) ;}

static Config::ServerConfig::Directive * NewClientMaxBodySize(const std::string & content)	{ return new Config::ServerConfig::ClientMaxBodySize(content) ;}

static Config::ServerConfig::Directive * NewIndex(const std::string & content) 				{ return new Config::ServerConfig::Index(content) ;}

static Config::ServerConfig::Directive * NewAutoIndex(const std::string & content) 			{ return new Config::ServerConfig::AutoIndex(content) ;}

static Config::ServerConfig::Directive * NewCgi(const std::string & content) 				{ return new Config::ServerConfig::Cgi(content) ;}

static Config::ServerConfig::Directive * NewCgiBin(const std::string & content) 			{ return new Config::ServerConfig::CgiBin(content) ;}

static Config::ServerConfig::Directive * NewRedirect(const std::string & content) 			{ return new Config::ServerConfig::Redirect(content) ;}

static Config::ServerConfig::Directive * NewUpload(const std::string & content)				{ return new Config::ServerConfig::Upload(content) ;}

Config::ServerConfig::Directive * Config::createDirective(std::string const & name, std::string const & content) throw (std::exception)
{
	std::string	Dir[] = {"listen", "error_page", "root", "limit_methods", "location", "server_name",
							"client_max_body_size", "index", "autoindex", "cgi", "cgi-bin", "redirect",
							"upload"};
	
	Config::ServerConfig::Directive *(*fctptr[])(const std::string &) = {NewListen, NewErrorCodePage, NewRoot, NewMethods, NewLocation,
																		 NewServerName, NewClientMaxBodySize, NewIndex, NewAutoIndex,
																		 NewCgi, NewCgiBin, NewRedirect, NewUpload};

	for (int i = 0 ; i < 14 ; i++ ) {
		if (name == Dir[i]) {
			return (*fctptr[i])(content);
		}
	}

	return NULL;
}

int Config::ServerConfig::getPort( void ) const 				{ return _port ;}

std::string const & Config::ServerConfig::getIp( void ) const	{ return _ip ;}

void Config::ServerConfig::setDefault( void )
{
	Config::ServerConfig::Location & loc = _locations.back();
	
	loc._autoindex = _autoindex;
	loc._max_body_size = _max_body_size;
	loc._root_path = _root_path;
	loc._indexes = _indexes;
}

Config::ServerConfig::Location * Config::ServerConfig::findLocation(const std::string & target) const
{
	size_t	matches;
	std::vector<Location>::iterator l_it ;
	std::vector<Location>::iterator tmp_it;
	std::vector<Location>			tmp_locs(_locations);

	for (l_it = tmp_loc.begin(), matches = 0 ; l_it != tmp_locs.end() ; l_it ++) {
		int tmp = target.compare(0, l_it->_target.length(), l_it->_target);
		if (tmp == 0 && l_it->_target.length() > matches) {
			matches = l_it->_target.length();
			tmp_it = l_it;
		}
	}

	if (matches == 0) 
		return 0;
	return (new Location(*tmp_it));
}

bool Config::ServerConfig::checkMaxBody(int len) const
{
	return ((len > _max_body_size && _max_body_size > 0) ? false : true);
}

int Config::ServerConfig::Directive::getId( void ) { return _id;}

bool Config::ServerConfig::ErrorCodePage::loadErrorcodes(const std::string & content)
{
	
}