/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jspitz <jspitz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/29 07:38:12 by jspitz            #+#    #+#             */
/*   Updated: 2025/08/07 13:35:33 by jspitz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# define AUTOINDEX					1
# define CGI						2
# define CGIBIN						3
# define CLIENTMAXBODYSIZE			4
# define ERRORPAGE					5
# define LIMITMETHODS				6
# define LISTEN						7
# define LOCATION					8
# define ROOT						9
# define SERVERNAME					10
# define UPLOAD						11
# define INDEX						12
# define REDIRECT					13

# define SERVER_CONTEXT_DIRECTIVE	8
# define LOCATION_CONTEXT_DIRECTIVE	10
# define TOTAL_DIRECTIVE			13
# define ALL_ERROR_CODES			40
# define PORT_MAX					65535
# define PORT_MIN					1
# define SEPARATORS					" \t\v\n\r\f;"
# define SERVER_CONTEXT				1
# define LOCATION_CONTEXT			2

# include <string>
# include <cstring>
# include <fstream>
# include <sstream>
# include <map>
# include <vector>
# include <cstdlib>

class Config
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

		class ServerConfig
		{
			public:
				class Directive
				{
					public:
																			Directive( int );
						int													getId( void ) const;
						virtual												~Directive( void );
						virtual void										setDirective(ServerConfig &, int) const = 0;

					protected:

					private:
						int													_id;
																			Directive();
				} ;
				
				class AutoIndex : public Directive
				{
					public:
																			AutoIndex(const std::string &) throw (std::exception);
						virtual												~AutoIndex( void );
						virtual void										setDirective(ServerConfig &, int) const;
					
					protected:

					private:
						bool												_option;
				} ;
				
				class Cgi : public Directive
				{
					public:
																			Cgi(const std::string &) throw (std::exception);
						virtual 											~Cgi( void );
						virtual void 										setDirective(ServerConfig &, int) const;

					protected:

					private:
						std::vector<std::string>							_cgi;
						void 												_parseCgiContent(std::vector<std::string> &, const std::string &);
				} ;

				class CgiBin : public Directive
				{
					public:
																			CgiBin(const std::string & ) throw (std::exception);
						virtual												~CgiBin( void );
						virtual void										setDirective(ServerConfig &, int ) const;

					protected:

					private:
						std::string		 									_path;
				} ;

				class ClientMaxBodySize : public Directive
				{
					public: 
																			ClientMaxBodySize(const std::string &) throw (std::exception);
						virtual 											~ClientMaxBodySize( void );
						virtual void										setDirective(ServerConfig &, int) const;

					protected:

					private:
						int													_max_size;
				} ;

				class ErrorCodePage : public Directive
				{
					public:
																			ErrorCodePage(const std::string & s) throw (std::exception);
						virtual 											~ErrorCodePage();
						virtual void 										setDirective(ServerConfig & , int) const;
						static const int									_allErrorCodes[ALL_ERROR_CODES];
					protected:

					private:
		
						std::vector<int>									_error_codes;
						std::string											_error_path;
						bool												loadErrorCodes(const std::string &);
																			ErrorCodePage();
				} ;

				class Index: public Directive
				{
					public:
																			Index(const std::string &) throw (std::exception);
						virtual												~Index( void );
						virtual void										setDirective(ServerConfig &, int) const;
					protected:

					private:
						std::vector<std::string> 							_indexes;
																			Index( void );
				};
				
				class Methods : public Directive
				{
					public:
																			Methods(const std::string &) throw (std::exception);
						virtual												~Methods( void );
						virtual void										setDirective(ServerConfig & , int) const;
						static const std::string							_valid_methods[4];
					protected:

					private:
						std::vector<std::string>							_methods;
						bool												_validMethod(const std::string &);
																			Methods( void );
				};

				class Listen : public Directive
				{
					public:
																			Listen(const std::string &) throw (std::exception);
						virtual												~Listen( void );
						virtual void										setDirective(ServerConfig &, int) const;

					protected:

					private:
						std::string											_ip;
						int													_port;
						bool												isIpValid(const std::string &);
																			Listen( void );
				};

				class Location: public Directive
				{
					public:
																			Location(std::string const &) throw (std::exception);
						virtual												~Location( void );
						virtual void										setDirective(ServerConfig &, int) const;
						bool												findMethod(std::string const &) const;
						bool												checkMaxBody( int ) const;
						
						std::vector<std::string>							_methods;
						std::string											_root_path;
						std::string											_target;
						std::vector<std::string>							_indexes;
						
						int													_max_body_size;
						int													_redirect_status;
						bool												_autoindex;
						
						std::string											_cgi_bin;
						std::string											_upload_path;
						std::string											_redirect_uri;
						std::map<std::string, std::vector<std::string> >	_cgi_map;
						std::map<std::string, std::vector<int> >			_location_errors_map;

					protected:

					private:
																			Location( void );
				} ;

				class Redirect : public Directive
				{
					public:
																			Redirect(const std::string & ) throw (std::exception);
						virtual												~Redirect( void );
						virtual void										setDirective(ServerConfig &, int) const;
						static std::map<int, std::string>					_redirect_status_codes;

					protected:


					private:
						int													_status_code;
						std::string											_redirect_uri;
				} ;

				class Root: public Directive
				{
					public:
																			Root(const std::string &) throw (std::exception);
						virtual												~Root( void );
						virtual void										setDirective(ServerConfig &, int) const;
						
					protected:

					private:
						const std::string &									_path;
																			Root( void );
				} ;

				class ServerName: public Directive
				{
					public:
																			ServerName(const std::string &) throw (std::exception);
						virtual												~ServerName( void );
						virtual void										setDirective(ServerConfig &, int) const;

					protected:

					private:
						std::vector<std::string>							_server_names;
																			ServerName( void );
				} ;
				
				class Upload : public Directive
				{
					public:
																			Upload(const std::string &) throw (std::exception);
						virtual												~Upload( void );
						virtual void										setDirective(ServerConfig &, int) const;
					protected:

					private:
						std::string											_upload_path;
				} ;

				
															ServerConfig( void );
															~ServerConfig( void );
				Location *									findLocation(std::string const &) const;
				void										setDefault( void );
				int											getPort( void ) const;
				bool										checkMaxBody( int ) const;
				std::string const & 						getIp( void ) const;
				bool										_autoindex;
				int											_max_body_size;
				std::string									_ip;
				int											_port;
				std::string									_root_path;
				std::vector<Location>						_locations;
				std::vector<std::string>					_indexes;
				std::vector<std::string>					_names;
				std::map<std::string, std::vector<int> > 	_server_error_maps;
				static const std::string					_valid_methods_server[3];
				
			protected:

			private:
		} ;
	
	public:
											Config( void );
											Config(std::string const &) throw(std::exception);
											~Config( void );
		std::vector<ServerConfig>			_servers;
	protected:

	private:
		static const std::string			_server_directives[SERVER_CONTEXT_DIRECTIVE];
		static const std::string			_location_directives[LOCATION_CONTEXT_DIRECTIVE];
		bool								validDirective(const std::string &, const std::string *, int len) const;
		Config::ServerConfig::Directive *	createDirective(std::string const &, std::string const &) throw(std::exception);
											Config(const Config &);

} ;

//std::ostream & operator<<(std::ostream &, const Config &);
//std::ostream & operator<<(std::ostream &, const Config::ServerConfig &);