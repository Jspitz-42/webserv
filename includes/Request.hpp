/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jspitz <jspitz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/29 07:38:22 by jspitz            #+#    #+#             */
/*   Updated: 2025/08/07 13:30:49 by jspitz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include <iostream>
# include <map>
# include <algorithm>

# include "Socket.hpp"
# include "utils.hpp"

class Config;
class Request
{
	public:
											Request(std::string const &, Config::ServerConfig const &);
											~Request( void );

		int									getErrorCode( void ) const;
		bool								isTargetDir( void ) const;
		bool								isTargetCGI( void ) const;
		bool								isTargetRedirect( void ) const;
		
		std::string const & 				getFinalPath( void ) const;
		std::string const & 				getContent( void ) const ;
		std::string const & 				getQuery( void ) const ;
		std::string const & 				getMethod( void ) const ;
		std::string const & 				getUriTarget( void ) const;
		
		const std::string 					getIndex( void ) const;
		const std::string 					getCGIBindPath( void ) const;
		const std::string 					getCGIFile( void ) const;
		const std::string 					getContentType( void ) const;
		const std::string 					getUserAgent( void ) const;
		const std::string 					getAcceptEncoding( void ) const;
		const std::string 					getCookies( void ) const;
		
		Config::ServerConfig::Location *	_lock;
	protected:

	private:
		int									_error_code;
		std::string							_method;
		std::string							_uri_target;
		std::string							_final_path;
		std::string							_http_version;
		std::string							_query;
		std::string							_content;
		std::map<std::string, std::string>	_headers;
		Config::ServerConfig const & 		_server_config;
		
//		bool								_check_default_error_code_and_assign_path( int ) const;
};