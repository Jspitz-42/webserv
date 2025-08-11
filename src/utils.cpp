/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jspitz <jspitz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 10:38:19 by jspitz            #+#    #+#             */
/*   Updated: 2025/08/11 09:43:10 by jspitz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "utils.hpp"
# include "Config.hpp"
# include "tcpServer.hpp"

std::string &	strtrim(std::string & s, const char * separator )
{
	s.erase(s.find_last_not_of(separator) + 1);
	s.erase(0, s.find_first_not_of(separator));

	return s;
}

static UINT64_T gettimeofday_ms( void )
{
	static struct timeval tv;

	gettimeofday(&tv, NULL);

	return ((tv.tv_sec * (UINT64_T)1000) + (tv.tv_usec / 1000));
}

UINT64_T	timestamp_in_ms( void )
{
	static UINT64_T start = 0;

	if (start == 0) {start = gettimeofday_ms() ;}

	return (gettimeofday_ms() - start);
}

char *	get_local_time( void )
{
	time_t		raw_time;
	struct tm	* time_info;

	time(&raw_time);
	time_info = localtime(&raw_time);

	return (asctime(time_info));
}

const std::string & readFileStream(std::ifstream const & s1, std::string & s2)
{
	if (s1.is_open())
	{
		std::ostringstream ss;
		ss << s1.rdbuf();
		
		s2 = ss.str();
	}

	return s2;
}

const std::string & readFileString(std::string const & s1, std::string & s2)
{
	std::ifstream file_stream;

	file_stream.open(s1.c_str());
	readFileStream(file_stream, s2);
	file_stream.close();

	return s2;
}

bool				isDir(const std::string & path)
{
	struct stat s;

	if (lstat(path.c_str(), &s) == 0)
	{
		if (S_ISDIR(s.st_mode)) return true;
	}
	
	return false;
}

bool				ifFile(const std::string & path)
{
	struct stat s;

	if (lstat(path.c_str(), &s) == 0)
	{
		if (S_ISREG(s.st_mode))  return true;
	}

	return false;
}

const std::string  autoindex_header(std::string const & path)
{
  return ("<html>\n<head><meta content=\"text/html;charset=utf-8\" http-equiv=\"Content-Type\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"><meta content=\"utf-8\" http-equiv=\"encoding\"><title>HTTP Autoindex</title><style> \
      div {display: flex; flex-wrap: wrap; justify-content: space-between; max-width: 80%; padding: 0.25rem; border-radius: 0.75rem;} div:hover {background-color: rgba(0, 0, 0, 0.25);} \
      svg {display: inline-block; width: 25px; height: 25px; margin-right: 0.25rem;} a {position: relative; display: inline; vertical-align: top;} .file_name {top: 0; left: 30px; white-space: nowrap;} \
      .flexible {display: flex; flex-direction: row; justify-content: space-around; gap: 1rem;} a:hover .folder .folder-front {transform: translate(0px, 230px) rotateX(60deg);} \
      a:hover .default-file .pencil { display: block; transform: translate(-20px, -35px); animation: 5s draw ease-in infinite; } @keyframes draw { \
      0% {transform: translate(-25px, -30px);} 5% {transform: translate(-20px, -35px);} 10% {transform: translate(-15px, -30px);} 15% {transform: translate(-10px, -35px);} 20% {transform: translate(-5px, -30px);} \
      25% {transform: translate(-0px, -30px);} 30% {transform: translate(-25px, -20px);} 35% {transform: translate(-18px, -25px);} 40% {transform: translate(-11px, -21px);} 45% {transform: translate(-5px, -25px);} \
      50% {transform: translate(-0px, -22px);} 55% {transform: translate(-24px, -15px);} 60% {transform: translate(-21px, -18px);} 65% {transform: translate(-12px, -12px);} 70% {transform: translate(-7px, -17px);} \
      75% {transform: translate(-0px, -12px);} 80% {transform: translate(-5px, -23px);} 90% {transform: translate(-25px, -30px);} 100% {transform: translate(-25px, -30px);} } \
      a:hover .file .top-bar {animation: 2s shrink ease-out;} a:hover .html-file .html-tag {animation: 2s flick ease-out infinite;} \
      @keyframes flick {to {opacity: 0;}} @keyframes shrink {0% {transform: rotateY(25deg) translate(0px,0px);} 70% {transform: rotateY(65deg) translate(40px,0px);} 90% {transform: rotateY(80deg) translate(150px,0px);} 100% {transform: rotateY(85deg) translate(285px,0px);}} \
      </style></head>\n<body>\n<h3>Autoindex for " + path + "</h3><hr>");
}