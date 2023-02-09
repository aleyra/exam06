void	fatal_error(void){}

int	rm_client(int fd){}

void	accept_client(void){}

void	send_all(int fd, char *ze_msg){}

void	send_msg(int fd){}

int	main(int ac, char** av){
	//
	while(1){
		//
		for (int fd = 0; fd <= get_max_fd(); fd++){
			if (FD_ISSET(fd, &cpy_read)){
				if (fd == sock_fd){
					//
				} else {
					//
					while(ret_recv == 1000 || str[strlen(str) - 1] != '\n'){
						//
						if (ret_recv <= 0)
							//
					}
					if(ret_recv <= 0){
						//
					} else {
						//
					}
				}
			}
		}
	}
	return (0);
}

