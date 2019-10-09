
#include <malloc.h> //realloc, malloc, free

#define SOCKET_DISCONNECT 0xFF
#define SOCKET_CONNECT 0xFD
#define BUFFER_OVERRUN 0xFC

const int DATA_COUNT = 6;
const int BLOCK_SIZE = 64;
const int HEADER_SIZE = 1024 * 4;

typedef struct HTTPDATA
{
    char *method;

    char *connection;
    char *content_location;
    char *content_type;
    char *content_disposition;
    int content_length;

    char *raw;
    char *content;

} HTTPDATA;

typedef class HTTPHEADER
{
public:
    char *table[BLOCK_SIZE] = {
        (char *)/**/ "",
        (char *)/**/ "Connection: ",
        (char *)/**/ "Content-Location: ",
        (char *)/**/ "Content-Type: ",
        (char *)/**/ "Content-Disposition: ",
        (char *)/**/ "Content-Length: "};

} HTTPHEADER;

typedef class HTTPINFO
{
public:
    char *method;

    char *connection;
    char *content_location;
    char *content_type;
    char *content_disposition;
    char *content_length;

    char *raw;
    char *content = NULL;

    char *ip;

    char **list;

    char *table[BLOCK_SIZE] = {
        (char *)/*METHOD*/ "\x01",
        (char *)/*Connection: */ "close",
        (char *)/*Content-Location: */ "/",
        (char *)/*Content-Type: */ "binary",
        (char *)/*Content-Disposition: */ "inline",
        (char *)/*Content-Length: */ "0"};
    HTTPHEADER header_info;

public:
    HTTPINFO(int size = BLOCK_SIZE)
    {
        raw = (char *)calloc(HEADER_SIZE * sizeof(char), sizeof(char));

        list = (char **)calloc((DATA_COUNT + 1) * sizeof(char *), sizeof(char *));
        char **ptr = &method;

        for (int i = 0; i != DATA_COUNT; i++)
        {
            list[i] = (char *)malloc(size * sizeof(char));
            memcpy(list[i], table[i], size * sizeof(char));

            *ptr = list[i];
            ptr += 1;

            if (ptr == &raw)
            {
                break;
            }
        }

        content = (char *)list[DATA_COUNT];
    }

    ~HTTPINFO()
    {
        for (int i = 0; i != DATA_COUNT; i++)
        {
            free(list[i]);
        }
        free(list);
        free(raw);

        free(content);
    }

} HTTPINFO, response, request;

int RecvLine(SOCKET hSocket, char *data, int length)
{
    memset(data, 0, length);

    int error = 0;

    for (int x = 0; x != length; x++)
    {
        if ((error = socket_read(hSocket, &data[x], sizeof(data[0]))) != 0)
        {
            break;
        }

        if (data[x] == '\n')
        {
            break;
        }
    }

    return error;
}

typedef class HTTPIO
{
    SOCKET server_socket;
    SOCKET client_socket;

    HTTPHEADER header;

    int HttpRecv(SOCKET hSocket, HTTPINFO *info)
    {
        char recv_line_buffer[1024];
        int raw_index = 0;
        int total_header_size = 0;

        for (;;)
        {
            register int recv_error = RecvLine(hSocket, recv_line_buffer, sizeof(recv_line_buffer));
            int raw_memory_block_size = _msize(info->raw);
            int recv_line_buffer_string_length = lstrlen(recv_line_buffer);

            total_header_size += recv_line_buffer_string_length;

            if (raw_memory_block_size < total_header_size)
            {
                info->raw = (char *)realloc(info->raw, total_header_size + raw_memory_block_size);
            }
            for (int recv_line_buffer_index = 0; recv_line_buffer_index != recv_line_buffer_string_length; recv_line_buffer_index++)
            {
                info->raw[raw_index++] = recv_line_buffer[recv_line_buffer_index];
            }
            if (recv_error != 0)
            {
                return recv_error;
            }
            if (KMP(info->raw, "\r\n\r\n") != NULL)
            {
                break;
            }
        }

        char **double_pointer = &info->method;

        for (int index = 1; index != DATA_COUNT; index++)
        {

            char *kmp_return = NULL;
            double_pointer += 1;

            if ((kmp_return = strstr(info->raw, header.table[index])) != NULL)
            {
                kmp_return += lstrlen(header.table[index]);
                char *CRLF = (char *)strstr(kmp_return, "\r\n");
                int header_distance = CRLF - kmp_return;

                if (header_distance + 1 > _msize(*double_pointer))
                {
                    *double_pointer = (char *)realloc(*double_pointer, header_distance + 1);
                }

                int size = _msize(*double_pointer);

                memset(*double_pointer, 0, size);
                memcpy(*double_pointer, kmp_return, header_distance);

                if (lstrcmpiA(header.table[index], "Content-Length: ") == 0)
                {
                    int content_length = atoi(*double_pointer);
                    info->content = (char *)realloc(info->content, content_length);
                    socket_read(hSocket, info->content, content_length);
                }
            }
            else
            {
                *double_pointer[0] = 0;
            }

            if (double_pointer == &info->raw)
            {
                break;
            }
        }

        return 0;
    }

    int HttpSend(SOCKET hSocket, HTTPINFO *info, const char *header_string)
    {
        // create response header

        char **double_pointer = &info->connection;
        memcpy(info->raw, header_string, lstrlenA(header_string) + 1);
        lstrcat(info->raw, "\r\n");

        for (int x = 1; x != DATA_COUNT; x++)
        {
            wsprintfA(info->raw, "%s%s%s\r\n", info->raw, header.table[x], *double_pointer);
            double_pointer += 1;
        }
        lstrcat(info->raw, "\r\n\r\n");

        int default_size = lstrlen(info->raw) - 1;
        int content_size = atoi(info->content_length);
        //
        // header size + content size
        int total_size = default_size + content_size;
        //
        // add content (binary data)

        char *raw_ptr = info->raw;

        info->raw += default_size;
        memcpy(info->raw, info->content, content_size);
        info->raw = raw_ptr;

        return socket_send(hSocket, info->raw, total_size);
    }

public:
    int request(const char *ip, const char *port, HTTPINFO *response_info, HTTPINFO *request_info)
    {
        int error = 0;

        SOCKET server_socket = socket(AF_INET, SOCK_STREAM, 0);
        for (;;)
        {
            connect(server_socket, ip, port);

            int r = WSAGetLastError();

            if (r == 0 || r == WSAEWOULDBLOCK || r == WSAEISCONN)
            {
                break;
            }
            else
            {
                return -1;
            }
        }

        HttpSend(server_socket, request_info, request_info->method);
        HttpRecv(server_socket, response_info);

        shutdown(server_socket, SD_BOTH);
        closesocket(server_socket);

        return error;
    }

    SOCKET InitializeHttpServer(int port, int backlog)
    {
        return (server_socket = open(port, backlog));
    }

    int response(HTTPINFO *request_info, HTTPINFO *response_info)
    {
        int error = 0;

        if ((client_socket = permit(server_socket)) != SOCKET_ERROR)
        {
        }
        else
        {
            error = SOCKET_ERROR;
        }

        return error;
    }

} HTTPIO;