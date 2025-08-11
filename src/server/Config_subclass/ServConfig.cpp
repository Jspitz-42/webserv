/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServConfig.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: altheven <altheven@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 09:07:24 by jspitz            #+#    #+#             */
/*   Updated: 2025/08/11 16:16:06 by altheven         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

Config::ServerConfig::ServerConfig( void ) : _autoindex(false),_max_body_size(0), _ip("127.0.0.1"), _port(80), _root_path("www/")
{
	if (_listing != true)
		_listing = false;
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
	
	loc._listing = _listing;
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
	if (matches == 0) {
        for (std::vector<Location>::iterator it = tmp_locs.begin(); it != tmp_locs.end(); ++it) {
            if (it->_target == "/") {
                tmp_it = it;
                matches = 1;
                break;
            }
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