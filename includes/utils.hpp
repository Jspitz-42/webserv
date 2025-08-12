/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jspitz <jspitz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 10:32:51 by jspitz            #+#    #+#             */
/*   Updated: 2025/08/12 12:14:50 by jspitz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include <iostream>

# include <time.h>
# include <sys/time.h>
# include <sys/stat.h> 

# define SEPARATORS					" \t\v\n\r\f;"


class Config;
class TcpServer;

# define UINT64_T unsigned long int

std::string &		strtrim(std::string & s, const char * separator = SEPARATORS);
UINT64_T			timestamp_in_ms( void );
char *				get_local_time( void );
const std::string & readFileStream(std::ifstream const & s1, std::string & s2);
const std::string & readFileString(std::string const & s1, std::string & s2);
const std::string 	autoindex_header(std::string const & path);
bool				isDir(const std::string & path);
bool				ifFile(const std::string & path);