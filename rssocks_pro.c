#include"rssocks_pro.h"

int create_rssocks_server(char *URL,int port,int usec){
    int cmd_sock, sendsize , readsize;
    char cmd_buff[MAX_CMD_PRO_LEN];
    struct rcsocktul *tunn;
    //rs_usec_time = usec;
    cmd_sock = proto_init_cmd_rcsocket(URL,port);
    if(cmd_sock == PROTO_INIT_CMD_RCSOCKET_ERROR){
        return -1;
    }
    printf("rssocks %s:%d <--[%4d usec]--> socks server\n",
    URL,port, usec );
    while(1){
        readsize = API_socket_recv(cmd_sock,cmd_buff,MAX_CMD_PRO_LEN);
        if(readsize == MAX_CMD_PRO_LEN &&
          cmd_buff[0] == PRO_MAKE_NET &&
          cmd_buff[1] == START_A_NEW_SOCK_TUNNEL_NOW){
            // read a pool number
            pthread_t thread_id;
            tunn = (struct rcsocktul *)malloc(sizeof(struct rcsocktul));
            strncpy(tunn->URL,URL,300);
            tunn->port = port;
            //printf("readsize = %d \n",readsize);
            tunn->poolnum = API_m_chartoi(&(cmd_buff[2]),4);
            //printf("after chartoi \n");
            if(pthread_create(&thread_id,NULL,
            socks_check_and_tunnel,(void *)tunn)<0)
            {
                puts("could not create one way tunnel\n");
                return 0;
            }
            // pthread_join(thread_id,NULL);
            usleep(1);
            tunn = NULL;
        }
        else{
            printf("may be sth wrong ~ %d reconnect now! \n", readsize);
            create_rssocks_server(URL,port,usec);
            return 1;
        }
        usleep(1);
    }
    printf("exit wihile ??????\n"); 
}
