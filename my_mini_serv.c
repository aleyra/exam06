#include <unistd.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct s_client
{
	int				fd;
	int				id;
	struct s_client	*next;
}	t_client;

t_client	*g_client = NULL;

void	fatal_error(void){
	write(2, "Fatal error\n", 12);
	exit(1);
}

int	main(int ac, char** av){
	if (ac != 2){
		write(2, "Wrong number of arguments\n", 26);
		exit(1);
	}
}