/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jspitz <jspitz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/29 07:38:31 by jspitz            #+#    #+#             */
/*   Updated: 2025/08/19 09:27:45 by jspitz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include <iostream>
# include <map>
# include <fstream>
# include <sstream>
# include <ostream>
# include <string>
# include <limits>
# include <algorithm>
# include <fcntl.h>
# include <cstdlib>
# include <sys/stat.h>
# include <cstdio>
# include <unistd.h>
# include <dirent.h>
# include <time.h>
# include <sys/time.h>
# include <sys/wait.h>

# include "utils.hpp"
# include "Config.hpp"

class Request;

class Response
{
	public:
		class ErrorMessage : virtual public std::exception
		{
			std::string msg;
			
			public:

			ErrorMessage(const std::string & message ) : msg(message) {}

			virtual ~ErrorMessage() throw() {}

			const char * what() const throw()
			{
				return msg.c_str();
			}	
		};

													Response(Request const &, Config::ServerConfig const &);
													~Response( void );

		const										std::string createResponse( void );

		bool										getKeepAlive( void ) const;
		int											getStatusCode( void ) const;
	
		static std::map<int, std::string>			_codeMessage;
		static std::map<std::string, std::string>	_mime_types;

	protected:

	private:
		int											_status_code;

		bool										_keep_alive;
		bool										_autoindex;
		bool										_cgi_response;

		std::string									_date;
		std::string									_server_name;
		std::string									_content_type;
		std::string									_content;

		Request	const &								_req;
		Config::ServerConfig const &				_server_config;

		int											execCGI( void );

		void										setMimeType(std::string const &);		

		const std::string 							createRedirectionResponse( void );
		const std::string 							createAutoindexResponse( void );
		const std::string 							deleteResponse( void );
		const std::string 							CGIResponse( void );
};