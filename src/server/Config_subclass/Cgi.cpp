/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jspitz <jspitz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 09:09:10 by jspitz            #+#    #+#             */
/*   Updated: 2025/08/19 12:56:14 by jspitz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Config.hpp"

Config::ServerConfig::Cgi::Cgi(const std::string & content) throw (std::exception) : Directive(CGI)
{
	if (content.empty()) {
		throw Config::ErrorMessage(CGI_WRONG_SYNTAX);
	}
	_parseCgiContent(content);
}

void Config::ServerConfig::Cgi::setDirective(ServerConfig & serv_conf, int context) const
{
	std::multimap<std::string, std::string>::const_iterator it = _all_cgi.begin();
	if (context == LOCATION_CONTEXT) {
		if (serv_conf._locations.empty()) {
			throw std::runtime_error(CGI_SETDIRECTIVE_ERR);
		}
		serv_conf._locations.back()._cgi_map.insert(std::pair<std::string,std::string>(it->first.c_str(), it->second.c_str()));
	}
}

void Config::ServerConfig::Cgi::_parseCgiContent(const std::string & content)
{
	std::istringstream iss(content);
	std::string key, value;

	if (iss >> key >> value) {
		if (key == "py" || key == "python")
			key = "/usr/bin/python3.10";
		else if (key == "js")
			key = "/usr/bin/node";
		else if (key == "php")
			key = "/usr/bin/php";
		_all_cgi.insert(std::pair<std::string, std::string>(key, value));
	}

}

Config::ServerConfig::Cgi::~Cgi( void ) { return ;}
