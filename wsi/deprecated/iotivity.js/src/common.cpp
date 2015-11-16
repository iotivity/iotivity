#include <string>
#include "OCPlatform.h"

using namespace std;

string make_json(const OC::OCRepresentation& rep, const int eCode, string resourceID) 
{
    stringstream converter;
    string boolResult;

    string makeJson = "{\"id\":\"" + resourceID +  "\",";
    if(eCode == OC_STACK_OK)
    {
        cout << "GET request was successful" << endl;
        cout << "=================================================" << endl;

        OCRepPayload* paylod = rep.getPayload();
        OCRepPayloadValue* val = paylod->values;
        while(val)
        {
            switch(val->type)
            {
                case OCREP_PROP_NULL:
                    makeJson = makeJson + "\"" + val->name + "\":"+ ":NULL,";
                    break;
                case OCREP_PROP_INT:
                    converter << val->i;
                    cout << val->name << " : " << converter.str() << endl;
                    makeJson = makeJson + "\"" + val->name + "\":"+ "\"" + converter.str() + "\",";
                    break;
                case OCREP_PROP_DOUBLE:
                    converter << val->d;
                    cout << val->name << " : " << converter.str() << endl;
                    makeJson = makeJson + "\"" + val->name + "\":"+ "\"" + converter.str() + "\",";
                    break;
                case OCREP_PROP_BOOL:
                    boolResult = (val->b ? "true" : "false");
                    cout << val->name << " : " << boolResult << endl;
                    makeJson = makeJson + "\"" + val->name + "\":"+ "\"" + boolResult + "\",";
                    break;
                case OCREP_PROP_STRING:
                    cout << val->name << " : " << val->str << endl;
                    makeJson = makeJson + "\"" + val->name + "\":"+ "\"" + val->str + "\",";
                    break;
                case OCREP_PROP_OBJECT:
                    cout << val->name << " : " << val->obj->uri << endl;
                    makeJson = makeJson + "\"" + val->name + "\":"+ "\"" + val->obj->uri + "\",";
                    break;
                case OCREP_PROP_ARRAY:
                    switch(val->arr.type)
                    {
                        case OCREP_PROP_INT:
                            cout << val->name << "(int array) : "
                                 << val->arr.dimensions[0] << " x "
                                 << val->arr.dimensions[1] << " x "
                                 << val->arr.dimensions[2] << endl;
                            break;
                        case OCREP_PROP_DOUBLE:
                            cout << val->name << "(double array) : "
                                 << val->arr.dimensions[0] << " x "
                                 << val->arr.dimensions[1] << " x "
                                 << val->arr.dimensions[2] << endl;
                            break;
                        case OCREP_PROP_BOOL:
                            cout << val->name << "(bool array) : "
                                 << val->arr.dimensions[0] << " x "
                                 << val->arr.dimensions[1] << " x "
                                 << val->arr.dimensions[2] << endl;
                            break;
                        case OCREP_PROP_STRING:
                            cout << val->name << "(string array) : "
                                 << val->arr.dimensions[0] << " x "
                                 << val->arr.dimensions[1] << " x "
                                 << val->arr.dimensions[2] << endl;
                            break;
                        case OCREP_PROP_OBJECT:
                            cout << val->name << "(OCRep array) : "
                                 << val->arr.dimensions[0] << " x "
                                 << val->arr.dimensions[1] << " x "
                                 << val->arr.dimensions[2] << endl;
                            break;
                        default:
                            cout << val->name <<  " : Unknown/unsupported array type!" << endl;
                            break;
                    }
                    break;
                default:
                    cout << val->name <<  " : Unknown type!" <<endl;
                    break;
            }
            val = val -> next;
        }
        cout << "=================================================" << endl;
        cout << "Resource URI: " << rep.getUri() << endl;
    }
    else
    {
        cout << "onGET Response error: " << eCode << endl;
        exit(-1);
    }
    makeJson.erase(prev(makeJson.end()));
    makeJson = makeJson + "}";

    return makeJson;
}

void init_iotivity()
{
    //Create PlatformConfig object
    OC::PlatformConfig cfg {
        OC::ServiceType::InProc,
        OC::ModeType::Both,
        "0.0.0.0",
        0,
        OC::QualityOfService::LowQos,
        NULL
    };

    OC::OCPlatform::Configure(cfg);

}
