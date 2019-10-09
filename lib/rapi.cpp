char *KMP(const char *source, const char *destination)
{
    char *address = NULL;

    int source_string_length = lstrlen(source);
    int destination_string_length = lstrlen(destination);

    for (int i = 0; i != source_string_length; i++)
    {
        if (source[i] == destination[0])
        {
            if (i + destination_string_length > source_string_length)
            {
                return NULL;
            }

            int x = 0;
            for (; x != destination_string_length; x++)
            {
                if (source[i + x] != destination[x])
                {
                    break;
                }
            }

            if (x == destination_string_length)
            {
                address = (char *)&source[i];
                break;
            }
        }
    }

    return address;
}

HANDLE MakeThread(THREAD function, LPVOID paramter)
{
    return CreateThread(NULL, 0, function, paramter, 0, NULL);
}

char *IntToString(int i)
{
    static char r[64];
    wsprintfA(r, "%d", i);

    return r;
}

char *SkipString(const char *source, const char *destination)
{
    return KMP(source, destination);
}

char *SeparateString(char *buffer, const char *source, const char *destination)
{
    char *address = KMP(source, destination);
    int length = address - source;

    memcpy(buffer, source, length);

    return buffer;
}
