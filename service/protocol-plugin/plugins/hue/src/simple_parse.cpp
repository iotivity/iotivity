#include "simple_parse.h"

using namespace std;
using namespace boost;



bool readConfigFile(const char *cfgfilepath, const string &key, string &value)
{
    fstream cfgFile;
    cfgFile.open(cfgfilepath);
    if ( ! cfgFile.is_open())
    {
        cout << "can not open cfg file!" << endl;
        return false;
    }
    char tmp[1000];
    while (!cfgFile.eof())
    {
        cfgFile.getline(tmp, 1000); 
        string line(tmp);
        size_t pos;
        if ((pos = line.find('#')) != -1)
        {
            if (0 == pos)  continue;
        }
        pos = line.find('=');
        if (pos == string::npos) continue;
        string tmpKey = line.substr(0, pos); 
        trim(tmpKey); 

        if (key == tmpKey)
        {
            size_t lscrap, rscrap;
            if (((lscrap = line.find('[')) != string::npos) && ((rscrap = line.find(']')) != string::npos))
            {
                value = line.substr(lscrap + 1, rscrap - lscrap - 1); 
            }
            else value = line.substr(pos + 1); 
            trim(value);
            return true;
        }
    }
    return false;
}


vector<string> NormalizedOut(string &input)
{
    char *outer_ptr = NULL;
    char *p[20];
    int in = 0;
    vector<string> vecStr;
    char *buf = (char *)input.c_str();

    while ((p[in] = strtok_r(buf, ",", &outer_ptr)) != NULL)
    {
        string out = p[in];
        trim(out);
        vecStr.push_back(out);
        in++;
        buf = NULL;
    }
    return vecStr;
}
