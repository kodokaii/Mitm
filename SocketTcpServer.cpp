/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketTcpServer.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nlaerema <nlaerema@student.42lehavre.fr>	+#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/05 10:58:17 by nlaerema          #+#    #+#             */
/*   Updated: 2024/03/17 17:25:39 by nlaerema         ###   ########.fr       */
/*                                                                            */ /* ************************************************************************** */

#include "SocketTcpServer.hpp"

SocketTcpServer::SocketTcpServer(void):	connected(false),
										backlog(SOCKET_TCP_SERVER_INVALID_BACKLOG),
										addrError(0)
{
}

SocketTcpServer::SocketTcpServer(std::string const &port, int backlog):	connected(false),
																		backlog(SOCKET_TCP_SERVER_INVALID_BACKLOG),
																		addrError(0)
{

	this->connect(port, backlog);
}

SocketTcpServer::~SocketTcpServer(void)
{
	std::map<int, SocketTcpClient *>::iterator	cr;

	for (cr = this->clients.begin(); cr != this->clients.end(); cr++)
		delete cr->second;
	this->disconnect();
}

int				SocketTcpServer::getAddrs(struct addrinfo **res, std::string const &service)
{
	struct addrinfo	hints = {};
	int				error;

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	error = getaddrinfo(NULL, service.c_str(), &hints, res);
	return (error);
}

int				SocketTcpServer::connect(std::string const &port, int backlog)
{
	struct addrinfo				*addrs;
	struct addrinfo				*cr;
	int							fd;
	int							opt;

	this->addrError = this->getAddrs(&addrs, port);
	if (this->addrError)
		return (this->addrError);
	for (cr = addrs; cr; cr = cr->ai_next)
	{
		fd = ::socket(cr->ai_family, cr->ai_socktype, cr->ai_protocol);
		if (fd == INVALID_FD)
			continue;
		opt = 1;
		if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
			return (EXIT_ERRNO);
		if (!::bind(fd, cr->ai_addr, cr->ai_addrlen) && !listen(fd, backlog))
			break;
		::close(fd);
	}
	freeaddrinfo(addrs);
	if (!cr)
		return (EXIT_ERRNO);
	this->disconnect();
	this->fd = fd;
	this->backlog = backlog;
	this->connected = true;
	return (EXIT_SUCCESS);
}

int				SocketTcpServer::accept(SocketTcpClient const *&client)
{
	struct sockaddr_storage clientAddr;
	socklen_t				addrSize;
	int						clientSocket;

	addrSize = sizeof(clientAddr);
	clientSocket = ::accept(this->fd, (struct sockaddr *)&clientAddr, &addrSize);
	if (clientSocket == INVALID_FD)
		return (EXIT_ERRNO);
	this->clients[clientSocket] = new SocketTcpClient(clientSocket);
	client = this->clients[clientSocket];
	return (EXIT_SUCCESS);
}

int				SocketTcpServer::broadcast(std::string const &str, int flags)
{	
	int											error(0);
	std::map<int, SocketTcpClient *>::iterator	cr;

	for (cr = this->clients.begin(); cr != this->clients.end(); cr++)
	{
		if (cr->second->send(str, flags) < 0)
			error++;
	}
	return (error);
}

int				SocketTcpServer::broadcast(void const *buf, size_t len, int flags)
{
	int											error(0);
	std::map<int, SocketTcpClient *>::iterator	cr;

	for (cr = this->clients.begin(); cr != this->clients.end(); cr++)
	{
		if (cr->second->send(buf, len, flags) < 0)
			error++;
	}
	return (error);
}

void			SocketTcpServer::disconnect(void)
{
	if (this->isConnected())
	{
		this->close();
		this->connected = false;
		this->clients.clear();
	}
}

void			SocketTcpServer::disconnectClient(int clientSocket)
{
	this->clients.erase(clientSocket);
}

int				SocketTcpServer::getClient(SocketTcpClient const *&client, int clientSocket)
{
	if (this->clients.find(clientSocket) == this->clients.end())
		return (EXIT_FAILURE);
	client = this->clients[clientSocket];
	return (EXIT_SUCCESS);
}

bool			SocketTcpServer::isConnected(void) const
{
	return (this->connected);
}

int				SocketTcpServer::getAddrError(void) const
{
	return (this->addrError);
}

int				SocketTcpServer::getBacklog(void) const
{
	return (this->backlog);
}
