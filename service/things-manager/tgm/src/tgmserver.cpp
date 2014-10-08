

#include <tgm.h> 
    
//#define TGM_SQLITE      

int main()    
{   
    // Create PlatformConfig object 
    PlatformConfig cfg(OC::ServiceType::InProc,
        OC::ModeType::Both, 
        "134.134.161.33", 
        56832,    
        OC::QualityOfService::NonConfirmable);
 
    try
    { 
        GroupManagerHandler *gmHandler = new GroupManagerHandler();
        g_platform = new OCPlatform(cfg);

        createGroupManagerResource(g_platform, gmHandler);

        g_mapDeviceListOfResourceType = new std::map< std::string, std::vector< Device* > >();
        g_mapDeviceOfDeviceId = new std::map< std::string, Device* >();


        while (true)
        {
            // some tasks
            int steps;

            std::cin >> steps;
            if (steps == 0)
            {
                closeSqlite3();
                exit(0);
            }

        }
    }
    catch (OCException e)
    {
        //log(e.what());
    }

    // No explicit call to stop the platform.
    // When OCPlatform destructor is invoked, internally we do platform cleanup
}