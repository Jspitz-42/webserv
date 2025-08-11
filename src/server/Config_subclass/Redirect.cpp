/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Redirect.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jspitz <jspitz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 09:16:17 by jspitz            #+#    #+#             */
/*   Updated: 2025/08/11 09:16:42 by jspitz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

Config::ServerConfig::Redirect::Redirect(const std::string & content) throw (std::exception) : 
																			Directive(REDIRECT)
{
	std::string 		tmp;
	std::stringstream	ss;

	ss << content.substr(0,3);
	ss >> _status_code;

	if (_redirect_status_codes.find(_status_code) == _redirect_status_codes.end()) {
		throw Config::ErrorMessage(REDIRECT_WRONG_SYNTAX_1);
	}
	
	tmp = content.substr(3);
	tmp = strtrim(tmp);
	
	if (tmp.empty() || tmp.find(SEPARATORS) != std::string::npos) {
		throw Config::ErrorMessage(REDIRECT_WRONG_SYNTAX_2);
	}
	
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
