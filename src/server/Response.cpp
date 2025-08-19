/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: altheven <altheven@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 13:30:17 by jspitz            #+#    #+#             */
/*   Updated: 2025/08/19 12:26:02 by altheven         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
# include "Request.hpp"

static void push_back_env(std::vector<char *> & vec, std::string const & name, std::string const & value)
{
	if (!name.empty() && !value.empty()) {
		
		std::string tmp(value);
		tmp = name + "=" + strtrim(tmp);
		
		if (tmp.length() < 8000) {
			char * str = strdup(tmp.c_str());
			vec.push_back(str);
		}
	}
}

static const std::map<int, std::string> insert_to_error_map( void )
{
	std::map<int, std::string> _codeMessage;

	_codeMessage[200] = "OK";
	_codeMessage[400] = "Bad Request";
	_codeMessage[403] = "Forbbiden";
	_codeMessage[404] = "Not Found";
	_codeMessage[405] = "Method Not Allowed";
	_codeMessage[413] = "Payload Too Large";
	_codeMessage[414] = "URI Too Long";
	_codeMessage[415] = "Unsupported Media Type";
	_codeMessage[431] = "Request Header Fields Too Large";
	_codeMessage[500] = "Internal Server Error";
	_codeMessage[505] = "HTTP Version Not Supported";

	return _codeMessage;
}

static const std::map<std::string, std::string> insert_to_mime_map( void )
{
	std::map<std::string, std::string> mime_map;

	mime_map["txt"]		= "text/plain";	
	mime_map["html"]	= "text/html";
	mime_map["js"] 		= "text/javascript";
	return (mime_map);
}

std::map<int, std::string> Response::_codeMessage = insert_to_error_map();

std::map<std::string, std::string> Response::_mime_types = insert_to_mime_map();

Response::Response(Request const & request, Config::ServerConfig const & sc):	_keep_alive(true),
																				_autoindex(false),
																				_cgi_response(false),
																				_req(request),
																				_server_config(sc)
{
	std::string							location;
	std::ifstream						file;
	
	_status_code = _req.getErrorCode();
	_date = get_local_time();
	_server_name = "Tom_Nook";
	
	if (_req.isTargetRedirect() && _req._lock) {
		_status_code = _req._lock->_redirect_status;
	} else if (_req.getErrorCode() == 0 && _req._lock) {
		_status_code = 404;
	
		if (_req.isTargetCGI()) {
			if (isDir(_req._lock->_cgi_bin)) {
				_status_code = execCGI();
				if (_status_code <= 0)
					_status_code = 500;
			} else if (_req.getMethod() == "DELETE") {
				location = _req.getFinalPath();
			
				if (ifFile(location) && !isDir(location))
					_status_code = 200;
			}
		} else if (_req.isTargetDir()) {
			location = _req.getIndex();
			if (ifFile(location)) {
				file.open(location.c_str(), std::ifstream::binary);
				if(file.is_open()) {
					readFileStream(file, _content);
					_status_code = 200;
					setMimeType(location);
				} else {
					_status_code = 403;
				}
			} else {
				if (_req._lock->_autoindex) {
					_status_code = 200;
					_autoindex = true;
				} else if (isDir(_req.getFinalPath())) {
					_status_code = 403;
				}
			}
		} else {
			location = _req.getFinalPath();
			file.open(location.c_str(), std::ifstream::binary);
			if(file.is_open() && ifFile(location)) {

				readFileStream(file, _content);
				_status_code = 200;
				setMimeType(location);
			}
		}
		file.close();
	}

}

Response::~Response()
{
 return ;
}

void Response::setMimeType(std::string const & file_name)
{
	size_t pos(file_name.find_last_of('.'));

	if (pos != std::string::npos) {
		std::string ext = file_name.substr(pos + 1);
	    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
	    
		if (_mime_types.find(ext) != _mime_types.end()) {
	    	_content_type = _mime_types[ext];
	    	return ;
	    } else {
			_content_type = "text/html";
	    }
	} else {
		_content_type = "application/octet-stream";
	}
}

int Response::execCGI()
{
	std::vector<char *> env;
	std::vector<char *>::iterator it;
	std::vector<char *> arg;
	std::stringstream ss;

	ss << _req.getContent().length();
	push_back_env(env, "PATH", _req.getCGIFile());

	push_back_env(env, "CONTENT_LENGTH", ss.str());
	push_back_env(env, "CONTENT_TYPE", "application/x-www-form-urlencoded");
	push_back_env(env, "DOCUMENT_ROOT", _req._lock->_upload_path);
	push_back_env(env, "GATEWAY_INTERFACE", "CGI/1.1");
	push_back_env(env, "HTTP_ACCEPT", "application/x-www-form-urlencoded,text/xml,application/xml,application/xhtml+xml,text/html,text/plain,charset=utf-8;");
	push_back_env(env, "HTTP_USER_AGENT", _req.getUserAgent());
	push_back_env(env, "PATH_INFO", _req.getCGIBindPath());
	push_back_env(env, "PATH_TRANSLATED", _req.getCGIBindPath());
	push_back_env(env, "QUERY_STRING", _req.getQuery());
	push_back_env(env, "REQUEST_METHOD", _req.getMethod());
	push_back_env(env, "REQUEST_URI", _req.getUriTarget());
	push_back_env(env, "SCRIPT_FILENAME", _req.getCGIFile());
	push_back_env(env, "SCRIPT_NAME", _req.getCGIFile());
	push_back_env(env, "SERVER_ADMIN", "jspitz@student42.fr");
	push_back_env(env, "SERVER_NAME", "WebServ");
	ss.str(std::string());
	ss << _server_config.getPort();
	push_back_env(env, "SERVER_PORT", ss.str());
	push_back_env(env, "SERVER_PROTOCOL", "HTTP/1.1");
	push_back_env(env, "SERVER_SOFTWARE", "Webserv42.0 (Linux)");
	if (!_req.getCookies().empty())
		push_back_env(env, "HTTP_COOKIE", _req.getCookies());
	env.push_back(NULL);
	arg.push_back(strdup(_req.getCGIFile().c_str()));
	arg.push_back(NULL);

	// for (int i = 0; env[i]; i++)
	// {
	// 	std :: cout << env[i] << " --- I VALUE = " <<i << std :: endl;
	// }

	int restore_input = dup(STDIN_FILENO);
	int restore_output = dup(STDOUT_FILENO);
	
	FILE * tmp_file_in = tmpfile();
	FILE * tmp_file_out = tmpfile();
	if (!tmp_file_in || !tmp_file_out || restore_input < 0)
		return (-1);
    int tmp_fd_in = fileno(tmp_file_in);
    int tmp_fd_out = fileno(tmp_file_out);
	
	if (tmp_fd_in < 0)
		return (-1);
	write(tmp_fd_in, _req.getContent().c_str(), _req.getContent().length());
	rewind(tmp_file_in);
	fclose(tmp_file_in);
	
	int pid = fork();
	if (pid < 0)
		return (-1);
	if (pid == 0) {
		dup2(tmp_fd_in, STDIN_FILENO);
		dup2(tmp_fd_out, STDOUT_FILENO);
		close(tmp_fd_in);
		close(tmp_fd_out);
		if (chdir(_req._lock->_cgi_bin.c_str()) != -1) {
			execve(_req.getCGIFile().c_str(), &arg[0], &env[0]);
		}
		exit(EXIT_FAILURE);
    }

    for (it = env.begin(); it != env.end(); ++it)
    	if (*it)
    		free (*it);
    for (it = arg.begin(); it != arg.end(); ++it)
    	if (*it)
    		free (*it);
    int child_status;
	
	waitpid(pid, &child_status, 0);
    close(tmp_fd_in);
    rewind(tmp_file_out);
    
	char buff[1024];
    int valread = -1;
    while(valread != 0)
	{
    	bzero(buff, 1024);
		valread = read(tmp_fd_out, buff, 1023);
		if (valread < 0) {
			close(tmp_fd_out);
			fclose(tmp_file_out);
			return (-1);
		}
		_content += buff;
    }    
	if (child_status != 0) {
    	_content = "ERROR!!!";
    	return (500);
    }
   
	close(tmp_fd_out);
	fclose(tmp_file_out);
    
	dup2(restore_input, STDIN_FILENO);
    dup2(restore_output, STDOUT_FILENO);
   
	close(restore_input);
    close(restore_output);
	
	std::cout << " CGI Child Finished with status: " << child_status;
    _cgi_response = true;
	return (200);
}

const std::string Response::createAutoindexResponse()
{
	std::string		 html_content;
	DIR	*			dr;
	struct dirent *	de;
    struct stat		st;
    struct tm		tm_time;

    dr = opendir(_req.getFinalPath().c_str());
	if (dr == NULL) {
		_status_code = 404;
	} else {
		html_content = autoindex_header(_req.getFinalPath());
		while ((de = readdir(dr)) != NULL) {
			if (*de->d_name == 0 || (*de->d_name == '.' && *(de->d_name + 1) == 0))
				continue ;
			std::string file_path(_req.getFinalPath() + "/" + de->d_name);
			int fd = open(file_path.c_str(), O_RDONLY);
			if (fd < 0 || fstat(fd, &st) == -1) {
				html_content += "<div>Failed to open " + file_path + "</div>";
				continue ;
			}
			gmtime_r(&(st.st_mtim.tv_sec), &tm_time);
			std::string s_time(asctime(&tm_time));
			std::string tmp_s_time(s_time.substr(0, s_time.length() - 1));
			std::string file_name(de->d_name);
			if (S_ISDIR(st.st_mode))
				html_content += "<div><a href=\"" + file_name + "/\">"  + "<span class=\"file_name\">" + de->d_name + "/</span></a><span class=\"flexible\"><span>" + tmp_s_time + "</span><span> - </span></span></div>";
			else {
				size_t num_file_size(st.st_size);
				std::stringstream ss;
				ss << num_file_size;
				html_content += "<div><a href=\"" + file_name + "\">"  + "<span class=\"file_name\">" + de->d_name + "</span></a><span class=\"flexible\"><span>" + tmp_s_time + "</span><span> " + ss.str() + " Bytes</span></span></div>";
			}
			close (fd);
	    }
	    html_content += "\n<hr><center>Tom_Nook/1.0</center>\n</body>\n</html>\n";
	    closedir(dr);
	}
	return (html_content);
}

const std::string Response::CGIResponse()
{
	std::string response;
	std::stringstream ss;
	std::stringstream ss2;

	ss << _status_code;
	response += "HTTP/1.1 " + ss.str() + " " + _codeMessage[_status_code] + "\n";
	response += "Date: " + _date;
	response += "Server: " + _server_name + "\n";
	response += "Access-Control-Allow-Origin: *\n";
	response += "Connection: close\n";
	ss.str(std::string());
	ss2 << _content;
	std::string line;
	int	content_len(0);
	while (getline(ss2, line)) {
		if (line.empty())
			break;
	}
	while (getline(ss2, line))
		content_len += line.length();
	ss << content_len;
	response += "Content-Length: " + ss.str() + "\n";
	response += _content;
	return (response);
}

const std::string Response::createRedirectionResponse() {
	std::string response_content;
	std::stringstream ss;

	ss << _status_code;
	response_content += "HTTP/1.1 " + ss.str() + " " + Config::ServerConfig::Redirect::_redirect_status_codes[_status_code] + "\n";
	response_content += "Date:" + _date;
	response_content += "Server: Tom_Nook\n";
	response_content += "Location: " + _req._lock->_redirect_uri +"\n";
	response_content += "Connection: close\n";
	_keep_alive = false;
	std::cout << "REDIRECTING TO: " << _req._lock->_redirect_uri << std::endl;
	return (response_content);
}

const std::string Response::deleteResponse() {
	std::string response_content;
	std::stringstream ss;
	std::string html_content;

	html_content = "<html><body>""<h1>File Deleted: ";
	html_content += _req.getFinalPath();
	html_content += "</h1></body></html>";
	response_content += "HTTP/1.1 200 " + _codeMessage[200] + "\n";
	response_content += "Date:" + _date;
	response_content += "Server: Tom_Nook\n";
	ss << html_content.length();
	response_content += "Content-Length: " + ss.str() + "\n";
	response_content += "Connection: close\n";
	response_content += "\r\n";
	response_content += html_content;
	_keep_alive = false;
	return (response_content);	
}

const std::string Response::createResponse() {
	std::string response;
	std::string html_content;
	std::ostringstream so;
	std::ifstream file;
	
	if (_req.isTargetRedirect()) {
		return (createRedirectionResponse());
	}
	if (_cgi_response && _status_code == 200) {
		_keep_alive = false;
		return (CGIResponse());
	}
	if (_req.getMethod() == "DELETE" && _status_code == 200) {
		if(remove(_req.getFinalPath().c_str()) != 0)
			_status_code = 404;
		return (deleteResponse());
	}
	if (_status_code == 200 && _req._lock) {
		if (_content.length() > 0) {
			html_content = _content;
		} else if (_autoindex) {
			html_content = createAutoindexResponse();
		}
	}
	so << _status_code;
	if (_status_code != 200) {
		_keep_alive = false;
		if (_req._lock) {
			std::map<std::string, std::vector<int> >::iterator l_it;
			for (l_it = _req._lock->_location_errors_map.begin(); l_it != _req._lock->_location_errors_map.end() && html_content.empty(); ++l_it) {
				std::vector<int>::iterator e_it;
				for (e_it = l_it->second.begin(); e_it != l_it->second.end(); ++e_it) {
					if (*e_it == _status_code) {
						break ;
					}
				}
				if (e_it != l_it->second.end()) {
					std::string error_loc = l_it->first + so.str() + ".html";

					file.open(error_loc.c_str(), std::ifstream::binary);
					if(file.is_open()) {
						readFileStream(file, html_content);
						_content_type = "text/html";
					}
					break ;
				}
			}
		}
		std::map<std::string, std::vector<int> >::const_iterator l_it;
		for (l_it = _server_config._server_error_maps.begin(); l_it != _server_config._server_error_maps.end() && html_content.empty(); ++l_it) {
			std::vector<int>::const_iterator e_it;
			for (e_it = l_it->second.begin(); e_it != l_it->second.end(); ++e_it) {
				if (*e_it == _status_code) {

					break ;
				}
			}
			if (e_it != l_it->second.end()) {
				std::string error_loc = l_it->first + so.str() + ".html";

				file.open(error_loc.c_str(), std::ifstream::binary);
				if(file.is_open()) {

					readFileStream(file, html_content);
					_content_type = "text/html";
				}
				break ;
			}
		}
		if (!html_content.length()) {
			_content_type = "text/html";
			html_content = "<html>\n<head><title>" + so.str() + "</title></head>\n<body bgcolor=\"gray\">\n<center><h1>" + so.str() + " " + _codeMessage[_status_code] + "</h1></center>\n<hr><center>Tom_Nook/1.0</center>\n</body>\n</html>\n";
		}
	}
	response += "HTTP/1.1 " + so.str() + " " + _codeMessage[_status_code] + "\n";
	const std::string cookieValue = !this->_req.getCookies().empty() ? this->_req.getCookies().substr(this->_req.getCookies().find("id_session=")) : "";
	if (!cookieValue.empty()) {
		std::cout << cookieValue << std::endl;
		std::cout << "Cookie already exist !" << std::endl;
		response += "Set-Cookie: ";
		response += cookieValue;
		response += "; Path=/; HttpOnly\n";
	} else {
		std::cout << "new client added !" << std::endl;
		const std::string newCookie = const_cast<Request&>(this->_req).createClientId();
		std::cout << newCookie << std::endl;
		response += "Set-Cookie: id_session=";
		response += newCookie;
		response += "; Path=/; HttpOnly\n";
	}
	response += "Date: " + _date;
	response += "Server: " + _server_name + "\n";
	response += "Accept-Charset: utf-8\n";
	response += "Content-Type: " + _content_type + "\n";
	so.str(std::string());
	so << html_content.length();
	response += "Content-Length: " + so.str() + "\n";
	response += "Connection: keep-alive\n";
	response += "\r\n";
	response += html_content;
	return (response);
}

bool Response::getKeepAlive(void) const {
	return (_keep_alive);
}

int Response::getStatusCode(void) const {
	return (_status_code);
}

std::ostream& operator<<(std::ostream& s, const Response& param) {
	(void)param;
	return (s);
}