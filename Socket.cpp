/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nlaerema <nlaerema@student.42lehavre.fr>	+#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/05 10:58:17 by nlaerema          #+#    #+#             */
/*   Updated: 2024/03/16 19:34:04 by cgodard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"

Socket::Socket(void):	fd(INVALID_FD)
{
}

Socket::Socket(int fd):	fd(fd)
{
}

Socket::Socket(int domain, int type, int protocol):	fd(INVALID_FD)
{
	this->open(domain, type, protocol);
}

Socket::~Socket(void)
{
	this->close();
}

int		Socket::open(int domain, int type, int protocol)
{
	this->fd = socket(domain, type, protocol);
	if (this->fd == INVALID_FD)
		return (EXIT_ERRNO);
	this->close();
	return (EXIT_SUCCESS);
}

bool	Socket::is_open(void) const
{
	return (this->fd != INVALID_FD);
}

void	Socket::close(void)
{
	if (this->is_open())
		::close(this->fd);
	this->fd = INVALID_FD;
}

int		Socket::getFd(void) const
{
	return (this->fd);
}

void	Socket::setFd(int fd)
{
	this->close();
	this->fd = fd;
}
