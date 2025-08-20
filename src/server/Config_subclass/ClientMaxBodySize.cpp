/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientMaxBodySize.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlonghin <tlonghin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 09:08:45 by jspitz            #+#    #+#             */
/*   Updated: 2025/08/20 08:54:21 by tlonghin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

bool is_digit_str(const std::string & s)
{
    for (size_t i = 0 ; i < s.size() ; i++) {
        if (!std::isdigit(s[i])) {
            return false;
        }
    }

    return true;
}

Config::ServerConfig::ClientMaxBodySize::ClientMaxBodySize(const std::string & content) throw (std::exception) : Directive(CLIENTMAXBODYSIZE), _max_size(0)
{
    if (content.find(SEPARATORS) != std::string::npos || !is_digit_str(content)) {
        throw Config::ErrorMessage(CLIENTMAXBODYSIZE_WRONG_SYNTAX);
    }
    
    std::stringstream intValue(content);

    intValue >> _max_size;

    if (_max_size < 0) {
        throw Config::ErrorMessage(CLIENTMAXBODYSIZE_INVALID_DIR);
    }
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