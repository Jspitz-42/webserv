/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jspitz <jspitz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 13:27:53 by jspitz            #+#    #+#             */
/*   Updated: 2025/08/06 10:49:17 by jspitz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Config.hpp"
# include "utils.hpp"
Config::Config(void) { return ;}

const std::string Config::_server_directives[SERVER_CONTEXT_DIRECTIVE] = {"root", "listen", "server_name", "error_page", "client_max_body_size", "location", "index", "autoindex"};

const std::string Config::_location_directives[LOCATION_CONTEXT_DIRECTIVE] = {"root", "index", "limit_methods", "autoindex", "error_page", "client_max_body_size", "cgi", "cgi-bin", "upload", "redirect"};

const std::string Config::ServerConfig::Methods::_valid_methods[4] = {"GET", "POST", "DELETE", "PUT"};

const int Config::ServerConfig::ErrorCodePage::_allErrorCodes[ALL_ERROR_CODES] = {	
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

// -----------------------------this part is for the Config class only --------------------

bool Config::validDirective(const std::string & str, const std::string * list, int len) const
{
	int	i = 0;

	while (i < len)
	{
		if (list[i++] == str) return true;
	}

	return false;
}

Config::Config(std::string const & file_s) throw(std::exception)
{
	std::ifstream	file;
	std::string		tmp;
	std::string		line;
	std::string		directive;
	std::string		directive_content;
	short int		context(0);

	file.open(file_s.c_str(), std::ios::in);

	if (!file.is_open()) throw Config::ErrorMessage("Invalid file: make sure you have permission, file exist and extension is .conf");
	
	while (std::getline(file, line))
	{
		line = strtrim(line);
		if (!line.length() || line[0] == '#')
			continue ;

		directive = line.substr(0, line.find_first_of(SEPARATORS));
		directive = strtrim(directive);
		
		if (line.find_first_of(SEPARATORS) == std::string::npos) {
			directive_content = "";
		} else {
			tmp = line.substr(line.find_first_of(SEPARATORS) + 1);
			directive_content = strtrim(tmp);
		}

		if (directive == "}" && directive_content.empty()) {
			if (--context < 0) throw Config::ErrorMessage("ERROR: [Config::Config] : Invalid Directive");
			
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
					throw Config::ErrorMessage("ERROR: [Config::COnfig: switch] [case 0] : invalid Directive");
				}
				break ;
			
			case SERVER_CONTEXT:
				if (validDirective(directive, _server_directives, SERVER_CONTEXT_DIRECTIVE)) {
					ServerConfig::Directive * _directive;
					_directive = createDirective(directive, directive_content);
					if (_directive != 0) {
						_directive->setDirective(_servers.back(), context);
						if (directive == "location") {
							_servers.back().setDefault();
							context++;
						}
						delete (_directive);
					} else {
						throw Config::ErrorMessage("ERROR: [Config::Config] [switch :case SERVER_CONTEXT] : Wrong Syntax");
					}
				} else {
					throw Config::ErrorMessage("ERROR: [Config::Config] [switch: case SERVER_CONTEXT] : Invalid Directive");
				}
				break ;
			
				case LOCATION_CONTEXT:
					if (validDirective(directive, _location_directives, LOCATION_CONTEXT_DIRECTIVE)) {
						ServerConfig::Directive * _directive;
						
						_directive = createDirective(directive, directive_content);
						if (_directive != 0) {
							_directive->setDirective(_servers.back(), context);
							delete(_directive);
						} else {
							throw Config::ErrorMessage("ERROR: [Config::Config] [switch : case LOCATION_CONTEXT] : wrong Syntax");
						}
					} else {
						throw Config::ErrorMessage("ERROR: [Config::Config] [switch: case LOCATION_CONTEXT] : Invalid directive");
					}
					break ;
					
				default:
					throw Config::ErrorMessage("ERROR: [Config::Config] [switch: case default] : Invalid Directive");
		}
	}
}

Config::~Config( void ) { return ;}
//----------------------------- end of Config::Config ----------------------------------------------


//------------------------------- ServerConfig only ------------------------------------------------
Config::ServerConfig::ServerConfig( void ) : _autoindex(false), _max_body_size(0), _ip("127.0.0.1"), _port(80), _root_path("www/")
{
	return ;
}

bool Config::ServerConfig::checkMaxBody(int len) const
{
	return ((len > _max_body_size && _max_body_size > 0) ? false : true);
}

int Config::ServerConfig::getPort( void ) const 
{
	return _port ;
}

std::string const & Config::ServerConfig::getIp( void ) const
{
	return _ip ;
}

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

	for (l_it = tmp_locs.begin(), matches = 0 ; l_it != tmp_locs.end() ; l_it ++) {
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

Config::ServerConfig::~ServerConfig( void )
{
	return ;
}

//--------------------------------end of ServerConfig only --------------------------------------------


//--------------------------------Directive only ------------------------------------------------------

Config::ServerConfig::Directive::Directive(int id) : _id(id)
{
	return ;
}

static Config::ServerConfig::Directive * NewListen(const std::string & content) 			{ return new Config::ServerConfig::Listen(content) ;}
static Config::ServerConfig::Directive * NewErrorCodePage(const std::string & content) 		{ return new Config::ServerConfig::ErrorCodePage(content) ;}
static Config::ServerConfig::Directive * NewRoot(const std::string & content) 				{ return new Config::ServerConfig::Root(content) ;}
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

int Config::ServerConfig::Directive::getId( void ) const
{
	return _id;
}

Config::ServerConfig::Directive::~Directive( void )
{
	return ;
}

//----------------------- end of Directive -----------------------------------------------------------


//------------------------- AutoIndex only -----------------------------------------------------------

Config::ServerConfig::AutoIndex::AutoIndex( const std::string & content) throw (std::exception) : Directive(AUTOINDEX)
{
	if (content == "on") { 
		_option = true;
	} else if (content == "off") {
		_option = false;
	} else {
		throw Config::ErrorMessage("ERROR: [Config::ServerConfig::AutoIndex::AutoIndex] : Invalid Directive");
	}

}

void Config::ServerConfig::AutoIndex::setDirective(ServerConfig & serv_conf, int context) const
{
	if (context == SERVER_CONTEXT) {
		serv_conf._autoindex = _option;
	} else if (context == LOCATION_CONTEXT) {
		serv_conf._locations.back()._autoindex = _option;
	}
}

Config::ServerConfig::AutoIndex::~AutoIndex( void )
{
	return ;
}

//-------------------------end of AutoIndex -------------------------------------------------------------


//-------------------------clientMaxBodySize only --------------------------------------------------------

Config::ServerConfig::ClientMaxBodySize::ClientMaxBodySize(const std::string & content) throw (std::exception) : Directive(CLIENTMAXBODYSIZE), _max_size(0)
{
	if (content.find(SEPARATORS) != std::string::npos)
		throw Config::ErrorMessage("ERROR: [Config::ServerConfig::ClientMaxBodySize::ClientMaxBodySize] : Wrong Syntax");
	
	std::stringstream intValue(content);
	
	intValue >> _max_size;
	
	if (_max_size < 0)
		throw Config::ErrorMessage("ERROR: [Config::ServerConfig::ClientMaxBodySize::ClientMaxBodySize]: Invalid Directive");
}

void Config::ServerConfig::ClientMaxBodySize::setDirective(ServerConfig & serv_conf, int context) const
{
	if (context == SERVER_CONTEXT) {
		serv_conf._max_body_size = _max_size;
	} else if (context == LOCATION_CONTEXT) {
		serv_conf._locations.back()._max_body_size = _max_size;
	}
}

Config::ServerConfig::ClientMaxBodySize::~ClientMaxBodySize( void )
{
	return ;
}

//------------------------end of clientMaxBodySize -----------------------------------------------------------


//------------------------ Cgi only -------------------------------------------------------------------------
Config::ServerConfig::Cgi::Cgi(const std::string & content) throw (std::exception) : Directive(CGI)
{
	if (content.empty())
		throw Config::ErrorMessage("ERROR: [Config::ServerConfig::Cgi::Cgi] : Wrong Syntax");

	_parseCgiContent(_cgi, content);
}

void Config::ServerConfig::Cgi::setDirective(ServerConfig & serv_conf, int context) const
{
	if (context == LOCATION_CONTEXT) {
		serv_conf._locations.back()._cgi_map[_cgi[0]].push_back(_cgi[1]);
	}
}

void Config::ServerConfig::Cgi::_parseCgiContent(std::vector<std::string> & _target, const std::string & content)
{
	char * token = strtok(const_cast<char*>(content.data()), " ");

	while (token)
	{
		_target.push_back(std::string(token));
		token = strtok(NULL, SEPARATORS);
	}

	if (_target.size() != 2) 
		throw Config::ErrorMessage("ERROR: [Config::ServerConfig::Cgi::_parseCgiContent] : Invalid Directive");
}

Config::ServerConfig::Cgi::~Cgi( void ) { return ;}

//------------------------ end of Cgi -----------------------------------------------------------------------


// ----------------------- CgiBin only ----------------------------------------------------------------------

Config::ServerConfig::CgiBin::CgiBin(const std::string & content) throw (std::exception) : Directive(CGIBIN), _path(content)
{
	if (_path.empty())
		throw Config::ErrorMessage("ERROR: [Config::ServerConfig::CgiBin::CgiBin] : Syntax");
}

void Config::ServerConfig::CgiBin::setDirective(ServerConfig & serv_conf, int context) const
{
	if (context == LOCATION_CONTEXT) {
		serv_conf._locations.back()._cgi_bin = _path;
	}
}

Config::ServerConfig::CgiBin::~CgiBin( void )
{
	return ;
}

//------------------------end of CgiBin ----------------------------------------------------------------------


//------------------------ErrorCodePage only ------------------------------------------------------------

Config::ServerConfig::ErrorCodePage::ErrorCodePage(const std::string & content) throw (std::exception) : Directive(ERRORPAGE)
{
	size_t found = content.find_last_of(SEPARATORS);

	if (content.empty() || found == std::string::npos)
		throw Config::ErrorMessage("ERROR: [Config::ServerConfig::ErrorCodePage::ErrorCodePage] : Wrong Syntax");
	
	if (!loadErrorCodes(content.substr(0, found)))
		throw Config::ErrorMessage("ERROR: [Config::ServerConfig::ErrorCodePage::ErrorCodePage] : Invalid Directive");
	
	_error_path = content.substr(found + 1) ;
}

void Config::ServerConfig::ErrorCodePage::setDirective(ServerConfig & serv_conf, int context) const
{
	if (context == SERVER_CONTEXT) {
		serv_conf._server_error_maps[_error_path] = _error_codes;
	} else if (context == LOCATION_CONTEXT) {

		std::vector<int>::const_iterator it;

		for (it = _error_codes.begin(); it != _error_codes.end() ; ++it)
			serv_conf._locations.back()._location_errors_map[_error_path].push_back(*it);
	}
}

bool Config::ServerConfig::ErrorCodePage::loadErrorCodes(const std::string & content)
{
	std::stringstream	stoi_converter;
	int					converted_number;
	int					loop_counter = 0;

	char *token = strtok(const_cast<char*>(content.data()), SEPARATORS);

	while (token != 0)
	{
		loop_counter++;
		stoi_converter.clear();
		stoi_converter << token;
		stoi_converter >> converted_number;
		
		for (int i = 0; i < 40 ; i++) {
			if (converted_number == _allErrorCodes[i]) {
				_error_codes.push_back(converted_number);
				loop_counter--;
			}
		}
		token = strtok(NULL, SEPARATORS);
	}
	return (!loop_counter) ? true : false;
}

Config::ServerConfig::ErrorCodePage::~ErrorCodePage( void )
{
	return ;
}

//-----------------------end of ErrorCodePage --------------------------------------------------------------------

//-----------------------Index only -------------------------------------------------------------------------

Config::ServerConfig::Index::Index(const std::string & content) throw (std::exception) : Directive(INDEX)
{
	if (content.empty())
		throw Config::ErrorMessage("ERROR: [Config::ServerConfig::Index::Index] : Syntax");
	
	std::string tmp(content);

	char * token = strtok(const_cast<char*>(tmp.data()), SEPARATORS);
	
	while (token != NULL)
	{
		_indexes.push_back(token);
		token = strtok(NULL, SEPARATORS);
	}
}

void Config::ServerConfig::Index::setDirective(ServerConfig & serv_conf, int context) const
{
	if (context == SERVER_CONTEXT) {
		serv_conf._indexes = _indexes;
	} else if (context == LOCATION_CONTEXT) {
		serv_conf._locations.back()._indexes =_indexes;
	}
}


Config::ServerConfig::Index::~Index( void )
{
	return ;
}

//------------------------end of Index----------------------------------------------------------------------

//------------------------Methods only --------------------------------------------------------------------

Config::ServerConfig::Methods::Methods(const std::string & content) throw (std::exception) : Directive(LIMITMETHODS)
{
	char * str = std::strtok(const_cast<char*>(content.c_str()), " ");

	while (str)
	{
		if (!_validMethod(std::string(str)))
			throw Config::ErrorMessage("ERRROR: [Config::ServerConfig::Methods::Methods] [_validMethod false ] : Invalid Directive");

		std::vector<std::string>::iterator last = _methods.end();
		std::vector<std::string>::iterator tmp = _methods.begin();

		for ( ; tmp != last ; ++tmp) {
			if (*tmp == str)
				throw Config::ErrorMessage("ERROR: [Config::ServerConfig::Methods::Methods] [double Methods]: Invalid Directive");
			_methods.push_back(std::string(str));
			str = std::strtok(NULL, " ");
		}
	}
}

void  Config::ServerConfig::Methods::setDirective(ServerConfig & serv_conf, int context) const
{
	if (context == LOCATION_CONTEXT) {
		serv_conf._locations.back()._methods = _methods;
	}
}

bool Config::ServerConfig::Methods::_validMethod(const std::string & method)
{
	for (size_t i  = 0; i < 4 ; i++) {
		if (method == _valid_methods[i])
			return true;
	}

	return false;
}

Config::ServerConfig::Methods::~Methods( void )
{
	return ;
}

//------------------------end of Methods--------------------------------------------------------------------------

//-----------------------Listen only------------------------------------------------------------------------------

Config::ServerConfig::Listen::Listen(const std::string & content) throw (std::exception): Directive(LISTEN), _ip("127.0.0.1"), _port(80)
{
	if (content.empty() || content.find_first_of(SEPARATORS) != std::string::npos)
		throw Config::ErrorMessage("ERROR: [Config::ServerConfig::Listen::Listen] : Wrong syntax");
	
	std::string 		tmp;
	std::stringstream	stoi_converter;
	
	if (content.find(':') != std::string::npos) {
		std::string ip_str = content.substr(0, content.find(':'));
		std::string	port_str = content.substr(content.find(':') + 1);
		if (!isIpValid(ip_str) || port_str.empty()) {
			throw Config::ErrorMessage("ERROR: [Config::ServerConfig::Listen::Listen] [ip invalid or port invalid]: Invalid Directive");
		} else if (ip_str == "*") {
			_ip = "0.0.0.0";
		} else if (ip_str == "localhost") {
			_ip = "127.0.0.1";
		} else {
			_ip = ip_str;
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
			throw Config::ErrorMessage("ERROR: [Config::ServerConfig::Listen::Listen] [Port out of range] : Invalid Directive");
	}
}

void Config::ServerConfig::Listen::setDirective(ServerConfig & serv_conf, int context) const
{
	if (context != SERVER_CONTEXT) return ;

	serv_conf._port = _port;
	serv_conf._ip = _ip;
}


bool Config::ServerConfig::Listen::isIpValid(const std::string & ip)
{
	int counter = 0;

	if (!ip.compare("0.0.0.0") || !ip.compare("localhost") || !ip.compare("*"))
		return true;
	
	for (u_int16_t i = 0 ; i <= ip.length() ; i++) {
		if (ip[i] == '.') counter++ ;
	}

	if (counter != 3) return false;

	int	 part1;
	char ch;
	std::stringstream stoi_converter(ip);

	stoi_converter >> part1 >> ch;

	if (part1 < 1 || part1 > 255 || ch != '.')
		return false;
	
	int rest;

	while (stoi_converter.rdbuf()->in_avail() != 0)
	{
		stoi_converter >> rest >> ch;
		if (rest < 0 || rest > 255 || ch != '.') 
			return false;
	}

	return true;
}

Config::ServerConfig::Listen::~Listen( void )
{
	return ;
}

// -----------------------end of Listen----------------------------------------------------------------------------------

//------------------------Location only----------------------------------------------------------------------------------

Config::ServerConfig::Location::Location(std:: string const & content) throw (std::exception) : Directive(LOCATION),
																								_target(content),
																								_max_body_size(0),
																								_redirect_status(0),
																								_autoindex(false)
{
	if (content.empty() || content[content.length() - 1] != '{')
		throw Config::ErrorMessage("ERROR: [Config::ServerConfig::Location::Location] [content] : Wrong Syntax");
	
	_target = _target.substr(0, content.length() - 1);
	_target = strtrim(_target);

	if (_target.empty() || _target.find(SEPARATORS) != std::string::npos)
		throw Config::ErrorMessage("ERROR: [Config::ServerConfig::Location::Location] [_target] : Wrong Syntax");
}

void Config::ServerConfig::Location::setDirective(ServerConfig & serv_conf, int context) const
{
	if (context == SERVER_CONTEXT) {
		serv_conf._locations.push_back(*this);
	}
}

bool Config::ServerConfig::Location::findMethod(const std::string & method) const
{
	std::vector<std::string>::const_iterator it = _methods.begin();

	for (; it != _methods.end() ; it++) {
		if (method == *it) return true;
	}

	return false;
}

Config::ServerConfig::Location::~Location( void )
{
	return ;
}

//-----------------------end of Location---------------------------------------------------------------------------------

//-----------------------Redirect only----------------------------------------------------------------------------------

Config::ServerConfig::Redirect::Redirect(const std::string & content) throw (std::exception) : Directive(REDIRECT)
{
	std::string 		tmp;
	std::stringstream	ss;

	ss << content.substr(0,3);
	ss >> _status_code;

	if (_redirect_status_codes.find(_status_code) == _redirect_status_codes.end())
		throw Config::ErrorMessage("ERROR: [Config::ServerConfig::Redirect::Redirect] [_redirect_status_code] : Wrong Syntax");
	
	tmp = content.substr(3);
	tmp = strtrim(tmp);
	
	if (tmp.empty() || tmp.find(SEPARATORS) != std::string::npos)
		throw Config::ErrorMessage("ERROR: [Config::ServerConfig::Redirect::Redirect] [tmp.empty | unwanted Separators] : Wrong Syntax");
	
	_redirect_uri = tmp;
}

void Config::ServerConfig::Redirect::setDirective(ServerConfig & serv_conf, int context) const
{
	if (context == LOCATION_CONTEXT) {
		serv_conf._locations.back()._redirect_uri = _redirect_uri;
		serv_conf._locations.back()._redirect_status = _status_code;
	}
}

Config::ServerConfig::Redirect::~Redirect( void )
{
	return ;
}

//---------------------end of Redirect---------------------------------------------------------------------------------

//---------------------Root only--------------------------------------------------------------------------------------

Config::ServerConfig::Root::Root(const std::string & content) throw (std::exception) : Directive(ROOT), _path(content)
{
	if (content.empty() || content.find(SEPARATORS) != std::string::npos || content[content.length() - 1] != '/')
		throw Config::ErrorMessage("ERROR: [Config::ServerConfig::Root::Root] : Wrong Syntax");
}

void Config::ServerConfig::Root::setDirective(ServerConfig & serv_conf, int context) const
{
	if (context == SERVER_CONTEXT) {
		serv_conf._root_path = _path;
	} else if (context == LOCATION_CONTEXT) {
		serv_conf._locations.back()._root_path = _path;
	}
}


Config::ServerConfig::Root::~Root( void )
{
	return ;
}

//---------------------end of Root-----------------------------------------------------------------------------------

//---------------------ServerName only------------------------------------------------------------------------------

Config::ServerConfig::ServerName::ServerName(const std::string & content) throw (std::exception) : Directive(SERVERNAME)
{
	std::string	tmp(content);
	char *		token = strtok(const_cast<char*>(tmp.c_str()), SEPARATORS);

	while (token)
	{
		_server_names.push_back(token);
		token = strtok(NULL, SEPARATORS);
	}
}

void Config::ServerConfig::ServerName::setDirective(ServerConfig & serv_conf, int context) const
{
	if (context == SERVER_CONTEXT) {
		serv_conf._names = _server_names;
	}
}

Config::ServerConfig::ServerName::~ServerName( void )
{
	return ;
}

//------------------end of ServerName----------------------------------------------------------------------------------

//------------------Upload only-----------------------------------------------------------------------------------------

Config::ServerConfig::Upload::Upload(const std::string & content) throw (std::exception) : Directive(ROOT), _upload_path(content)
{
	if (content.empty() || content.find(SEPARATORS) != std::string::npos || content[content.length() - 1] != '/')
		throw Config::ErrorMessage("ERROR: [Config::ServerConfig::Upload::Upload] : Wrong Syntax");
}

void Config::ServerConfig::Upload::setDirective(ServerConfig & serv_conf, int context) const
{
	if (context == LOCATION_CONTEXT) {
		serv_conf._locations.back()._upload_path = _upload_path;
	}
}

Config::ServerConfig::Upload::~Upload( void )
{
	return ;
}

//----------------end of Upload ------------------------------------------------------------------------------------------
