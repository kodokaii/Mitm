/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nlaerema <nlaerema@student.42lehavre.fr>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/05 10:58:17 by nlaerema          #+#    #+#             */
/*   Updated: 2024/03/08 10:51:16 by nlaerema         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "kdolib/kdolib.hpp"
#include <sys/socket.h>

class Socket
{
	protected:
		int		fd;

	public:
				Socket(void);
				Socket(int fd);
				Socket(int domain, int type, int protocol = 0);
				~Socket(void);
		int		open(int domain, int type, int protocol = 0);
		bool	is_open(void) const;
		void	close(void);
		int		getFd(void) const;
		void	setFd(int fd);
};
