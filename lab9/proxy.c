/*
 * proxy.c - ICS Web proxy
 *
 *
 */

#include "csapp.h"
#include <stdarg.h>
#include <sys/select.h>

/*
 * Function prototypes
 */
int parse_uri(char *uri, char *target_addr, char *path, char *port);
void format_log_entry(char *logstring, struct sockaddr_in *sockaddr, char *uri, size_t size);

ssize_t Rio_readnb_w(rio_t* rp, void* usrbuf, size_t n)
{
    ssize_t readnb;
    if((readnb = rio_readnb(rp, usrbuf, n))<0){
        readnb = 0;
        printf(stderr, "Rio_readnb_w error: %s\n", strerror(errno));
    }
    return readnb;
}

ssize_t Rio_readlineb_w(rio_t* rp, void* usrbuf, size_t maxlen)
{
   ssize_t readlineb;
    if((readlineb = rio_readlineb(rp, usrbuf, maxlen))<0){
        readlineb = 0;
        printf(stderr, "Rio_readlineb_w error: %s\n", strerror(errno));
    }
    return readlineb;
}

void Rio_writen_w(int fd, void* usrbuf, size_t n)
{
    if(n != rio_writen(fd, usrbuf, n)){
        printf(stderr, "Rio_writen_w error: %s\n", strerror(errno));
    }
}


typedef struct threadArgs{
    int connfd;
    struct sockaddr_storage clientaddr;
} threadArgs_t;

sem_t mutex;

void *thread(void* vargp)
{
    threadArgs_t* targs = (threadArgs_t*)vargp;
    struct sockaddr_storage clientaddr = targs->clientaddr;
    int connfd = targs->connfd;  
    Free(targs);

    rio_t client_rio;
    char buf[MAXLINE];
    char method[MAXLINE], uri[MAXLINE], version[MAXLINE];
    char hostname[MAXLINE], pathname[MAXLINE], port[MAXLINE];

    Rio_readinitb(&client_rio, connfd);
    if(Rio_readlineb_w(&client_rio, buf, MAXLINE) == 0){
        Close(connfd);
        return NULL;;
    }
    if(sscanf(buf, "%s %s %s", method, uri, version) != 3){
        printf(stderr, "Request format error\n");
        Close(connfd);
        return NULL;;
    }
    if(parse_uri(uri, hostname, pathname, port) == -1){
        printf(stderr, "Parse_uri error\n");
        Close(connfd);
        return NULL;;
    }
 
    int clientfd;
    rio_t  server_rio;
    if((clientfd = open_clientfd(hostname, port)) == -1){
        printf(stderr, "Server connection error\n");
        Close(connfd);
        return NULL;;
    }

    char  request[MAXLINE];
    Rio_readinitb(&server_rio, clientfd);
    sprintf(request, "%s%s%s%s%s%s", method, " /", pathname, " ", version, "\r\n");
    Rio_writen_w(clientfd, request, strlen(request));

    // read request header from client and send to server
    size_t n;
    int contentLength = 0;
    while((n = Rio_readlineb_w(&client_rio, buf, MAXLINE)) != 0)
    {
        Rio_writen_w(clientfd, buf, n);
        //empty line terminates headers
        if(!strcmp("\r\n", buf)){
            break;
        }
        if(!strncmp("Content-Length: ", buf, 16)){
            sscanf(buf+16, "%d", &contentLength);
        }
    }

    // reed request body if method is post
        if(strcmp(method, "GET")) {
        if (contentLength == 0) {
            Close(connfd);
            Close(clientfd);
            return NULL;
        }
        while(contentLength > 0) {
            if(Rio_readnb_w(&client_rio, buf, 1)) {
                contentLength--;
                Rio_writen_w(clientfd, buf, 1);
            }
            else break;
        }
        if(contentLength > 0) {
            printf(stderr, "Wrong request body\n");
            Close(connfd);
            Close(clientfd);
            return NULL;
        }
    }

    int receiveSize = 0;
    // read response header from server and send to client
    while( (n = Rio_readlineb_w(&server_rio, buf, MAXLINE)) != 0){
        Rio_writen_w(connfd, buf, n);
        receiveSize += n;
        if(!strcmp("\r\n", buf)){
            break;
        }
        if(!strncmp("Content-Length: ", buf, 16)){
            sscanf(buf+16, "%d", &contentLength);
        }
    }

    //read response body
    while(contentLength > 0) {
        if (Rio_readnb_w(&server_rio, buf, 1)) {
            contentLength--;
            receiveSize++;
            Rio_writen_w(connfd, buf, 1);
        }
        else break;
    }
    if(contentLength > 0) {
        printf(stderr, "Wrong response body\n");
        Close(connfd);
        Close(clientfd);
        return NULL;
    }


    char logstring[MAXLINE];
    format_log_entry(logstring, &clientaddr, uri, receiveSize);

    P(&mutex);
    printf("%s\n", logstring);
    V(&mutex);

    Close(connfd);
    Close(clientfd);
    return NULL;;
}


/*
 * main - Main routine for the proxy program
 */
int main(int argc, char **argv)
{

    int listenfd;
    socklen_t clientlen = sizeof(struct sockaddr_storage);
    pthread_t tid;
    threadArgs_t* targs;

    /* Check arguments */
    if (argc != 2) {
        printf(stderr, "Usage: %s <port number>\n", argv[0]);
        exit(0);
    }

    Sem_init(&mutex, 0, 1);
    Signal(SIGPIPE, SIG_IGN);
    listenfd = Open_listenfd(argv[1]);
    
    while(1)
    {
        targs = Malloc(sizeof(threadArgs_t));
        targs->connfd = Accept(listenfd, (SA *)(&(targs->clientaddr)), &clientlen);
        Pthread_create(&tid, NULL, thread, targs);
    }

    Close(listenfd);
    exit(0);
}


   
/*
 * parse_uri - URI parser
 *
 * Given a URI from an HTTP proxy GET request (i.e., a URL), extract
 * the host name, path name, and port.  The memory for hostname and
 * pathname must already be allocated and should be at least MAXLINE
 * bytes. Return -1 if there are any problems.
 */
int parse_uri(char *uri, char *hostname, char *pathname, char *port)
{
    char *hostbegin;
    char *hostend;
    char *pathbegin;
    int len;

    if (strncasecmp(uri, "http://", 7) != 0) {
        hostname[0] = '\0';
        return -1;
    }

    /* Extract the host name */
    hostbegin = uri + 7;
    hostend = strpbrk(hostbegin, " :/\r\n\0");
    if (hostend == NULL)
        return -1;
    len = hostend - hostbegin;
    strncpy(hostname, hostbegin, len);
    hostname[len] = '\0';

    /* Extract the port number */
    if (*hostend == ':') {
        char *p = hostend + 1;
        while (isdigit(*p))
            *port++ = *p++;
        *port = '\0';
    } else {
        strcpy(port, "80");
    }

    /* Extract the path */
    pathbegin = strchr(hostbegin, '/');
    if (pathbegin == NULL) {
        pathname[0] = '\0';
    }
    else {
        pathbegin++;
        strcpy(pathname, pathbegin);
    }

    return 0;
}

/*
 * format_log_entry - Create a formatted log entry in logstring.
 *
 * The inputs are the socket address of the requesting client
 * (sockaddr), the URI from the request (uri), the number of bytes
 * from the server (size).
 */
void format_log_entry(char *logstring, struct sockaddr_in *sockaddr,
                      char *uri, size_t size)
{
    time_t now;
    char time_str[MAXLINE];
    char host[INET_ADDRSTRLEN];
    
    /* Get a formatted time string */
    now = time(NULL);
    strftime(time_str, MAXLINE, "%a %d %b %Y %H:%M:%S %Z", localtime(&now));


    if (inet_ntop(AF_INET, &sockaddr->sin_addr, host, sizeof(host)) == NULL)
        unix_error("Convert sockaddr_storage to string representation failed\n");

    /* Return the formatted log entry string */
    sprintf(logstring, "%s: %s %s %zu", time_str, host, uri, size);
}
