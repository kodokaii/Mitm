/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mitm.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nlaerema <nlaerema@student.42lehavre.fr>	+#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/05 10:58:17 by nlaerema          #+#    #+#             */
/*   Updated: 2024/03/17 16:52:16 by nlaerema         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Mitm.hpp"

static int g_sigint(0);

void sigintHandler(int sig)
{
	if (sig == SIGINT)
	{
		std::cout << std::endl;
		g_sigint = 1;
	}
}

Mitm::Mitm(void) : epoll(INVALID_FD)
{
}

Mitm::Mitm(std::string const &addr, std::string const &port) : epoll(INVALID_FD)
{
	this->connect(addr, port);
}

Mitm::Mitm(std::string const &addr, std::string const &inPort, std::string const &outPort) : epoll(INVALID_FD)
{
	this->connect(addr, inPort, outPort);
}

Mitm::~Mitm(void)
{
	if (this->epoll != INVALID_FD)
		::close(epoll);
}

void Mitm::printLine(int clientSocket, std::string const &msg, bool out)
{
	std::string line;
	size_t lineStart;
	size_t lineEnd;

	line = this->lineBuf[clientSocket] + msg;
	lineStart = 0;
	lineEnd = line.find('\n');
	while (lineEnd != std::string::npos)
	{
		if (out)
			std::cout << COLOR_RED << clientSocket << "< " << COLOR_RESET;
		else
			std::cout << COLOR_GREEN << clientSocket << "> " << COLOR_RESET;
		std::cout << line.substr(lineStart, lineEnd - lineStart + 1);
		lineStart = lineEnd + 1;
		lineEnd = line.find('\n', lineStart);
	}
	this->lineBuf[clientSocket] = line.substr(lineStart);
}

int Mitm::loop(void)
{
	struct epoll_event events[this->TcpServer::getBacklog()];
	int readyFd;

	std::signal(SIGINT, sigintHandler);
	while (!g_sigint)
	{
		readyFd = epoll_wait(this->epoll, events, this->TcpServer::getBacklog(), -1);
		for (int i(0); i < readyFd; i++)
		{
			if (events[i].events & (EPOLLRDHUP | EPOLLHUP))
			{
				if (events[i].data.fd == this->TcpServer::getFd() || events[i].data.fd == this->TcpClient::getFd())
				{
					std::signal(SIGINT, SIG_DFL);
					std::cerr << "connection closed" << std::endl;
					return (EXIT_FAILURE);
				}
				else
					this->disconnectClient(events[i].data.fd);
			}
			else
			{
				if (events[i].data.fd == this->TcpServer::getFd())
					this->connectClient();
				else if (events[i].data.fd == this->TcpClient::getFd())
					this->receive_out(this->TcpClient::getFd());
				else
					this->receive_in(events[i].data.fd);
			}
		}
	}
	std::signal(SIGINT, SIG_DFL);
	return (EXIT_SUCCESS);
}

int Mitm::receive_in(int clientSocket)
{
	TcpClient const *client;
	std::string msg;

	if (this->TcpServer::getClient(client, clientSocket))
		return (EXIT_FAILURE);
	if (client->recv(msg, MSG_DONTWAIT) < 0)
		return (EXIT_FAILURE);
	this->printLine(clientSocket, msg, false);
	return (0 <= this->TcpClient::send(msg));
}

int Mitm::receive_out(int clientSocket)
{
	std::string msg;

	if (this->TcpClient::recv(msg, MSG_DONTWAIT) < 0)
		return (EXIT_FAILURE);
	this->printLine(clientSocket, msg, true);
	return (this->TcpServer::broadcast(msg));
}

int Mitm::connectClient(void)
{
	TcpClient const *newClient;
	struct epoll_event event = {};

	if (this->TcpServer::accept(newClient))
		return (perror("accept"), EXIT_ERRNO);
	event.events = EPOLLIN | EPOLLRDHUP;
	event.data.fd = newClient->getFd();
	if (epoll_ctl(this->epoll, EPOLL_CTL_ADD, newClient->getFd(), &event))
	{
		perror("epoll_ctl");
		this->TcpServer::disconnectClient(newClient->getFd());
		return (EXIT_ERRNO);
	}
	std::cout << "[" << newClient->getFd() << "]: connect" << std::endl;
	return (EXIT_SUCCESS);
}

int Mitm::disconnectClient(int clientSocket)
{
	this->TcpServer::disconnectClient(clientSocket);
	std::cout << "[" << clientSocket << "]: disconnect" << std::endl;
	return (epoll_ctl(this->epoll, EPOLL_CTL_DEL, clientSocket, NULL));
}

int Mitm::connect(std::string const &addr, std::string const &port)
{
	return (this->connect(addr, port, port));
}

int Mitm::connect(std::string const &addr, std::string const &inPort, std::string const &outPort)
{
	struct epoll_event event = {};
	int error;

	error = this->TcpClient::connect(addr, outPort);
	if (error)
	{
		if (error != EXIT_ERRNO)
			std::cout << "server ip error: " << gai_strerror(error) << std::endl;
		else
			perror("connection out failed");
		return (error);
	}
	error = this->TcpServer::connect(inPort);
	if (error)
	{
		if (error != EXIT_ERRNO)
			std::cout << "ip error: " << gai_strerror(error) << std::endl;
		else
			perror("connection in failed");
		this->TcpServer::disconnect();
		return (error);
	}
	this->epoll = epoll_create(this->TcpServer::getBacklog());
	if (this->epoll == INVALID_FD)
	{
		perror("epoll_create");
		this->TcpServer::disconnect();
		this->TcpClient::disconnect();
		return (EXIT_ERRNO);
	}
	event.events = EPOLLIN | EPOLLRDHUP;
	event.data.fd = this->TcpClient::getFd();
	if (epoll_ctl(this->epoll, EPOLL_CTL_ADD, this->TcpClient::getFd(), &event))
	{
		perror("epoll_ctl");
		this->TcpServer::disconnect();
		this->TcpClient::disconnect();
		::close(this->epoll);
		return (EXIT_ERRNO);
	}
	event.events = EPOLLIN | EPOLLRDHUP;
	event.data.fd = this->TcpServer::getFd();
	if (epoll_ctl(this->epoll, EPOLL_CTL_ADD, this->TcpServer::getFd(), &event))
	{
		perror("epoll_ctl");
		this->TcpServer::disconnect();
		this->TcpClient::disconnect();
		::close(this->epoll);
		return (EXIT_ERRNO);
	}
	return (EXIT_SUCCESS);
}

bool Mitm::isConnected(void)
{
	return (this->TcpClient::isConnected() && this->TcpServer::isConnected());
}

void Mitm::disconnect(void)
{
	this->TcpClient::disconnect();
	this->TcpServer::disconnect();
	if (this->epoll != INVALID_FD)
		::close(epoll);
}

int Mitm::getAddrError(void)
{
	if (this->TcpClient::getAddrError())
		return (this->TcpClient::getAddrError());
	return (this->TcpServer::getAddrError());
}

int main(int argc, char **argv)
{
	Mitm mitm;
	std::string addr;
	std::string inPort;
	std::string outPort;

	if (argc != 4)
	{
		std::cerr << "USAGE: " << argv[0] << " <target_host> <target_port> <listen_port>" << std::endl;
		return (EXIT_FAILURE);
	}

	addr = argv[1];
	outPort = argv[2];
	inPort = argv[3];
	if (mitm.connect(addr, inPort, outPort))
		return (EXIT_FAILURE);
	mitm.loop();
}
