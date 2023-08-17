#include "http_server.hh"

#include <stdio.h>

#include <bits/stdc++.h>

#include <stdlib.h>

#include <string.h>

#include <thread>

#include <unistd.h>

#include <netinet/in.h>

#include <pthread.h>

pthread_mutex_t lck;
pthread_cond_t master;
pthread_cond_t slave;
queue<int> q;
int noth =200;

void *tfn(void *p)
{
    while (1)
    {
        free(p);
        pthread_mutex_lock(&lck);
        while (q.empty())
        {
            pthread_cond_wait(&slave, &lck);
        }
        int x = q.front();
        q.pop();
        pthread_cond_signal(&master);
        pthread_mutex_unlock(&lck);
        
        char buffer[512];
        int n;
        bzero(buffer, 512);
        n = read(x, buffer, 511);
        string check = buffer;
        if (check.length() > 0 && buffer[0] == 'G')
        {
            HTTP_Response *rep = handle_request(buffer);
            string res = (*rep).get_string(rep);
            free(rep);
            int w = res.length();
            char cha[w + 1];
            strcpy(cha, res.c_str());
            n = write(x, cha, res.length());
            if (n < 0)
                cout << "write failed\n";
            close(x);
            fflush(stdout);
            bzero(buffer, 512);
            //sleep(4);
        }
        bzero(buffer, 512);
    }
}

void error(char *msg)
{
    perror(msg);
    exit(1);
}
void create(int i)
{
    for (int j = 0; j < i; j++)
    {
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, tfn, NULL);
    }
}
int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;

    if (argc < 2)
    {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    bzero((char *)&serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");
    listen(sockfd, 25);
    clilen = sizeof(cli_addr);
    pthread_mutex_init(&lck, NULL);
    pthread_cond_init(&master, NULL);
    pthread_cond_init(&slave, NULL);
    create(noth);
    
    while (1)
    {
        bzero((char *)&cli_addr, sizeof(cli_addr));
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        if (newsockfd < 0)
            error("ERROR on accept");
        pthread_mutex_lock(&lck);
        while (q.size() == 4000)
       	{
            pthread_cond_wait(&master, &lck);
        }
        q.push(newsockfd);
        pthread_cond_signal(&slave);
        pthread_mutex_unlock(&lck);
        
    }
    return 0;
}
