/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SessionClient.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlonghin <tlonghin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 06:23:07 by tlonghin          #+#    #+#             */
/*   Updated: 2025/08/12 07:05:28 by tlonghin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <SessionClient.hpp>

SessionClient::SessionClient() : uuid("") {}

SessionClient::SessionClient(std::string &uuid) : uuid(uuid) {}

std::string SessionClient::getUUID() { return this->uuid; }

void    SessionClient::setUUID(const std::string &uuid) { this->uuid = uuid; }

SessionClient::~SessionClient() {}