#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <arpa/inet.h>

int	id[FD_SETSIZE];
fd_set	curr_sock, cpy_read, cpy_write;
char	msg[42], str[60000000], tmp[60000000], buf[60000042];
int	g_id = 0;

void	fatal(void){
	write(2, "Fatal error\n", strlen("Fatal error\n"));
	close_all_fd();
	for (int fd = 3; fd < FD_SETSIZE; ++fd)
		if (id[fd] != -1)
			close(fd);
	exit(1);
}

void	send_all(int fd, char* s){
	for(int f = 0; f < FD_SETSIZE; ++f)
		if (f != fd && id[fd] != -1 && FD_ISSET(f, &cpy_write))
			if(send(f, s, strlen(s), 0) < 0)
				fatal();
}

void	add_client(void){
	int	clientsock;
	if((clientsock = accept(sockfd, NULL, NULL)) < 0)
		fatal();
	id[clientsock] = g_id++;
	FD_SET(clientsock, &curr_sock);
	bzero(&msg, sizeof(msg));
	sprintf(msg, "server: client %d just arrived\n", id[clientsock]);
	send_all(clientsock, msg);
}

void	rm_client(int fd){
	bzero(&msg, sizeof(msg));
	sprintf(msg, "server: client %d just left\n", id[fd]);
	FD_CLR(fd, &curr_sock);
	id[fd] = -1;
	close(fd);
}

void	send_msg(int fd){
	int	i = 0;
	int	j = 0;

	while(str[i]){
		tmp[j] = str[i];
		j++;
		if(str[i] == '\n'){
			bzero(&buf, sizeof(buf));
			sprintf(maxibuf, "client %d: %s", id[fd], tmp);
			send_all(fd, buf);
			j = 0;
			bzero(&tmp, sizeof(tmp));
		}
		i++;
	}
}

int	main(int ac, char** av){
	if (ac != 2){
		write(2, "Wrong number of arguments\n", strlen("Wrong number of arguments\n"));
		exit(1);
	}

	struct sockaddr_in	servaddr;
	uint16_t		port = atoi(av[1]);
	int			sockfd;

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(2130706433);//127.0.0.1
	servaddr.sin_port = htons(port);
I	
	for(int fd = 0; fd < FD_SETSIZE; ++fd)
		id[fd] = -1;

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		fatal();
	if(bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
		fatal();
	if(listen(sockfd, 0) < 0)
		fatal();

	FD_ZERO(&curr_sock);
	FD_SET(sockfd, &curr_sock);
	bzero(&str, sizeof(str));
	bzero(&tmp, sizeof(tmp));

	while(1){
		cpy_write = cpy_read = curr_sock;
		if(select(FD_SETSIZE, &cpy_read, &cpy_write, NULL, NULL) < 0)
			continue ;
		for(int fd = 0; fd < FD_SETSIZE; ++fd){
			if(FD_ISSET(fd, &cpy_read)){
				if(fd == sockfd){
					add_client();
					break ;
				} else {
					int	ret = 1000;
					while(ret == 1000 || str[strlen(str)]){
						ret = recv(fd, str + strlen(str), 1000, 0);
						if(ret <= 0)
							break ;
					}
					if(ret <= 0){
						rm_client();
						break ;
					} else 
						send_msg(fd);
				}
			}
		}
	}
}
