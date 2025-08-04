/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jspitz <jspitz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 10:32:51 by jspitz            #+#    #+#             */
/*   Updated: 2025/08/04 11:59:47 by jspitz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include <iostream>

# include <time.h>
# include <sys/time.h>
# include <sys/stat.h> 

# include "config.hpp"
# include "tcpServer.hpp"

# define uint64_t unsigned long int

std::string &		strtrim(std::string & s, const char * separator = SEPARATORS);
uint64_t			timestamp_in_ms( void );
char *				get_local_time( void );
const std::string & readFileStream(std::ifstream const & s1, std::string & s2);
const std::string & readFileString(std::string const & s1, std::string & s2);
const std::string  autoindex_header(std::string const & path);
bool				isDir(const std::string & path);
bool				ifFile(const std::string & path);