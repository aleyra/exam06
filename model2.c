#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <arpa/inet.h>

int fid[FD_SETSIZE];
fd_set read_set, read_copy, write_set, write_copy;
const size_t BUFSIZE = 60000000;
char maxibuf[BUFSIZE];
int indexclient = 0;
int oneprintclient[FD_SETSIZE];//sert a rien

void error_display(char * str)
{
	if (str)
		write(2, str, strlen(str));
	exit (1);
}

void close_all_fd()
{
	for (int fd = 3; fd < FD_SETSIZE; ++fd)
		if (fid[fd] != -1)
			close(fd);
	error_display("Fatal error\n");
}

void send_buf(int sock, char *buf, size_t len)
{
	ssize_t ret;
	while ( (ret = send(sock, buf, len, 0)) > 0)
	{
		len = len - ret;
		buf = buf + ret;
	}
	if (ret == -1)
		close_all_fd();
}

void send_to_all(int sock, char *buf, size_t len)
{
	for(int fd = 0; fd < FD_SETSIZE; ++fd)
		if (fd != sock && fid[fd] != -1 && FD_ISSET(fd, &write_copy))
			send_buf(fd, buf, len);
}

int listen_and_send(int emiter_sock)
{
	char c;
	ssize_t ret;
	
	ret = recv(emiter_sock, &c, 1, 0);
	if (ret < 1)
		return -1;
		
	size_t len = sprintf(maxibuf, "client %d: ", fid[emiter_sock]);
	
	while( ret == 1)
	{
		len = sprintf(maxibuf, "%s%c", maxibuf, c);
		if (c == '\n')
			break;
		ret = recv(emiter_sock, &c, 1, 0);
	}
	if (ret == -1)
		close_all_fd();
	send_to_all(emiter_sock, maxibuf, len);
	return 0;
}

int main (int ac, char **av)
{
	if (ac != 2)
		error_display("Wrong number of arguments\n");
	
	for (int i = 0; i < FD_SETSIZE; ++i)
	{
		fid[i] = -1;
		oneprintclient[i] = 0;//sert a rien
	}
	
	int mastersock;
	struct sockaddr_in servaddr;

	mastersock = socket(AF_INET, SOCK_STREAM, 0);
	if (mastersock < 0)
		error_display("Fatal error\n");

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(2130706433);
	servaddr.sin_port = htons(atoi(av[1]));

	if ((bind(mastersock, (const struct sockaddr *)&servaddr, sizeof(servaddr))) < 0)
		error_display("Fatal error\n");

	if (listen(mastersock, 128) < 0)
		error_display("Fatal error\n");

	FD_ZERO(&read_set);
	FD_ZERO(&write_set);
	FD_SET(mastersock, &read_set);

	size_t len;
	
	while (1)
	{
		read_copy = read_set;
		write_copy = write_set;

		if (select(FD_SETSIZE, &read_copy, &write_copy, 0, 0) < 0)
			close_all_fd();
		for (int fd = 0; fd < FD_SETSIZE; ++fd)
		{
			if (FD_ISSET(fd, &read_copy))
			{
				if (fd == mastersock)
				{
					int clientsock;
					clientsock = accept(mastersock, NULL, NULL);
					if (clientsock < 0)
						close_all_fd();
					fid[clientsock] = indexclient;
					indexclient++;
					FD_SET(clientsock, &read_set);
					FD_SET(clientsock, &write_set);
					len = sprintf(maxibuf, "server: client %d just arrived\n", fid[clientsock]);
					send_to_all(clientsock, maxibuf, len);
				}
				else
				{
					int ret;
					ret = listen_and_send(fd);
					if (ret == -1)
					{
						len = sprintf(maxibuf, "server: client %d just left\n", fid[fd]);
						send_to_all(fd, maxibuf, len);
						FD_CLR(fd, &read_set);
						FD_CLR(fd, &write_set);
						fid[fd] = -1;
						close(fd);
					}
				}
			}
		}
	}
}