/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketTcpClient.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nlaerema <nlaerema@student.42lehavre.fr>	+#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/05 10:58:17 by nlaerema          #+#    #+#             */
/*   Updated: 2024/03/17 16:19:48 by nlaerema         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "SocketTcpClient.hpp"

SocketTcpClient::SocketTcpClient(void):	connected(false),
										addrError(0)
{
}

SocketTcpClient::SocketTcpClient(int fd):	connected(false),
											addrError(0)
{
	this->connect(fd);
}

SocketTcpClient::SocketTcpClient(std::string const &addr, std::string const port):	connected(false),
																					addrError(0)
{
	this->connect(addr, port);
}

SocketTcpClient::~SocketTcpClient(void)
{
	this->disconnect();
}

int				SocketTcpClient::getAddrs(struct addrinfo **res,
				std::string const &node, std::string const &service) const
{
	struct addrinfo	hints = {};
	int				error;

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	error = getaddrinfo(node.c_str(), service.c_str(), &hints, res);
	return (error);
}

ssize_t			SocketTcpClient::send(void const *buf, size_t len, int flags) const
{
	return (::send(this->fd, buf, len, flags));
}

ssize_t			SocketTcpClient::send(std::string const &str, int flags) const
{
	return (::send(this->fd, str.c_str(), str.size(), flags));
}

ssize_t			SocketTcpClient::recv(void *buf, size_t len, int flags) const
{
	return (::recv(this->fd, buf, len, flags));
}

ssize_t			SocketTcpClient::recv(std::string &str, int flags) const
{
	static char	buf[SOCKET_TCP_CLIENT_BUF_SIZE];
	ssize_t		bytes_recv(SOCKET_TCP_CLIENT_BUF_SIZE);
	ssize_t		all_bytes_recv(0);

	str.clear();
	while (bytes_recv == SOCKET_TCP_CLIENT_BUF_SIZE)
	{
		bytes_recv = ::recv(this->fd, buf, SOCKET_TCP_CLIENT_BUF_SIZE, flags);
		if (0 < bytes_recv)
		{
			all_bytes_recv += bytes_recv;
			str.append(buf, bytes_recv);
		}
		if (bytes_recv < 0 && all_bytes_recv == 0)
			return (-1);
	}
	return (all_bytes_recv);
}

int				SocketTcpClient::connect(int socketConnected)
{
	int			socketType;
	socklen_t	typeSize;

	typeSize = sizeof(socketType);
	if (getsockopt(socketConnected, SOL_SOCKET, SO_TYPE, &socketType, &typeSize))
		return (EXIT_ERRNO);
	if (socketType != SOCK_STREAM)
		return (EXIT_FAILURE);
	this->setFd(socketConnected);
	return (EXIT_SUCCESS);
}

int				SocketTcpClient::connect(std::string const &addr, std::string const &port)
{
	struct addrinfo				*addrs;
	struct addrinfo				*cr;
	int							fd;

	this->addrError = this->getAddrs(&addrs, addr, port);
	if (this->addrError)
		return (this->addrError);
	for (cr = addrs; cr; cr = cr->ai_next)
	{
		fd = ::socket(cr->ai_family, cr->ai_socktype, cr->ai_protocol);
		if (fd == INVALID_FD)
			continue;
		if (!::connect(fd, cr->ai_addr, cr->ai_addrlen))
			break;
		::close(fd);
	}
	freeaddrinfo(addrs);
	if (!cr)
		return (EXIT_ERRNO);
	this->disconnect();
	this->fd = fd;
	this->connected = true;
	return (EXIT_SUCCESS);
}

void			SocketTcpClient::disconnect(void)
{
	if (this->isConnected())
	{
		this->close();
		this->connected = false;
	}
}

bool			SocketTcpClient::isConnected(void) const
{
	return (this->connected);
}

int				SocketTcpClient::getAddrError(void) const
{
	return (this->addrError);
}

SocketTcpClient	const &SocketTcpClient::operator<<(std::string const &str) const
{
	this->send(str);
	return (*this);
}

std::string		&SocketTcpClient::operator>>(std::string &str) const
{
	this->recv(str);
	return (str);
}
