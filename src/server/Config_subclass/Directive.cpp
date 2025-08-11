/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Directive.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jspitz <jspitz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 09:08:04 by jspitz            #+#    #+#             */
/*   Updated: 2025/08/11 10:21:20 by jspitz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

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
static Config::ServerConfig::Directive * NewDirectoryListing(const std::string & content)	{ return new Config::ServerConfig::DirectoryListing(content) ;}

Config::ServerConfig::Directive * Config::createDirective(std::string const & name, std::string const & content) throw (std::exception)
{
	std::string	Dir[] = {"listen", "error_page", "root", "allow_methods", "location", "server_name",
							"client_max_body_size", "index", "autoindex", "cgi", "cgi-bin", "redirect",
							"upload", "directory_listing"};

	Config::ServerConfig::Directive *(*fctptr[])(const std::string &) = {NewListen, NewErrorCodePage, NewRoot, NewMethods, NewLocation,
																		 NewServerName, NewClientMaxBodySize, NewIndex, NewAutoIndex,
																		 NewCgi, NewCgiBin, NewRedirect, NewUpload, NewDirectoryListing};
	for (int i = 0 ; i < 15 ; i++ ) {
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
