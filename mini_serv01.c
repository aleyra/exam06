#include <unistd.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <strings.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>

typedef struct s_client{
	int fd;
	int id;
	struct s_client *next;
} t_client;

t_client *g_cli = NULL;
int sock_fd, g_id = 0;
char msg[42], tmp[4 * 4096], str[4 * 4096], buf[4 * 4096 + 42];
fd_set	cpy_write, cpy_read, curr_sock;

void	fatal_error(void){
	write(2, "Fatal error\n", strlen("Fatal error\n"));
	close(sock_fd);
	exit(1);
}

int	get_id(int fd){
	t_client	*t_cli = g_cli;

	while(t_cli){
		if (t_cli->fd == fd)
			return (t_cli->id);
		t_cli = t_cli->next;
	}
	return (-1);
}

int	get_max_fd(void){
	int	max = sock_fd;
	t_client	*t_cli = g_cli;
	while (t_cli){
		if(t_cli->fd > max)
			max = t_cli->fd;
		t_cli = t_cli->next;
	}
	return(max);
}

int	add_client(int fd){
	t_client	*t_cli = g_cli;
	t_client	*new;
	if(!(new = calloc(1, sizeof(t_client))))
		fatal_error();
	new->id = g_id++;
	new->fd = fd;
	new->next = NULL;
	if(!g_cli){
		g_cli = new;
	} else {
		while (t_cli->next)
			t_cli = t_cli->next;
		t_cli->next = new;
	}
	return(new->id);
}

int	rm_client(int fd){
	t_client	*t_cli = g_cli;
	t_client	*del;
	int	del_id = get_id(fd);
	if(g_cli && g_cli->fd == fd){
		g_cli = g_cli->next;
		free(t_cli);
	} else {
		while(t_cli && t_cli->next && t_cli->next->fd != fd)
			t_cli = t_cli->next;
		if (t_cli->next->fd == fd){
			del = t_cli->next;
			t_cli->next = t_cli->next->next;
			free(del);
		}
	}
	return(del_id);
}

void send_all(int fd, char* s){
	t_client	*t_cli = g_cli;

	while(t_cli){
		if (t_cli->fd != fd && FD_ISSET(t_cli->fd, &cpy_write)){
			if ((send(t_cli->fd, s, strlen(s), 0)) < 0)
				fatal_error();
		}
		t_cli = t_cli->next;
	}
}

void accept_client(void){
	int	cli_fd;
	struct sockaddr_in clientaddr;
	socklen_t len = sizeof(clientaddr);
	if((cli_fd = accept(sock_fd, (struct sockaddr *)&clientaddr, &len)) < 0)
		fatal_error();
	sprintf(msg, "server: client %d just arrived\n", add_client(cli_fd));
	send_all(cli_fd, msg);
	FD_SET(cli_fd, &curr_sock);
}

void send_msg(int fd){
	int i = 0;
	int j = 0;
	while (str[i]){
		tmp[j] = str[i];
		j++;
		if (str[i] == '\n'){
			sprintf(buf, "client %d: %s", get_id(fd), tmp);
			send_all(fd, buf);
			j = 0;
			bzero(&tmp, sizeof(tmp));
			bzero(&buf, sizeof(buf));
		}
		i++;
	}
	bzero(&str, sizeof(str));
}

int	main(int ac, char** av){
	if (ac != 2){
		write(2, "Wrong number of arguments\n", strlen("Wrong number of arguments\n"));
		exit(1);
	}
	uint16_t	port = atoi(av[1]);
	struct sockaddr_in servaddr;

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(2130706433); //127.0.0.1
	servaddr.sin_port = htons(port);

	if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		fatal_error();
	if ((bind(sock_fd, (const struct sockaddr *)&servaddr, sizeof(servaddr))) < 0)
		fatal_error();
	if ((listen(sock_fd, 0)) < 0)
		fatal_error();

	FD_ZERO(&curr_sock);
	FD_SET(sock_fd, &curr_sock);
	bzero(&buf, sizeof(buf));
	bzero(&tmp, sizeof(tmp));
	bzero(&str, sizeof(str));

	while(1){
		cpy_write = cpy_read = curr_sock;
		if ((select(get_max_fd() + 1, &cpy_read, &cpy_write, NULL, NULL)) < 0)
			continue ;
		for(int fd = 0; fd < get_max_fd(); fd++){
			if (fd == sock_fd){
				bzero(&msg, sizeof(msg));
				accept_client();
				break ;
			} else {
				if (FD_ISSET(fd, &cpy_read)){
					int ret_recv = 1000;
					while (ret_recv == 1000 || str[strlen(str) - 1] != '\n'){
						ret_recv = recv(fd, str + strlen(str), 1000, 0);
						if (ret_recv < 0)
							break ;
					}
					if (ret_recv < 0){
						bzero(&msg, sizeof(msg));
						sprintf(msg, "server: client %d just left\n", rm_client(fd));
						send_all(fd, msg);
						FD_CLR(fd, &curr_sock);
						close(fd);
						break ;
					} else
						send_msg(fd);
				}
			}
		}
	}
	return (0);
}
