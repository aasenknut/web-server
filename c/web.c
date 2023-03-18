#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080
#define CONN_QUEUE_SIZE 64
#define BUFFER_SIZE 1024

int main() {
    char buf[BUFFER_SIZE];
    char resp[] = "HTTP/2.0 200 OK\r\n"
    "Content-type: text/html\r\n\r\n"
    "<html>message recieved</html>\r\n";
    size_t resp_size = sizeof(resp);
    // Socket
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd == -1) {
        perror("socket");
        return 1;
    }
    printf("socket created\n");

    struct sockaddr_in addr_srv;
    int addr_srv_len = sizeof(addr_srv);
    addr_srv.sin_port = htons(PORT);
    addr_srv.sin_addr.s_addr = htons(INADDR_ANY);
    addr_srv.sin_family = AF_INET;

    struct sockaddr_in addr_client;
    int addr_client_len = sizeof(addr_client);
 
    // Bind
    if (bind(sock_fd, (struct sockaddr *)&addr_srv, addr_srv_len) != 0) {
        perror("bind");
        return 1;
    }
    printf("bound\n");

    // Listen
    if (listen(sock_fd, CONN_QUEUE_SIZE) != 0) {
        perror("listen\n");
        return 1;
    }
    printf("listening\n");

    // Accept
    while (1) {
        int accept_sock_fd = accept(sock_fd, (struct sockaddr *)&addr_srv, (socklen_t *)&addr_srv_len);
        if (accept_sock_fd < 0) {
            perror("accept");
            continue;
        }
        printf("accepted connection\n");


        int client_sockn = getsockname(accept_sock_fd, (struct sockaddr *)&addr_client, (socklen_t *)&addr_client_len);
        if (client_sockn < 0) {
            perror("client socketname\n");
            continue;
        }
        printf("%s:%u\n", inet_ntoa(addr_client.sin_addr), ntohs(addr_client.sin_port));

        // Read
        ssize_t req = read(accept_sock_fd, buf, BUFFER_SIZE);
        if (req < 0) {
            perror("read\n");
            continue;
        }



        ssize_t wr = write(accept_sock_fd, resp, resp_size);
        if (wr < 0) {
            perror("write\n");
            continue;
        }

        // Close
        close(accept_sock_fd);
    }
    return 0;
}
