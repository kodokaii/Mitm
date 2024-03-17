/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketTcpServer.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nlaerema <nlaerema@student.42lehavre.fr>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/05 10:58:17 by nlaerema          #+#    #+#             */
/*   Updated: 2024/03/17 16:27:15 by nlaerema         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <netdb.h>
#include <map>
#include "Socket.hpp"
#include "SocketTcpClient.hpp"

#define SOCKET_TCP_SERVER_INVALID_BACKLOG -1
#define SOCKET_TCP_SERVER_DEFAULT_BACKLOG 128

class SocketTcpClient;

class SocketTcpServer: protected Socket
{
	protected:
		bool							connected;
		int								backlog;
		int								addrError;
		std::map<int, SocketTcpClient *>	clients;

		int				getAddrs(struct addrinfo **res, std::string const &port);

	public:
						SocketTcpServer(void);
						SocketTcpServer(std::string const &port, int backlog = SOCKET_TCP_SERVER_DEFAULT_BACKLOG);
						~SocketTcpServer(void);
		int				connect(std::string const &port, int backlog = SOCKET_TCP_SERVER_DEFAULT_BACKLOG);
		int				accept(SocketTcpClient const *&client);
		int				broadcast(std::string const &str, int flags = 0);
		int				broadcast(void const *buf, size_t len, int flags = 0);
		void			disconnect(void);
		void			disconnectClient(int clientSocket);
		int				getClient(SocketTcpClient const *&client, int clientSocket);
		bool			isConnected(void) const;
		int				getAddrError(void) const;
		int				getBacklog(void) const;
		using			Socket::getFd;
};
