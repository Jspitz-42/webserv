/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: altheven <altheven@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/29 07:38:22 by jspitz            #+#    #+#             */
/*   Updated: 2025/08/19 09:39:58 by altheven         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include <iostream>
# include <map>
# include <algorithm>
# include <SessionClient.hpp>
# include <iomanip>
# include "utils.hpp"
# include "Config.hpp"

class Socket;
class Request
{
	public:
											Request(std::string const &, Config::ServerConfig const &);
											~Request( void );

		std::string const					createClientId();
		int									getErrorCode( void ) const;
		bool								isTargetDir( void ) const;
		bool								isTargetCGI( void ) const;
		bool								isTargetRedirect( void ) const;
		
		std::string const & 				getFinalPath( void ) const;
		std::string const & 				getContent( void ) const ;
		std::string const & 				getQuery( void ) const ;
		std::string const & 				getMethod( void ) const ;
		std::string const & 				getUriTarget( void ) const;
		std::string const & 				getHttpVersion( void ) const;
		
		bool								getClientId(std::string uuid) const;
		const std::string 					getContentType( void ) const;
		const std::string 					getIndex( void ) const;
		const std::string 					getCGIBindPath( void ) const;
		const std::string 					getCGIFile( void ) const;
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
		SessionClient						_clientList[2048];
		Config::ServerConfig const & 		_server_config;
		
//		bool								_check_default_error_code_and_assign_path( int ) const;
};