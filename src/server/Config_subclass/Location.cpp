/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlonghin <tlonghin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 09:15:49 by jspitz            #+#    #+#             */
/*   Updated: 2025/08/20 02:41:34 by tlonghin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include "utils.hpp"

static bool is_cgi(std::string const & content)
{
	size_t found_py = content.find(".py");
	size_t found_js = content.find(".js");
	
	if ((found_py != std::string::npos || found_js != std::string::npos)) {
		return true ;
	} else {
		return false;
	}

}

static bool file_exist_and_exec(std::string const & file)
{
	return (access(file.c_str(), F_OK | R_OK) == 0) ? true : false;
}

Config::ServerConfig::Location::Location(std:: string const & content) throw (std::exception) : Directive(LOCATION),
																								_target(content),
																								_max_body_size(0),
																								_redirect_status(0),
																								_autoindex(false),
																								_listing(false),
																								_is_cgi(false)
																								
{
	if (content.empty() || content[content.length() - 1] != '{') {
		throw Config::ErrorMessage(LOCATION_WRONG_SYNTAX_1);
	}
	
	_target = _target.substr(0, content.length() - 1);
	_target = strtrim(_target);

	if (is_cgi(_target)) {
		_is_cgi = true;
	}

	struct stat s;

	if (_is_cgi && file_exist_and_exec(_target))
	{
		throw Config::ErrorMessage("Error: [Location] [File do not exist] : " + _target);	
	} else if ((stat(_target.c_str(), &s)) == 0 && !(s.st_mode & S_IFDIR)) {
		throw Config::ErrorMessage("ERROR: [Location] [Directory does not exist] : " + _target);
	}
	if (_target.empty() || _target.find(SEPARATORS) != std::string::npos) {
		throw Config::ErrorMessage(LOCATION_WRONG_SYNTAX_2);
	}
}

void Config::ServerConfig::Location::setDirective(ServerConfig & serv_conf, int context) const
{
	if (context == SERVER_CONTEXT) {
		serv_conf._locations.push_back(*this);
	}
}

bool Config::ServerConfig::Location::checkMaxBody(int len) const
{
	if (len > _max_body_size && _max_body_size > 0) return false;

	return true;
}

bool Config::ServerConfig::Location::findMethod(const std::string & method) const
{
	std::vector<std::string>::const_iterator it = _methods.begin();

    std::cerr << "DEBUG Location::findMethod this=" << this 
              << " methods size=" << _methods.size() << std::endl;
	for (; it != _methods.end() ; it++) {
		if (!it->empty() && method == *it) return true;
	}
	return false;
}

Config::ServerConfig::Location::~Location( void )
{
	return ;
}
