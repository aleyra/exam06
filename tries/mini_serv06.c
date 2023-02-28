#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <sys/types.h>
#include <arpa/inet.h>

int	sockfd, g_id = 0;
int	id[1024];
char	msg[42], str[42 * 4096], tmp[42 * 4096], buf[42 * 4096 + 42];
fd_set	set, cpy_read, cpy_write;

void	fatal(void){
	write(2, "Fatal error\n", strlen("Fatal error\n"));
	for(int fd = 3; fd < 1024; ++fd)
		if (id[fd] != -1)
			close(fd);
	exit(1);
}

void	send_all(int e, char* s){
	for(int fd = 0; fd < 1024; ++fd)
		if (fd != e && id[fd] != -1 && FD_ISSET(fd, &cpy_write))
			if(send(fd, s, strlen(s), 0) < 0)
				fatal();
}

void	add_client(void){
	int	new;

	if ((new = accept(sockfd, NULL, NULL)) < 0)
		fatal();
	id[new] = g_id++;
	FD_SET(new, &set);
	bzero(&msg, sizeof(msg));
	sprintf(msg, "server: client %d just arrived\n", id[new]);
	send_all(new, msg);
}

void	rm_client(int fd){
	bzero(&msg, sizeof(msg));
	sprintf(msg, "server: client %d just left\n", id[fd]);
	send_all(fd, msg);
	FD_CLR(fd, &set);
	close(fd);
	id[fd] = -1;
}

void	send_msg(int fd){
	int	i = 0;
	int	j = 0;

	while(str[i]){
		tmp[j] = str[i];
		j++;
		if(str[i] == '\n'){
			bzero(&buf, sizeof(buf));
			sprintf(buf, "client %d: %s", id[fd], tmp);
			send_all(fd, buf);
			j = 0;
			bzero(&tmp, sizeof(tmp));
		}
		i++;
	}
	bzero(&str,sizeof(str));
}

int	main(int ac, char** av){
	if (ac != 2){
		write(2, "Wrong number of arguments\n", strlen("Wrong number of arguments\n"));
		exit(1);
	}

	struct sockaddr_in servaddr;
	uint16_t	port = atoi(av[1]);
	
	for(int fd = 0; fd < 1024; ++fd)
		id[fd] = -1;
	
	bzero(&servaddr, sizeof(servaddr));
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		fatal();
	if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) != 0)
		fatal();
	if (listen(sockfd, 128) != 0)
		fatal();

	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl(2130706433); //127.0.0.1
	servaddr.sin_port = htons(port);

	FD_ZERO(&set);
	FD_SET(sockfd, &set);
	bzero(&str, sizeof(str));
	bzero(&tmp, sizeof(tmp));

	while(1){
		cpy_read = cpy_write = set;
		if (select(1024, &cpy_read, &cpy_write, NULL, NULL) <= 0)
			continue ;
		for(int fd = 0; fd < 1024; ++fd){
			if (FD_ISSET(fd, &cpy_read)){
				if(fd == sockfd){
					add_client();
					break ;
				} else {
					int	ret = 1;

					while(ret == 1 && str[strlen(str) - 1] != '\n')
						ret = recv(fd, str + strlen(str), 1, 0);
					if (ret <= 0){
						rm_client(fd);
						break ;
					} else
						send_msg(fd);
				}
			}
		}
	}

	return 0;
}
