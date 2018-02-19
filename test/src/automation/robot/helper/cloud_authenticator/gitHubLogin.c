/******************************************************************
 *
 * Copyright 2018 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *
 ******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

char INIT_GET_LOGIN_URL[]="https://github.com/login";
char LOGIN_URL[]="https://github.com/session";
char IOTIVITY_CLOUD_URL[]="https://github.com/login?return_to=%2Flogin%2Foauth%2Fauthorize%3Fclient_id%3Dea9c18f540323b0213d0%26redirect_uri%3Dhttp%253A%252F%252Fwww.example.com%252Foauth_callback%252F";
char PAR_POST_VAL[]="commit=Sign+in&utf8=%E2%9C%93";//"commit=Sign+in&utf8=✓";//"commit=Sign+in&utf8=%E2%9C%93";
char AUTHENTICITY_TOKEN[]="&authenticity_token=";


typedef struct stringstr {
  char *ptr;
  size_t len;
}stringstr;


void init_string(stringstr *str) {
    str->len = 0;
    str->ptr = malloc(str->len+1);
  if (str->ptr == NULL) {
    fprintf(stderr, "malloc() failed\n");
    exit(EXIT_FAILURE);
  }
  str->ptr[0] = '\0';
}

size_t writefunc(void *ptr, size_t size, size_t nmemb, stringstr *str)
{
  size_t new_len = str->len + size*nmemb;
  str->ptr = realloc(str->ptr, new_len+1);
  if (str->ptr == NULL) {
    fprintf(stderr, "realloc() failed\n");
    exit(EXIT_FAILURE);
  }
  memcpy(str->ptr+str->len, ptr, size*nmemb);
  str->ptr[new_len] = '\0';
  str->len = new_len;

  return size*nmemb;
}

char* get_authenticity_token(const char* resposeTxt)
{
	char* authLast="";
	char* auth_token=strstr(resposeTxt,"<input name=\"authenticity_token\" type=\"hidden\" value=\"");
	auth_token=strstr(auth_token,"value=\"");
	auth_token=&auth_token[strlen("value=\"")];
	authLast=strstr(auth_token,"\" />");
	auth_token[(authLast-auth_token)]='\0';
	return auth_token;
}

char* get_auth_token_code(const char* resposeTxt,char *code)
{
    char* authLast="";
    //char *code;
    int len;
    char* auth_token=strstr(resposeTxt,"<a href=\"http://www.example.com/oauth_callback/?code=");
    authLast=strstr(auth_token,"&");
    auth_token[(authLast-auth_token)]='\0';
    auth_token=strstr(auth_token,"code=");
    auth_token=strstr(auth_token,"=");
    len = strlen(auth_token);
    memset(code,0,len);
    strncpy(code,auth_token+1,len-1);
    return code;
}

char* getgithubcode(char gitlogin[],char gitpassword[], char *code)
{
    char demoPost[1000]={0};
    char *auth_text="";
    char *auth_url_text="";
    char *code_text = "";
    CURL *curl;
    CURLcode res;

    struct curl_slist *cookies;
    struct curl_slist *nc;
    struct curl_slist *header;
    struct curl_slist *rheader;

    int i;
    int http_code=0;
    stringstr str;
    init_string(&str);

    curl_global_init(CURL_GLOBAL_ALL);

    //get login page
    curl = curl_easy_init();

    if(curl)
    {
        //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        //Get Github login page
        curl_easy_setopt(curl, CURLOPT_URL, INIT_GET_LOGIN_URL);
        curl_easy_setopt(curl, CURLOPT_COOKIEFILE, ""); /* start cookie engine */
        curl_easy_setopt(curl, CURLOPT_AUTOREFERER, 1L);

        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_COOKIELIST, "ALL"); // clear all cookies
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &str);

        res = curl_easy_perform(curl);

        if(res != CURLE_OK) {
          fprintf(stderr, "Curl perform failed: %s\n", curl_easy_strerror(res));
          return 1;
        }

        auth_text=get_authenticity_token(str.ptr);
        auth_url_text=curl_easy_escape(curl, auth_text, strlen(auth_text));
        sprintf(demoPost,"%s%s%s%s%s",PAR_POST_VAL,AUTHENTICITY_TOKEN,auth_url_text,gitlogin,gitpassword);
        free(str.ptr);

        init_string(&str);
        res = curl_easy_getinfo(curl, CURLINFO_COOKIELIST, &cookies);
          if(res != CURLE_OK) {
            fprintf(stderr, "Curl curl_easy_getinfo failed: %s\n",
                    curl_easy_strerror(res));
            exit(1);
          }

          //github login
          curl_easy_setopt(curl, CURLOPT_URL, LOGIN_URL);
          curl_easy_setopt(curl, CURLOPT_POST, 1L); /* start cookie engine */
          curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
          curl_easy_setopt(curl, CURLOPT_WRITEDATA, &str);
          curl_easy_setopt(curl,CURLOPT_POSTFIELDS,demoPost);


          nc = cookies, i = 1;
              while(nc) {
                curl_easy_setopt(curl, CURLOPT_COOKIELIST, nc->data);
                nc = nc->next;
                i++;
              }

          res = curl_easy_perform(curl);
          res = curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);

          //printf("\n\nResposecode: %d\n\n\n",http_code);
          //printf("\n%s\n",s.ptr);

          res = curl_easy_getinfo(curl, CURLINFO_COOKIELIST, &cookies);
              if(res != CURLE_OK) {
                fprintf(stderr, "Curl curl_easy_getinfo failed: %s\n",
                        curl_easy_strerror(res));
                exit(1);
              }
            free(str.ptr);

            //request for cloud code
            init_string(&str);
            curl_easy_setopt(curl, CURLOPT_URL, IOTIVITY_CLOUD_URL);
            curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);

            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &str);
            nc = cookies, i = 1;
                  while(nc) {
                    curl_easy_setopt(curl, CURLOPT_COOKIELIST, nc->data);
                    nc = nc->next;
                    i++;
                  }
            res = curl_easy_perform(curl);
            if(res != CURLE_OK) {
                  fprintf(stderr, "Curl perform failed: %s\n", curl_easy_strerror(res));
                  return 1;
                }
            //printf("\n%s\n",str.ptr);
            code_text = get_auth_token_code(str.ptr,&code);
            //printf("%s\n", code_text);

            curl_easy_cleanup(curl);
        }
        curl_global_cleanup();
        free(str.ptr);

        return code_text;
}

int main(int argc, char *argv[])
{
    char gitlogin[1000]="&login=";
    char gitpassword[1000]="&password=";
    char *code;
    strcat(gitlogin, argv[1]);
    strcat(gitpassword, argv[2]);
    code=getgithubcode(gitlogin,gitpassword,&code);
    printf("%s\n",code);
	return 0;
}
