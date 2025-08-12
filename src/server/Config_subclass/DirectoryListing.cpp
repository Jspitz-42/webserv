/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DirectoryListing.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jspitz <jspitz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 10:11:53 by jspitz            #+#    #+#             */
/*   Updated: 2025/08/11 14:56:54 by jspitz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

Config::ServerConfig::DirectoryListing::DirectoryListing(const std::string & content) throw (std::exception): Directive(LISTING)
{
	if (content == "on") {
		_ListingOnOff = true;
	} else if (content == "off") {
		_ListingOnOff = false;
	} else {
		throw Config::ErrorMessage(LISTING_INVALID_DIR);
	}
}

Config::ServerConfig::DirectoryListing::~DirectoryListing( void )
{
	return ;
}

void Config::ServerConfig::DirectoryListing::setDirective(ServerConfig & serv_conf, int context) const
{
	if (context == SERVER_CONTEXT) {
		serv_conf._listing = _ListingOnOff;
	}
}