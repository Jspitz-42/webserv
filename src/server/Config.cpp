/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: altheven <altheven@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 13:27:53 by jspitz            #+#    #+#             */
/*   Updated: 2025/08/12 14:17:18 by altheven         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Config.hpp"
# include "utils.hpp"
Config::Config(void) { return ;}

const std::string Config::_server_directives[SERVER_CONTEXT_DIRECTIVE] = {"root", "listen", "server_name", "error_page", "client_max_body_size", "location", "index", "autoindex", "directory_listing"};

const std::string Config::_location_directives[LOCATION_CONTEXT_DIRECTIVE] = {"root", "index", "allow_methods", "autoindex", "error_page", "client_max_body_size", "cgi", "cgi-bin", "upload", "redirect"};

const std::string Config::ServerConfig::Methods::_valid_methods[4] = {"GET", "POST", "DELETE", "PUT"};

const int Config::ServerConfig::ErrorCodePage::_allErrorCodes[ALL_ERROR_CODES] = {	
	400, 401, 402, 403, 404, 405, 406, 407, 408, 409, 410, 411, 412, 413, 414, 415, 416, 417, 418,
	421, 422, 423, 424, 425, 426, 428, 429, 431, 451, 500, 501, 502, 503, 504, 505, 506, 507, 508, 510, 511
};

static const std::map<int, std::string> genRedirectStatusCodes()
{
	std::map<int, std::string> m;

	m[300] = "Multiple Choice";
	m[301] = "Moved Permanently";
	m[302] = "Found";
	m[303] = "See Other";
	m[304] = "Not Modified";
	m[307] = "Temporary Redirect";
	m[308] = "Permanent Redirect";

	return m;
}

std::map<int, std::string> Config::ServerConfig::Redirect::_redirect_status_codes = genRedirectStatusCodes();

bool Config::validDirective(const std::string & str, const std::string * list, int len) const
{
	int	i = 0;

	while (i < len)
	{
		if (list[i++] == str) return true;
	}

	return false;
}

Config::Config(std::string const & file_s) throw(std::exception)
{
	std::ifstream	file;
	std::string		tmp;
	std::string		line;
	std::string		directive;
	std::string		directive_content;
	int				context = 0;

	file.open(file_s.c_str(), std::ios::in);

	if (!file.is_open()) throw Config::ErrorMessage(CONFIG_INVALID_FILE);
	
	while (std::getline(file, line))
	{
		line = strtrim(line);
		if (!line.length() || line[0] == '#')
			continue ;

		directive = line.substr(0, line.find_first_of(SEPARATORS));
		directive = strtrim(directive);
		
		if (line.find_first_of(SEPARATORS) == std::string::npos) {
			directive_content = "";
		} else {
			tmp = line.substr(line.find_first_of(SEPARATORS) + 1);
			directive_content = strtrim(tmp);
		}

		if (directive == "}" && directive_content.empty()) {
			if (--context < 0) throw Config::ErrorMessage(CONFIG_INVALID_DIR_EMPTY);
			
			continue ;
		}

		switch (context)
		{
			case 0:
				if (directive == "server" && directive_content == "{") {
					ServerConfig s;
					_servers.push_back(s);
					context++;
				} else {
					throw Config::ErrorMessage(CONFIG_CASE_0_ERR);
				}
				break ;
			
			case SERVER_CONTEXT:
				if (validDirective(directive, _server_directives, SERVER_CONTEXT_DIRECTIVE)) {
					ServerConfig::Directive * _directive;
					_directive = createDirective(directive, directive_content);
					if (_directive != 0) {
						_directive->setDirective(_servers.back(), context);
						if (directive == "location") {
							_servers.back().setDefault();
							context++;
						}
						delete (_directive);
					} else {
						throw Config::ErrorMessage(CONFIG_CASE_SERVER_CONTEXT_ERR_1);
					}
				} else {
					throw Config::ErrorMessage(CONFIG_CASE_SERVER_CONTEXT_ERR_2);
				}
				break ;
			
				case LOCATION_CONTEXT:
				if (validDirective(directive, _location_directives, LOCATION_CONTEXT_DIRECTIVE)) {
					ServerConfig::Directive * _directive;
						_directive = createDirective(directive, directive_content);
						if (_directive != 0) {
							_directive->setDirective(_servers.back(), context);
							delete(_directive);
						} else {
							throw Config::ErrorMessage(CONFIG_CASE_LOCATION_CONTEXT_ERR_1);
						}
					} else {
						throw Config::ErrorMessage(CONFIG_CASE_LOCATION_CONTEXT_ERR_2);
					}
					break ;
					
				default:
					throw Config::ErrorMessage(CONFIG_CASE_LOCATION_CONTEXT_ERR_3);
		}
	}
}

Config::~Config( void ) { return ;}
