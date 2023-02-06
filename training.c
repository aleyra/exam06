#include <unistd.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <strings.h>
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>

typedef struct s_client{
	int	fd;
	int	id;
	struct s_client	*next;
} t_client;

t_client	*g_client = NULL;
int		sock_fd, g_id = 0;
fd_set		curr_sock, cpy_write, cpy_read;
char		msg[42], tmp[4 * 4096], str[4 * 4096], buf[4 * 4096 +42];


void fatal_error(void){
	write(2, "Fatal error\n", strlen("Fatal error\n");
	close(sock_fd);
	exit(1);
}

int	get_id(int fd){
	t_client	*t_cli = g_client;
	while (t_cli){
		if(t_cli->fd == fd){
			return (t_cli->id)
		}
		t_cli = t_cli->next;
	}
	return (-1);
}

int	get_max_fd(void){
	t_client	*t_cli = g_client;
	int		max = sock_fd;
	while (t_cli){
		if (t_cli->fd -> max)
			max = t_cli->fd;
		t_cli = t_cli->next;
	}
	return (max);
}

int	add_client(int fd){
	t_client	*t_cli = g_client;
	t_client	*new;

	if(!(new = calloc(1, sizeof(t_client)))){
		fatal_error();
	}
	new->fd = fd;
	new->id = g_id++;
	new->next = NULL;
	if (g_client == NULL){
		g_client = new;
	} else {
		while(t_cli && t_cli->next != NULL){
			t_cli = t_cli->next;
		}
		t_cli->next = new;
	}
	return(new->id);
}

int	rm_client(int fd){
	t_client	*t_cli = g_client;
	t_client	*del;
	int		cli_id = -1;
	if (t_cli->fd == fd){
		del = t_cli;
		cli_id = del->id;
		g_client = g_client->next;
		free (del);	
	} else {
		while (t_cli && t_cli->next && t_cli->next->fd != fd){
			t_cli = t_cli->next;
		del = t_cli->next;
		cli_id = del->fd;
		t_cli->next = t_cli->next->next;
		free(del);
	}
	return (cli_id);
}

void	accept_client(void){
	struct sockaddr_in	clientaddr;
	socklen_t	len = sizeof(clientaddr);
	int		cli_fd;

	if ((cli_fd = accept(fd, (struct sockaddr *)&clientaddr, &len)) < 0){
		fatal_error();
	}
	sprintf(msg, "server: client %d just arrived\n", add_client(cli_fd));
	send_all(fd, msg);
	FD_SET(cli_fd, &curr_sock);
}

void	send_all(int fd, char* msg){
	t_client	*tcli = g_client;

	while (t_cli != NULL){
		if (t_cli->fd != fd){
			if (send(t_cli->fd, msg, strlen(msg), 0) < 0){
				fatal_error();
			}
		} 
		t_cli = t_cli->next;
	}
}

void	send_msg(int fd){
	int i = 0;
	int j = 0;

	while (str[i]){
		tmp[j] = str[i];
		j++;
		if (str[i] == '\n'){
			sprintf(buf, "client %d: %s", get_id(fd), tmp);
			send_all(fd, buf);
			bzero(&buf, sizeof(buf));
			j = 0;
			bzero(&tmp, sizeof(tmp));
		}
		i++
	}
	bzero(&str, sizeof(str));
}

int	main(int ac, char** av){
	if (ac != 2){
		write(2, "Wrong number of arguments\n", strlen("Wrong number of arguments\n");
				exit(1);
	}
	
	struct sockaddr_in servaddr;
	uint16_t	port = atoi(av[1]);

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(2130706433); //127.0.0.1
	servaddr.sin_port = htons(port);

	bzero(&servaddr, sizeof(servaddr)); 
	FD_ZERO(&curr_sock);

	if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0) < 0)
		fatal_error();
	if ((bind(sock_fd, (const struct sockaddr *)&servaddr, sizeof(servaddr))) != 0)
       		fatal_error();
	if (listen(sock_fd, 0) != 0)
		fatal_error();

	bzero(&str, sizeof(str));
	bzero(&buf, sizeof(buf));
	bzero(&tmp, sizeof(tmp));
	FD_SET(sock_fd, &curr_sock);

	while(1){
		cpy_write = cpy_read = curr_sock;
		if (select(get_max_fd() + 1, &cpy_read, &cpy_write, NULL, NULL) < 0)
			continue ;
		for(int fd = 0; fd <= get_max_fd(); fd++){
			if (FD_ISSET(fd, &cpy_read)){
				if(fd == sock_fd){
					bzero(&msg, sizeof(msg));
					accept_client();
					break ;
				} else {
					int ret_recv = 1000;
					while (ret_recv == 1000 || str[strlen(str) - 1] == '\n'){
						ret_recv = recv(fd, str + strlen(str), 1000, 0);
						if (ret_recv < 0){
							break ;
						}
					}
					if (ret_recv < 0){
						bzero(&msg, sizeof(msg));	
						sprintf(msg, "server: client %d just left\n", rm_client(fd));
						send_all(fd, msg);
						FD_CLR(fd, &curr_sock);
						close(fd);
						break ;
					} else {
						send_msg(fd);
					}
				}
			}
		}
	}

	return (0);
}
