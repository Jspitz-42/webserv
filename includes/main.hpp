/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: altheven <altheven@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 11:46:50 by jspitz            #+#    #+#             */
/*   Updated: 2025/08/20 14:01:36 by altheven         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef MAIN_HPP

# define MAIN_HPP

# include <string>
# include <cstring>
# include <iostream>
# include <sys/types.h>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <cstdio>
# include <cstdlib>
# include <unistd.h>
# include <csignal>
# include "tcpServer.hpp"

# define TIME_TO_DIE 5000

extern int g_signal;

extern int g_epoll_fd;

void sigint_handle_main(int signum);

int set_sig_handler(int signum, void (*fn)(int));

#endif