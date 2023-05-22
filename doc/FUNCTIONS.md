# syscall

## <unistd.h>
execve, dup, dup2, pipe, fork, close, read, write, access

## <sys/select.h>
select - synchronous I/O multiplexing

## <poll.h>
poll - wait for some event on a file descriptor

## <sys/epoll.h>
epoll - I/O event notification facility

The epoll API is Linux-specific.  Some other systems provide
similar mechanisms, for example, FreeBSD has kqueue, 
and Solaris has /dev/poll.

## <sys/socket.h>
socket - create an endpoint for communication
accept - accept a connection on a socket
listen - listen for connections on a socket
send - send a message on a socket
recv - receive a message from a socket
bind - bind a name to a socket
connect - initiate a connection on a socket
setsockopt - get and set options on sockets
getsockname - get socket name

## <fcntl.h>
fcntl - manipulate file descriptor

## <sys/wait.h>
waitpid

## <signal.h>
signal

## <sys/stat.h>
stat

# others

## <string.h>
strerror

## <netdb.h>
gai_strerror — address and name information error description
getaddrinfo, freeaddrinfo - network address and service translation
getprotobyname — network protocol database functions

## <errno.h>
errno - number of last error

## <arpa/inet.h>
htonl, htons, ntohl, ntohs - convert values between host and network byte order

## <dirent.h>
opendir, readdir, closedir