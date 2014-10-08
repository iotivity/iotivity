#ifndef _SQLSTRING_H_
#define _SQLSTRING_H_

#include "sqlite3.h"
#include "Group.h"
#include <cstring>
#include <time.h>

#define SQL_RESULT int
#define FULLTEXT -1

#define TGM_DB_NAME "tgm.db"
#define LOGPRINT(stdout, string, ...)  fprintf(stdout, string , ## __VA_ARGS__ )


typedef const char* SQLTEXT;

sqlite3 *g_tgmDB;

const char *sqlStrCreateGroupTable = "CREATE TABLE IF NOT EXISTS 'group' ("
        "'guid'	TEXT,"
        "'name'	TEXT,"
        "'description' TEXT,"
        "'created_date'	INTEGER,"
        "'actionset_list'	TEXT,"
        "'device_mandatory_list'	TEXT,"
        "'device_optional_list'	TEXT,"
        "PRIMARY KEY(guid)"
        ");";
const char *sqlStrInsertToGroup =
        "INSERT INTO 'group'"
                "('guid','name','created_date','description','actionset_list','device_mandatory_list','device_optional_list') VALUES "
                "(@guid,@name,@created_date,@description,@actionset_list,@device_mandatory_list,@device_optional_list);";

int sqlCallback(void *NotUsed, int argc, char **argv, char **azColName)
{
    int i;
    LOGPRINT(stdout, "\n=====CALL BACK=====\n");

    for (i = 0; i < argc; i++)
    {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

void checkResultLog(int rc, char *zErrMsg, const char *OkMsg)
{
    LOGPRINT(stdout, "\n");

    if (rc != SQLITE_OK)
    {
        LOGPRINT(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else
    {
        LOGPRINT(stdout, "%s successfully\n", OkMsg);
    }
}

void resetTables(sqlite3 *db)
{
    int rc = 0;
    char *errMsg = 0;

    const char* dropTable1 = "DROP TABLE IF EXISTS 'group';";
    const char* dropTable2 = "DROP TABLE IF EXISTS 'device';";
    const char* dropTable3 = "DROP TABLE IF EXISTS 'actuation_set';";

    rc = sqlite3_exec(db, dropTable1, sqlCallback, 0, &errMsg);
    checkResultLog(rc, errMsg, "DROP TABLE 'group' ");
    rc = sqlite3_exec(db, dropTable2, sqlCallback, 0, &errMsg);
    checkResultLog(rc, errMsg, "DROP TABLE 'device' ");
    rc = sqlite3_exec(db, dropTable3, sqlCallback, 0, &errMsg);
    checkResultLog(rc, errMsg, "DROP TABLE 'actuation_list' ");
}

void checkResultErrMsg(sqlite3* db, int rc, const char* msg)
{
    LOGPRINT(stdout, "\n");

    if (rc != SQLITE_DONE)
    {
        LOGPRINT(stdout, "%s SQL Error \nerrcode %d : %s\n", msg, rc, sqlite3_errmsg(db));
    }
    else
    {
        LOGPRINT(stdout, "%s successfully \n", msg);
    }
}
void createGroupTable(sqlite3 *db)
{
    int rc;
    char *errMsg = 0;
    rc = sqlite3_exec(db, sqlStrCreateGroupTable, sqlCallback, 0, &errMsg);
    checkResultLog(rc, errMsg, "CREATE TABLE group ");
}

void deleteGroupTable(sqlite3 *db)
{
    int rc;
    char *errMsg = 0;
    rc = sqlite3_exec(db, "DELETE FROM 'group' ", sqlCallback, 0, &errMsg);
    checkResultLog(rc, errMsg, "DELETE FROM 'group'");
}
/* note v2 apis */
//rc = sqlite3_open_v2(TGM_DB_NAME, &db,SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,NULL);
void deleteGroupToGroupTable(const char* guid)
{
    std::string deleteSql = "DELETE FROM 'group' WHERE guid='";
    deleteSql.append(guid);
    deleteSql.append("'");
    int rc;
    char *errMsg = 0;
    rc = sqlite3_exec(g_tgmDB, deleteSql.c_str(), sqlCallback, 0, &errMsg);
    checkResultLog(rc, errMsg, deleteSql.c_str());
}
void insertGroupToGroupTable(OC::Group* groupData)
{
    time_t created_date = time(NULL);

    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(g_tgmDB, sqlStrInsertToGroup, -1, &stmt, NULL);

    int paramIndex = sqlite3_bind_parameter_index(stmt, "@guid");
    sqlite3_bind_text(stmt, paramIndex, groupData->m_guid.c_str(), -1, SQLITE_TRANSIENT);

    /* Test for exception handling */
    paramIndex = sqlite3_bind_parameter_index(stmt, "@name");
    if (groupData->m_name != "")
    {
        sqlite3_bind_text(stmt, paramIndex, groupData->m_name.c_str() , -1, SQLITE_TRANSIENT);
    }
    else
    {
        sqlite3_bind_null(stmt, paramIndex);
    }

    paramIndex = sqlite3_bind_parameter_index(stmt, "@created_date");
    sqlite3_bind_int(stmt, paramIndex, created_date);

    paramIndex = sqlite3_bind_parameter_index(stmt, "@description");
    sqlite3_bind_text(stmt, paramIndex, groupData->m_description.c_str(), -1, SQLITE_TRANSIENT);


    std::string actionset_list = JsonPackActionSetList(groupData->m_listActionSet);
    paramIndex = sqlite3_bind_parameter_index(stmt, "@actionset_list");
    sqlite3_bind_text(stmt, paramIndex, actionset_list.c_str(), -1, SQLITE_TRANSIENT);



    std::string device_mandatory;
    JsonPackDeviceList(groupData->m_listMandatoryDevices, device_mandatory);
    paramIndex = sqlite3_bind_parameter_index(stmt, "@device_mandatory_list");
    sqlite3_bind_text(stmt, paramIndex, device_mandatory.c_str(), -1, SQLITE_TRANSIENT);


    std::string device_optional;
    JsonPackDeviceList(groupData->m_listOptionalDevices, device_optional);
    paramIndex = sqlite3_bind_parameter_index(stmt, "@device_optional_list");
    sqlite3_bind_text(stmt, paramIndex, device_optional.c_str(), -1, SQLITE_TRANSIENT);


    int rc = sqlite3_step(stmt); 


    checkResultErrMsg(g_tgmDB, rc, "INSERT TO GROUP");
}


int groupSelectCallback(void *NotUsed, int argc, char **argv, char **azColName)
{
    int i;
    LOGPRINT(stdout, "\n=====CALL BACK=====\n");

    OC::Group *foundGroup = new OC::Group();

    for (i = 0; i < argc; i++)
    {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");

        if (strncmp(azColName[i], "guid",4) == 0)
        {
            foundGroup->m_guid = argv[i];
        }
        else if(strncmp(azColName[i], "name",4) == 0)
        {
            foundGroup->m_name = argv[i];
        }
        else if(strncmp(azColName[i], "description",11) == 0)
        {
            foundGroup->m_description = argv[i];
        }
        else if(strncmp(azColName[i], "device_mandatory_list",21) == 0)
        {
            foundGroup->m_listMandatoryDevices = JsonUnpackDeviceListReturn(argv[i]);
        }
        else if(strncmp(azColName[i], "device_optional_list",20) == 0)
        {
            foundGroup->m_listOptionalDevices = JsonUnpackDeviceListReturn(argv[i]);
        }
        else if(strncmp(azColName[i], "actionset_list",20) == 0)
        {
            foundGroup->m_listActionSet = JsonUnpackActionSetList(argv[i]);
        }
    }
    
    /*//TEST CODE
    ///ActionSetList Test START
    std::string actionstr = JsonPackActionSetList(foundGroup->m_listActionSet);
    printf("\n\n%s\n\n",actionstr.c_str());

    std::vector<OC::ActionSet*> asList = JsonUnpackActionSetList(actionstr.c_str());
    std::string actionstr2 = JsonPackActionSetList(asList);
    printf("\n\n%s\n\n",actionstr2.c_str());
    ///ActionSetList Test END



    //DeviceList Test START
    std::string devicestr1,devicestr2;
    JsonPackDeviceList(foundGroup->m_listMandatoryDevices,devicestr1);
    printf("\n\n%s\n\n",devicestr1.c_str());


    std::vector< OC::Device* > deviceList;
    JsonUnpackDeviceList(devicestr1.c_str(),deviceList);
    JsonPackDeviceList(deviceList,devicestr2);
    printf("\n\n%s\n\n",devicestr2.c_str());
    //DeviceList Test END
    */

    return 0;
}

bool initSqlite3()
{
    bool returnValue = false;
    int rc = sqlite3_open(TGM_DB_NAME, &g_tgmDB);

    if (rc != SQLITE_OK)
    {
        LOGPRINT(stderr, "Can't open database: %s\n", sqlite3_errmsg(g_tgmDB));
        returnValue = false;
    }
    else
    {
        LOGPRINT(stderr, "\nOpened database successfully\n");
        returnValue = true;
    }

    createGroupTable(g_tgmDB);

    return returnValue;
}

bool closeSqlite3()
{
    bool returnValue = false;
    int rc = sqlite3_close(g_tgmDB);

    if (rc != SQLITE_OK)
    {
        LOGPRINT(stderr, "Can't close database: %s\n", sqlite3_errmsg(g_tgmDB));
        returnValue = false;
    }
    else
    {
        LOGPRINT(stderr, "\nclose database successfully\n");
        returnValue = true;
    }

    return returnValue;
}

#endif