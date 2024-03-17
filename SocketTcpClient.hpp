/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketTcpClient.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nlaerema <nlaerema@student.42lehavre.fr>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/05 10:58:17 by nlaerema          #+#    #+#             */
/*   Updated: 2024/03/08 13:11:49 by nlaerema         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <netdb.h>
#include "Socket.hpp"
#include "SocketTcpServer.hpp"

#define	SOCKET_TCP_CLIENT_BUF_SIZE 512

class SocketTcpServer;

class SocketTcpClient: protected Socket
{
	protected:
		bool			connected;
		int				addrError;

		int				getAddrs(struct addrinfo **res,
							std::string const &node, std::string const &service) const;

	public:
						SocketTcpClient(void);
						SocketTcpClient(int socketConnected);
						SocketTcpClient(std::string const &addr, std::string const port);
						~SocketTcpClient(void);
		ssize_t			send(void const *buf, size_t len, int flags = 0) const;
		ssize_t			send(std::string const &str, int flags = 0) const;
		ssize_t			recv(void *buf, size_t len, int flags = 0) const;
		ssize_t			recv(std::string &str, int flags = 0) const;
		int				connect(int socketConnected);
		int				connect(std::string const &addr, std::string const &port);
		void			disconnect(void);
		bool			isConnected(void) const;;
		int				getAddrError(void) const;
		using			Socket::getFd;
		SocketTcpClient	const &operator<<(std::string const &str) const;
		std::string		&operator>>(std::string &str) const;
};
