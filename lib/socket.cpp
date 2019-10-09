SOCKET socket()
{
    BOOL NonBlockingFg = TRUE;
    SOCKET fd = socket(AF_INET, SOCK_STREAM, 0);
    ioctlsocket(fd, FIONBIO, (DWORD *)&NonBlockingFg);

    return fd;
}

int set_nonblock_socket(SOCKET *fd)
{
    BOOL NonBlockingFg = TRUE;
    return ioctlsocket(*fd, FIONBIO, (DWORD *)&NonBlockingFg);
}

int _bind(SOCKET *fd, int port)
{
    SOCKADDR_IN name;
    name.sin_addr.s_addr = htonl(INADDR_ANY);
    name.sin_port = htons(port);
    name.sin_family = AF_INET;

    return bind(*fd, (SOCKADDR *)&name, sizeof(name));
}

int _listen(SOCKET *fd, const int backlog)
{
    return listen(*fd, backlog);
}

SOCKADDR_IN get_name(const char *name)
{
    struct hostent *myent = NULL;
    struct in_addr myen;
    int *add;

    SOCKADDR_IN info = {
        0,
    };

    myent = (hostent *)gethostbyname(name);
    if (myent == NULL)
    {
        return info;
    }
    else
    {
        while (*myent->h_addr_list != NULL)
        {
            add = (int *)*myent->h_addr_list;
            myen.s_addr = *add;

            info.sin_addr = myen;

            myent->h_addr_list++;
        }
    }

    return info;
}

SOCKET accept(int openSocket)
{
    SOCKADDR_IN name;

    int size = sizeof(name);
    return accept(openSocket, (SOCKADDR *)&name, &size);
}

int connect(SOCKET fd, const char *ip, const char *port)
{
    SOCKADDR_IN info = {
        0,
    };

    if (inet_addr(ip) != -1)
    {
        info.sin_addr.s_addr = inet_addr(ip);
    }
    else
    {
        info = get_name(ip);
    }

    info.sin_family = AF_INET;
    info.sin_port = htons(atoi(port));

    if (connect(fd, (SOCKADDR *)&info, sizeof(info)) == SOCKET_ERROR)
    {
        return SOCKET_ERROR;
    }
    else
    {
        return 0;
    }
}

int socket_read(SOCKET hSocket, LPVOID Buff, int ReqSize)
{
    int AccAddBytes = 0, RecvBytes;

    for (;;)
    {
        if ((RecvBytes = recv(hSocket, (LPSTR)Buff + AccAddBytes, ReqSize - AccAddBytes, 0)) > 0)
        {
            AccAddBytes += RecvBytes;
            if (AccAddBytes >= ReqSize)
                return 0;
        }
        else
        {
            int r = WSAGetLastError();

            if (r != WSAEWOULDBLOCK)
            {
                AccAddBytes = r;
                break;
            }
            Sleep(1);
        }
    }

    return AccAddBytes;
}

int socket_send(SOCKET hSocket, LPCVOID buffer, int ReqSize)
{
    int AccAddBytes = 0, SendBytes;

    for (;;)
    {
        if ((SendBytes = send(hSocket, (LPSTR)buffer + AccAddBytes, ReqSize - AccAddBytes, 0)) > 0)
        {
            AccAddBytes += SendBytes;
            if (AccAddBytes >= ReqSize)
                return 0;
        }
        else
        {
            int r = WSAGetLastError();

            if (r != WSAEWOULDBLOCK)
            {
                AccAddBytes = r;
                break;
            }
            Sleep(1);
        }
    }

    return AccAddBytes;
}

char *GetSocketByName(SOCKET fd)
{
    SOCKADDR_IN name;
    int size = sizeof(name);

    getsockname(fd, (SOCKADDR *)&name, &size);

    return inet_ntoa(name.sin_addr);
}

SOCKET open(int port, int backlog)
{
    SOCKET fd = socket();

    if (_bind(&fd, port) == SOCKET_ERROR)
    {
        return -1;
    }

    if (_listen(&fd, backlog) == SOCKET_ERROR)
    {
        return -1;
    }

    return fd;
}

SOCKET permit(SOCKET fd)
{
    return accept(fd);
}