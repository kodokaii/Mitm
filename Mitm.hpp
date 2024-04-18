/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mitm.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nlaerema <nlaerema@student.42lehavre.fr>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/05 10:58:17 by nlaerema          #+#    #+#             */
/*   Updated: 2024/03/17 23:10:07 by nlaerema         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <sys/epoll.h>
#include "kdolib/kdolib.hpp"

class Mitm: protected TcpClient, protected TcpServer
{
	private:
		std::map<int, std::string>	lineBuf;
		int							epoll;

		void	printLine(int clientSocket, std::string const &msg, bool out);

	public:
				Mitm(void);
				Mitm(std::string const &addr, std::string const &port);
				Mitm(std::string const &addr, std::string const &inPort, std::string const &outPort);
				~Mitm(void);
		int		loop(void);
		int		receive_in(int clientSocket);
		int		receive_out(int clientSocket);
		int		connectClient(void);
		int		disconnectClient(int clientSocket);
		int		connect(std::string const &addr, std::string const &port);
		int		connect(std::string const &addr, std::string const &inPort, std::string const &outPort);
		bool	isConnected(void);
		void	disconnect(void);
		int		getAddrError(void);
};
