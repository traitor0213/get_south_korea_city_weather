#include "./lib/cuslib.h"

char *input_location()
{
    //data section
    static char input[1024];
    static char html[1024];

    //shell
    printf("type the city name in south korea\n");
    printf(">");
    
    //input
    fgets(input, sizeof(input) - 1, stdin);
    strtok(input, "\r\n");
    
    //string control
    wsprintf(html, "<dl class=\"po_%s\">", input);
    
    //return data section
    return html;
}

int get_weather(HTTPIO *io)
{
    response res; //response class
    request req;  //request class 

    //create request method
    req.method = (char *)"GET http://www.weather.go.kr/home/index.tab.now-ten.jsp?unit=K HTTP/1.1\r\n";

    //send request & recv response
    io->request("www.weather.go.kr", "80", &res, &req);

    //find suwon content
    char *find_location = KMP(res.content, input_location());

    //successfully find suwon content
    if (find_location != NULL)
    {
        //find weather
        char *tmp = KMP(find_location, "alt=\"");
        //skip html
        tmp += lstrlen("alt=\"");
        //temp buffer
        char buffer[1024];

        //copy string content (weather)
        for (int i = 0; i != sizeof(buffer); i++)
        {
            if (tmp[i] == '\"')
            {
                buffer[i] = 0;
                break;
            }

            buffer[i] = tmp[i];
        }

        //save in variable weather
        char weather[1024];
        memcpy(weather, buffer, sizeof(weather));

        //get temperature
        tmp = KMP(find_location, "<dd class=\"temp\">");
        //skip html
        tmp += lstrlen("<dd class=\"temp\">");

        //copy string content
        for (int i = 0; i != sizeof(buffer); i++)
        {
            if (tmp[i] == '<')
            {
                buffer[i] = 0;
                break;
            }

            buffer[i] = tmp[i];
        }

        //save in variable temperature
        char temperature[1024];
        memcpy(temperature, buffer, sizeof(temperature));

        //print suwon info
        printf("weather->%s\n", weather);
        printf("temperature->%s\n", temperature);
        return 0;
    }
    else
    {
        return -1;
    }
}

int main()
{
    //Initialize Windows Socket Library (ws2_32.dll)
    //if use the visual-studio compiler, add ...
    /*
    #pragma comment(lib, "ws2_32.lib")
    #define _WINSOCK_DEPRECATED_NO_WARNINGS
    #define _CRT_SECURE_NO_WARNINGS
    */

    //WSA is windows socket api

    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    HTTPIO http;        //http in/out put class
    get_weather(&http); //get weather & print

    WSACleanup(); //uninitialize ws2_32.dll

    return 0;
}