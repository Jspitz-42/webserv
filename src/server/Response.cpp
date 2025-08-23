/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: altheven <altheven@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 13:30:17 by jspitz            #+#    #+#             */
/*   Updated: 2025/08/23 13:25:33 by altheven         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
# include "Request.hpp"
# include "Upload.hpp"
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
	_content_type = "text/html";
	
	if (_req.getMethod() == "POST" && _req.getUriTarget() == "/upload") {
		Config::ServerConfig::Location * _lock = _server_config.findLocation(_req.getUriTarget());
		std::string uploadDir;
		if (!_lock->_root_path.empty()) {
			uploadDir = _lock->_root_path;
		} else {
			uploadDir = "./uploads";
		}
		
		std::string content = _req.getContent();
		
		if (content.empty()) {
			_content = "<html><body><h1>Error: Empty content received</h1></body></html>";
			_status_code = 400;
			delete _lock;
			return;
		}
		
		std::string contentType = _req.getContentType();
		
		if (contentType.find("multipart/form-data") != std::string::npos) {
			size_t bpos = contentType.find("boundary=");
			if (bpos != std::string::npos) {
				std::string boundary = contentType.substr(bpos + 9);
				bool ok = Upload::handleMultipartUpload(content, boundary, uploadDir);
				
				if (ok) {
					_content = "<html><body><h1>File uploaded successfully!</h1><a href='/'>Back to home</a></body></html>";
					_status_code = 200;
				} else {
					_content = "<html><body><h1>Upload failed!</h1><a href='/'>Back to home</a></body></html>";
					_status_code = 500;
				}
				_content_type = "text/html";
				delete _lock;
				return;
			}
		}
	}
	else if(_req.getMethod() == "DELETE")
	{

		std::string content = _req.getContent();
		
		if (content.empty())
		{
			_content = "<html><body><h1>Error: Empty content received</h1></body></html>";
			_status_code = 400;
			return ;
		}

		std :: string dstr = "name=\"deleteFiles\"";
		std :: string dstr_bis = _req.getContentType();
		std::size_t findpos = dstr_bis.find("boundary=");
		if (findpos== std::string::npos)
		{
			_status_code = 400;
			return ;
		}
		dstr_bis = dstr_bis.substr(findpos + 9);
		size_t dpos_bis = content.find_last_of(dstr_bis);
		size_t dpos = content.find(dstr);
		content = content.substr(dpos + dstr.size(), dpos_bis - dpos - dstr_bis.size() - dstr.size());
		content.erase(0,4);
		findpos = content.find("\n");
		if (findpos== std::string::npos || findpos <= 0)
		{
			_status_code = 400;
			return ;
		}
		content = content.substr(0, findpos - 1);
		_delete_name = _req._lock->_root_path + "/" + content;
	}
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

int Response::execCGI() {
    FILE *tmp_in = tmpfile();
    if (!tmp_in)
		return 500;
    int fd_in = fileno(tmp_in);
    ssize_t body_size = _req.getContent().size();
    if (body_size > 0)
		write(fd_in, _req.getContent().c_str(), body_size);
    rewind(tmp_in);

    FILE *tmp_out = tmpfile();
    if (!tmp_out)
	{
		fclose(tmp_in);
		return 500;
	}
    int fd_out = fileno(tmp_out);

    std::vector<char*> env;
    std::stringstream ss;
    ss << body_size;
    push_back_env(env, "CONTENT_LENGTH", ss.str());
    push_back_env(env, "CONTENT_TYPE", _req.getContentType());
    push_back_env(env, "GATEWAY_INTERFACE", "CGI/1.1");
    push_back_env(env, "REQUEST_METHOD", _req.getMethod());
    push_back_env(env, "SCRIPT_FILENAME", _req.getCGIFile());
    push_back_env(env, "SCRIPT_NAME", _req.getCGIFile());
    push_back_env(env, "PATH_INFO", _req.getCGIBindPath());
    push_back_env(env, "PATH_TRANSLATED", _req.getCGIBindPath());
    push_back_env(env, "QUERY_STRING", _req.getQuery());
    push_back_env(env, "SERVER_PROTOCOL", "HTTP/1.1");
    env.push_back(NULL);

    pid_t pid = fork();
    if (pid == 0) {
        dup2(fd_in, STDIN_FILENO);
        dup2(fd_out, STDOUT_FILENO);
        close(fd_in); close(fd_out);
        chdir(_req._lock->_cgi_bin.c_str());
        char *argv[2] = { strdup(_req.getCGIFile().c_str()), NULL }; 
        execve(_req.getCGIFile().c_str(), argv, &env[0]);
		free(argv[0]);
		for (size_t i = 0; i < env.size(); i++) {
			free(env[i]);
		}
        exit(1);
    } else if (pid > 0) {
		for (size_t i = 0; i < env.size(); i++) {
			free(env[i]);
		}
        int status;
        int elapsed = 0;
        while (true) {
            pid_t r = waitpid(pid, &status, WNOHANG);
            if (r == pid)
				break;
            if (elapsed >= 30) {
                kill(pid, SIGKILL);
                waitpid(pid, &status, 0);
                fclose(tmp_in); fclose(tmp_out);
                return 504;
            }
            usleep(100000);
            elapsed += 0.1;
        }
        rewind(tmp_out);
        std::stringstream ssout;
        char buffer[4096];
        ssize_t n;
        while ((n = read(fd_out, buffer, sizeof(buffer))) > 0) 
			ssout.write(buffer, n);
        _content = ssout.str();
        _cgi_response = true;
        fclose(tmp_in); fclose(tmp_out);
        return 200;
    } else {
		fclose(tmp_in);
		fclose(tmp_out);
		return 500;
	}
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
		if(remove(_delete_name.c_str()) != 0)
			_status_code = 400;
		if (_status_code == 200)
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
	const std::string& cookies = this->_req.getCookies();
	if (cookies.find("id_session=") == std::string::npos)
	{
	    const std::string newSessionId = const_cast<Request &>(_req).createClientId();
		response += "Set-Cookie: id_session=" + newSessionId + "; Path=/;\n";
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