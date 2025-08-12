/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jspitz <jspitz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 11:34:29 by jspitz            #+#    #+#             */
/*   Updated: 2025/08/12 11:37:06 by jspitz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>

int main( void )
{

	std::cout << "Content-type:text/html";
	std::cout << "<html>" << std::endl;
	std::cout << "<head>" << std::endl;
	std::cout << "<title> Hello From First Cgi</title>" << std::endl;
	std::cout << "</head>" << std::endl;
	std::cout << "<body" << std::endl;
	std::cout << "<h2>Really, the first one</h2>" << std::endl;
	std::cout << "</body>" << std::endl;
	std::cout << "</html>" << std::endl;
	return 0;
}