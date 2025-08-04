/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jspitz <jspitz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 13:27:53 by jspitz            #+#    #+#             */
/*   Updated: 2025/08/04 14:46:33 by jspitz           ###   ########.fr       */
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