#ifndef __HTTP_CURL_H__
#define __HTTP_CURL_H__

#include <string>
#include <curl/curl.h>
using namespace std;

class CHttpClient
{
    public:
        CHttpClient(void);
        ~CHttpClient(void);

    public:

        int Post(const std::string &strUrl, const std::string &strPost, std::string &strResponse);


        int Get(const std::string &strUrl, std::string &strResponse);

        int Put(const std::string &strUrl, const std::string &strPost, std::string &strResponse);

        int Delete(const std::string &strUrl, std::string &strResponse);

    public:
        void SetDebug(bool bDebug);

    private:
        bool m_bDebug;
};

#endif
