/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DirectoryListing.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jspitz <jspitz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 10:11:53 by jspitz            #+#    #+#             */
/*   Updated: 2025/08/11 11:35:37 by jspitz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

static bool validDirList(const std::string & content)
{
	if (content == "on" || content == "off")
		return true;
	return false;
}

Config::ServerConfig::DirectoryListing::DirectoryListing(const std::string & content) throw (std::exception): Directive(LISTING)
{
	char *str = std::strtok(const_cast<char *>(content.c_str()), " ");

	if (!validDirList(str)) {
		throw Config::ErrorMessage(LISTING_INVALID_DIR);
	}

	if (!std::strncmp("on", str, 2)) {
		_ListingOnOff = true ;
	} else if (!std::strncmp("off", str, 3)) {
		_ListingOnOff = false;
	}
}

Config::ServerConfig::DirectoryListing::~DirectoryListing( void )
{
	return ;
}

void Config::ServerConfig::DirectoryListing::setDirective(ServerConfig & serv_conf, int context) const
{
	if (context == LISTING) {
		serv_conf._listing = _ListingOnOff;
	}
}