/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jspitz <jspitz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/29 07:38:31 by jspitz            #+#    #+#             */
/*   Updated: 2025/08/06 11:45:50 by jspitz           ###   ########.fr       */
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
# include <time>
# include <sys/time.h>
# include <sys/wait.h>

# include "Request.hpp"
# include "utils.hpp"

class Response
{
	public:
		class ErrorMessage : virtual public std::exception
		{
			std::string 													msg;
			
			public:

			ErrorMessage(const std::string & message ) : msg(message) {}

			virtual ~ErrorMessage() throw() {}

			const char * what() const throw()
			{
				return msg.c_str();
			}	
		};

	protected:

	private:

};