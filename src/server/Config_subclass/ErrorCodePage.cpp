/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorCodePage.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jspitz <jspitz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 09:09:55 by jspitz            #+#    #+#             */
/*   Updated: 2025/08/11 09:10:41 by jspitz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Config.hpp"

Config::ServerConfig::ErrorCodePage::ErrorCodePage(const std::string & content) throw (std::exception) : Directive(ERRORPAGE)
{
	size_t found = content.find_last_of(SEPARATORS);

	if (content.empty() || found == std::string::npos) {

		throw Config::ErrorMessage(ERRORCODEPAGE_WRONG_SYNTAX);

	} else if (!loadErrorCodes(content.substr(0, found))) {

		throw Config::ErrorMessage(ERRORCODEPAGE_INVALID_DIR);

	}

	_error_path = content.substr(found + 1) ;
}

void Config::ServerConfig::ErrorCodePage::setDirective(ServerConfig & serv_conf, int context) const
{
	if (context == SERVER_CONTEXT) {
		serv_conf._server_error_maps[_error_path] = _error_codes;
	} else if (context == LOCATION_CONTEXT) {

		std::vector<int>::const_iterator it;

		for (it = _error_codes.begin(); it != _error_codes.end() ; ++it)
			serv_conf._locations.back()._location_errors_map[_error_path].push_back(*it);
	}
}

bool Config::ServerConfig::ErrorCodePage::loadErrorCodes(const std::string & content)
{
	std::stringstream	stoi_converter;
	int					converted_number;
	int					loop_counter = 0;

	char *token = strtok(const_cast<char*>(content.data()), SEPARATORS);

	while (token != 0)
	{
		loop_counter++;
		stoi_converter.clear();
		
		stoi_converter << token;
		stoi_converter >> converted_number;
		
		for (int i = 0; i < 40 ; i++) {
			if (converted_number == _allErrorCodes[i]) {
				_error_codes.push_back(converted_number);
				loop_counter--;
			}
		}
		token = strtok(NULL, SEPARATORS);
	}
	return (!loop_counter) ? true : false;
}

Config::ServerConfig::ErrorCodePage::~ErrorCodePage( void )
{
	return ;
}