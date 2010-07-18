#include <stdio.h>
#include <iostream>
#include <string.h>
#include <curl/curl.h>

using namespace std;

static string buffer;

static int writer(char *data, size_t size, size_t nmemb, string *buffer) {

    int result = 0;

    if (buffer != NULL) {

        buffer->append(data, size * nmemb);

        result = size * nmemb;
    }

    return result;
}

string perform_request() {

    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    
    if (curl) {

        curl_easy_setopt(curl, CURLOPT_URL, "http://whatthecommit.com");
        curl_easy_setopt(curl, CURLOPT_HEADER, 0);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (!res) {

            return buffer;
        }
    }
    
    return "";
}

string parse(string response) {

    string p_start("<p>"), p_stop("</p>"), buffer("");
    int start, stop;
    
    start = response.find(p_start);
    stop = response.find(p_stop);
    
    if ((start != string::npos) && (stop != string::npos)) {
        
        start += p_start.length();
        buffer = response.substr(start, stop - start - 1);
    }
    
    return buffer;
}

int main(int argc, char* argv[]) {

    string cmd("git");
    string response;
    string message;

    for (int i = 1; i < argc; i++) {

        if (!strcmp(argv[i], "--automessage") || !strcmp(argv[i], "-am")) {

            response = perform_request();

            if (response != "") {

                message = parse(response);
                
            } else {
                
                exit(1);
            }
            
            string new_message("-m \"");
            new_message.append(message);
            new_message.append("\"");
            cmd += " ";
            cmd += new_message;
        } else {

            if (cmd.length()) {

                cmd += " ";
            }

            cmd += argv[i];
        }
    }
    
    cout << message << "\n";
    
    system(cmd.c_str());
    
    return 0;
}
