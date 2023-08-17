#include "http_server.hh"

#include <vector>

#include <bits/stdc++.h>

#include <sys/stat.h>

#include <fstream>

#include <sstream>

vector<string> split(const string &s, char delim)
{
    vector<string> elems;

    stringstream ss(s);
    string item;

    while (getline(ss, item, delim))
    {
        if (!item.empty())
            elems.push_back(item);
    }
    return elems;
}

HTTP_Request::HTTP_Request(string request)
{
    vector<string> lines = split(request, '\n');
    vector<string> first_line = split(lines[0], ' ');

    this->HTTP_version = "1.0";
    this->method = first_line[0];
    this->url = first_line[1];
    if (this->method != "GET")
    {
        cerr << "Method '" << this->method << "' not supported" << endl;
        exit(1);
    }

    //cout << this->url << endl;
}

HTTP_Response *handle_request(string req)
{
    HTTP_Request *request = new HTTP_Request(req);
	
    HTTP_Response *response = new HTTP_Response();
    string url = string("html_files") + request->url;
    response->HTTP_version = "1.0";
    struct stat sb;

    if (stat(url.c_str(), &sb) == 0) // requested path exists
    {
        response->status_code = "200";
        response->status_text = "OK";
        response->content_type = "text/html";

        string body;
        string ind = "/index.html";
        if (S_ISDIR(sb.st_mode))
        {
            url = url + ind;
        }

        int n = url.length();
        char cha[n + 1];
        strcpy(cha, url.c_str());
        FILE *fp = fopen(cha, "r");

        if (fp == NULL)
        {
            printf("Could not create \n");
        }

        response->body = "";
        char *fr = (char *)malloc(sizeof(char) * 256);
        while (fgets(fr, 255, fp) != NULL)
        {
            response->body = response->body + fr;
        }
        free(fr);
        fclose(fp);
        response->content_length = to_string(response->body.length());
    }
    else
    {
        response->status_code = "404";
        response->status_text = "Not Found";
        response->content_type = "text/html";
        FILE *fp = fopen("error.html", "r");
        response->body = "";
        char fr[256];
        while (fgets(fr, 256, fp) != NULL)
        {
            response->body = response->body + fr;
        }

        fclose(fp);
        response->content_length = to_string(response->body.length());
    }

    delete request;

    return response;
}

string HTTP_Response::get_string(HTTP_Response *response)
{
    string temp = "HTTP/1.0";
    temp = temp + " " + response->status_code + " " + response->status_text + "\n";
    temp = temp + "Content-Length: " + response->content_length + "\n";
    temp = temp + "Content-Type: " + response->content_type + "\n";
    temp = temp + "\n";
    temp = temp + response->body;
	//free(response);
    return temp;
}
