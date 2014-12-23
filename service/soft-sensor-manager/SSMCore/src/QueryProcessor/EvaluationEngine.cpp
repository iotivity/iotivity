/******************************************************************
*
* Copyright 2014 Samsung Electronics All Rights Reserved.
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
******************************************************************/
#include "EvaluationEngine.h"

#define CHK_SQLITE(val, cond) {SSM_CLEANUP_COND_ASSERT(val, cond, sqlite3_errmsg(m_pSQLite3));}

SSMRESULT CEvaluationEngine::finalConstruct()
{
    SSMRESULT res = SSM_E_FAIL;

    m_pSQLite3 = NULL;

    m_mtxTriggerId.lock();
    m_iTriggerId = 0;
    m_mtxTriggerId.unlock();

    SSM_CLEANUP_ASSERT(CreateInstance(OID_ITasker, (IBase **)&m_pTasker));
    SSM_CLEANUP_ASSERT(initializeEngine());

CLEANUP:
    return res;
}

void CEvaluationEngine::finalRelease()
{
    terminateEngine();
}

SSMRESULT CEvaluationEngine::executeSQL_NoReturn(IN std::string strSQL)
{
    SSMRESULT       res = SSM_E_FAIL;
    sqlite3_stmt    *stmt = NULL;

    CHK_SQLITE(sqlite3_prepare_v2(m_pSQLite3, strSQL.c_str(), strSQL.length(), &stmt, NULL), SQLITE_OK);
    CHK_SQLITE(sqlite3_step(stmt), SQLITE_DONE);
    CHK_SQLITE(sqlite3_finalize(stmt), SQLITE_OK);

    res = SSM_S_OK;
CLEANUP:
    return res;
}

SSMRESULT CEvaluationEngine::executeSQL_IntReturn(IN std::string strSQL, OUT int *pResult)
{
    SSMRESULT       res = SSM_E_FAIL;
    sqlite3_stmt    *stmt = NULL;

    CHK_SQLITE(sqlite3_prepare_v2(m_pSQLite3, strSQL.c_str(), strSQL.length(), &stmt, NULL), SQLITE_OK);
    CHK_SQLITE(sqlite3_step(stmt), SQLITE_ROW);
    *pResult = sqlite3_column_int(stmt, 0);
    CHK_SQLITE(sqlite3_finalize(stmt), SQLITE_OK);

    res = SSM_S_OK;
CLEANUP:
    return res;
}

void CEvaluationEngine::onSQLTrigger(IN sqlite3_context *context, IN int argc,
                                     IN sqlite3_value **argv)
{
    CEvaluationEngine *pEvaluationEngine = (CEvaluationEngine *)sqlite3_value_int(argv[0]);

    pEvaluationEngine->onWatcherTriggered(sqlite3_value_int(argv[1]), sqlite3_value_int(argv[2]));
}

void CEvaluationEngine::onExecute(IN void *pArg)
{
    std::map<int, IEvaluationEngineEvent *>::iterator itor;
    intptr_t *pData = (intptr_t *)pArg;

    m_mtxTriggerId.lock();
    itor = m_mapTriggers.find(pData[0]);
    if (itor != m_mapTriggers.end())
    {
        IEvaluationEngineEvent      *pEvent = itor->second;
        pEvent->onWatchModelData(pData[0], pData[1]);
    }
    m_mtxTriggerId.unlock();
}

void CEvaluationEngine::onTerminate(IN void *pArg)
{
    intptr_t *pData = (intptr_t *)pArg;
    SAFE_ARRAY_DELETE(pData);
}

SSMRESULT CEvaluationEngine::onWatcherTriggered(IN int triggerId, IN int dataId)
{
    intptr_t     *pData = new intptr_t[2];
    pData[0] = triggerId;
    pData[1] = dataId;
    m_pTasker->addTask(this, (void *)pData);
    return SSM_S_OK;
}

SSMRESULT CEvaluationEngine::initializeEngine()
{
    SSMRESULT           res = SSM_E_FAIL;
    sqlite3             *pBackupFile = NULL;
    sqlite3_backup      *pBackup = NULL;

    const char *strCreate_ModelRelationTable =
        "create table [ModelRelation]\
		(\
		modelId integer primary key autoincrement,\
		modelName text NOT NULL,\
		lPos int NOT NULL,\
		rPos int NOT NULL\
		);";

    const char *strCreate_DataRelationTable =
        "create table [DataRelation]\
		(\
		id integer primary key autoincrement,\
		modelId int NOT NULL,\
		lPos int NOT NULL,\
		rPos int NOT NULL,\
		dataId int NOT NULL\
		);";

    //Create rootModel
    const char *strRootModel = "insert into [ModelRelation] values (null, 'root', 1, 2);";

    const char *tblRoot =
        "create table [ModelData1](dataId integer primary key autoincrement, name text);";

    const char *rootData = "insert into [ModelData1] values (null, 'root');";

    const char *rootRelation = "insert into [DataRelation] values (null, 1, 1, 2, 1);";

    CHK_SQLITE(sqlite3_open_v2(LOCATION_SSM_DB, &m_pSQLite3,
                               SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX, NULL), SQLITE_OK);

    CHK_SQLITE(sqlite3_create_function_v2(m_pSQLite3, "OnSQLTrigger", 3, SQLITE_UTF8, NULL,
                                          onSQLTrigger, NULL, NULL, NULL),
               SQLITE_OK);

    if (strlen(LOCATION_SSM_DB_DUMP) > 0 &&
        sqlite3_open_v2(LOCATION_SSM_DB_DUMP, &pBackupFile, SQLITE_OPEN_READWRITE, NULL) == SQLITE_OK)
    {
        pBackup = sqlite3_backup_init(m_pSQLite3, "main", pBackupFile, "main");
        CHK_SQLITE(sqlite3_errcode(pBackupFile), SQLITE_OK);
        SSM_CLEANUP_NULL_ASSERT(pBackup);
        CHK_SQLITE(sqlite3_backup_step(pBackup, -1), SQLITE_DONE);
        CHK_SQLITE(sqlite3_backup_finish(pBackup), SQLITE_OK);
        CHK_SQLITE(sqlite3_close(pBackupFile), SQLITE_OK);
        res = SSM_S_OK;
    }
    else
    {
        SSM_CLEANUP_ASSERT(executeSQL_NoReturn(strCreate_ModelRelationTable));
        SSM_CLEANUP_ASSERT(executeSQL_NoReturn(strCreate_DataRelationTable));

        SSM_CLEANUP_ASSERT(executeSQL_NoReturn(strRootModel));
        SSM_CLEANUP_ASSERT(executeSQL_NoReturn(tblRoot));
        SSM_CLEANUP_ASSERT(executeSQL_NoReturn(rootData));
        SSM_CLEANUP_ASSERT(executeSQL_NoReturn(rootRelation));
    }

CLEANUP:
    return res;
}

void CEvaluationEngine::terminateEngine()
{
    SSMRESULT           res = SSM_E_FAIL;
    sqlite3             *pBackupFile = NULL;
    sqlite3_backup      *pBackup = NULL;
    std::stringstream   sstream;

    //Remove all triggers on db side
    m_mtxTriggerId.lock();
    for (std::map<int, IEvaluationEngineEvent *>::iterator itor = m_mapTriggers.begin();
         itor != m_mapTriggers.end(); ++itor)
    {
        sstream << "drop trigger WatchInsertModel" << itor->first << ";" << std::ends;
        SSM_CLEANUP_ASSERT(executeSQL_NoReturn(sstream.str()));
        sstream.str("");

        sstream << "drop trigger WatchUpdateModel" << itor->first << ";" << std::ends;
        SSM_CLEANUP_ASSERT(executeSQL_NoReturn(sstream.str()));
        sstream.str("");
    }
    m_mtxTriggerId.unlock();

    if (strlen(LOCATION_SSM_DB_DUMP) > 0)
    {
        CHK_SQLITE(sqlite3_open(LOCATION_SSM_DB_DUMP, &pBackupFile), SQLITE_OK);
        pBackup = sqlite3_backup_init(pBackupFile, "main", m_pSQLite3, "main");
        CHK_SQLITE(sqlite3_errcode(pBackupFile), SQLITE_OK);
        SSM_CLEANUP_NULL_ASSERT(pBackup);
        CHK_SQLITE(sqlite3_backup_step(pBackup, -1), SQLITE_DONE);
        CHK_SQLITE(sqlite3_backup_finish(pBackup), SQLITE_OK);
        CHK_SQLITE(sqlite3_close(pBackupFile), SQLITE_OK);
    }

    CHK_SQLITE(sqlite3_close(m_pSQLite3), SQLITE_OK);

    m_pSQLite3 = NULL;

    res = SSM_S_OK;
CLEANUP:
    return;
}

SSMRESULT CEvaluationEngine::createModel(IN int parentModelId, IN const char *newModelName,
        IN ModelPropertyVec *pModelDescs, OUT int *pModelId)
{
    SSMRESULT res = SSM_E_FAIL;
    std::stringstream sstream;
    std::string         strSQL;
    sqlite3_stmt        *stmt = NULL;
    int         lPos = 0;

    //Check if same name exists
    sstream << "select modelId from [ModelRelation] where modelName='" << newModelName << "';" <<
            std::ends;
    strSQL = sstream.str();
    CHK_SQLITE(sqlite3_prepare_v2(m_pSQLite3, strSQL.c_str(), strSQL.length(), &stmt, NULL), SQLITE_OK);
    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        *pModelId = sqlite3_column_int(stmt, 0);
        CHK_SQLITE(sqlite3_finalize(stmt), SQLITE_OK);
        res = SSM_S_OK;
        goto CLEANUP;
    }
    CHK_SQLITE(sqlite3_finalize(stmt), SQLITE_OK);
    sstream.str("");

    sstream << "select lPos from [ModelRelation] where modelId = '" << parentModelId << "';" <<
            std::ends;
    SSM_CLEANUP_ASSERT(executeSQL_IntReturn(sstream.str(), &lPos));
    sstream.str("");

    sstream << "update [ModelRelation] set rPos = rPos + 2 where rPos > " << lPos << ";" << std::ends;
    SSM_CLEANUP_ASSERT(executeSQL_NoReturn(sstream.str()));
    sstream.str("");

    sstream << "update [ModelRelation] set lPos = lPos + 2 where lPos > " << lPos << ";" << std::ends;
    SSM_CLEANUP_ASSERT(executeSQL_NoReturn(sstream.str()));
    sstream.str("");

    sstream << "insert into [ModelRelation] values (null, '" << newModelName << "', " << lPos + 1 <<
            ", " << lPos + 2 <<
            ");" << std::ends;
    SSM_CLEANUP_ASSERT(executeSQL_NoReturn(sstream.str()));
    sstream.str("");

    SSM_CLEANUP_ASSERT(executeSQL_IntReturn("select last_insert_rowid();", pModelId));

    sstream << "create table [ModelData" << *pModelId <<
            "](dataId integer primary key autoincrement, lastTime NOT NULL DEFAULT CURRENT_TIMESTAMP, lifetime integer NOT NULL DEFAULT 2147483647, available text DEFAULT 'true'";

    for (ModelPropertyVec::iterator itor =
             pModelDescs->begin(); itor != pModelDescs->end(); ++itor)
    {
        sstream << "," << itor->propertyName;
        switch (itor->propertyType)
        {
            case ModelProperty::TYPE_NUMERIC:
                sstream << " numeric";
                break;

            case ModelProperty::TYPE_INTEGER:
                sstream << " integer";
                break;

            case ModelProperty::TYPE_REAL:
                sstream << " real";
                break;

            case ModelProperty::TYPE_TEXT:
                sstream << " text";
                break;

            default:
                res = SSM_E_FAIL;
                goto CLEANUP;
        }
    }

    sstream << ");" << std::ends;

    SSM_CLEANUP_ASSERT(executeSQL_NoReturn(sstream.str()));

CLEANUP:
    return res;
}

SSMRESULT CEvaluationEngine::addModelData(IN int modelId, IN int parentModelId, IN int parentDataId,
        IN ModelPropertyVec *pModelValues, OUT int *pDataId)
{
    SSMRESULT           res = SSM_E_FAIL;
    std::stringstream   sstream;
    std::string         strSQL;
    sqlite3_stmt        *stmt = NULL;
    int                 lPos = 0;
    unsigned int        i;

    sstream << "insert into [ModelData" << modelId << "] (";

    for (ModelPropertyVec::iterator itor = pModelValues->begin();
         itor != pModelValues->end(); ++itor)
    {
        sstream << itor->propertyName;

        if (itor < pModelValues->end() - 1)
        {
            sstream << ",";
        }
    }

    sstream << ") values (";

    for (i = 0; i < pModelValues->size(); i++)
    {
        sstream << "?";

        if (i < pModelValues->size() - 1)
        {
            sstream << ",";
        }
    }

    sstream << ");" << std::ends;

    strSQL = sstream.str();
    CHK_SQLITE(sqlite3_prepare_v2(m_pSQLite3, strSQL.c_str(), strSQL.length(), &stmt, NULL), SQLITE_OK);
    sstream.str("");

    i = 1;
    for (ModelPropertyVec::iterator itor = pModelValues->begin();
         itor != pModelValues->end(); ++itor)
    {
        switch (itor->propertyType)
        {
            case ModelProperty::TYPE_NUMERIC:
            case ModelProperty::TYPE_INTEGER:
                CHK_SQLITE(sqlite3_bind_int(stmt, i, atoi(itor->propertyValue.c_str())), SQLITE_OK);
                break;

            case ModelProperty::TYPE_REAL:
                CHK_SQLITE(sqlite3_bind_double(stmt, i, atof(itor->propertyValue.c_str())), SQLITE_OK);
                break;

            case ModelProperty::TYPE_TEXT:
                CHK_SQLITE(sqlite3_bind_text(stmt, i, itor->propertyValue.c_str(), itor->propertyValue.size(),
                                             SQLITE_STATIC), SQLITE_OK);
                break;

            default:
                res = SSM_E_FAIL;
                goto CLEANUP;
        }

        i++;
    }

    m_mtxDataRelation.lock();

    CHK_SQLITE(sqlite3_step(stmt), SQLITE_DONE);

    CHK_SQLITE(sqlite3_finalize(stmt), SQLITE_OK);

    SSM_CLEANUP_ASSERT(executeSQL_IntReturn("select last_insert_rowid();", pDataId));

    sstream << "select lPos from [DataRelation] where modelId = " << parentModelId << " and dataId = "
            << parentDataId << ";" << std::ends;
    SSM_CLEANUP_ASSERT(executeSQL_IntReturn(sstream.str(), &lPos));
    sstream.str("");

    sstream << "update [DataRelation] set rPos = rPos + 2 where rPos > " << lPos << ";" << std::ends;
    SSM_CLEANUP_ASSERT(executeSQL_NoReturn(sstream.str()));
    sstream.str("");

    sstream << "update [DataRelation] set lPos = lPos + 2 where lPos > " << lPos << ";" << std::ends;
    SSM_CLEANUP_ASSERT(executeSQL_NoReturn(sstream.str()));
    sstream.str("");

    sstream << "insert into [DataRelation] values (null, " << modelId << ", " << lPos + 1 << ", " <<
            lPos + 2 <<
            ", " << *pDataId << ");" << std::ends;
    SSM_CLEANUP_ASSERT(executeSQL_NoReturn(sstream.str()));

CLEANUP:
    m_mtxDataRelation.unlock();
    return res;
}

SSMRESULT CEvaluationEngine::updateModelData(IN int modelId, IN int dataId,
        IN ModelPropertyVec *pModelValues)
{
    SSMRESULT res = SSM_E_FAIL;
    std::stringstream sstream;
    std::string         strSQL;
    sqlite3_stmt        *stmt = NULL;
    unsigned int        i = 1;

    sstream << "update [ModelData" << modelId << "] set lastTime=CURRENT_TIMESTAMP, ";

    for (ModelPropertyVec::iterator itor = pModelValues->begin();
         itor != pModelValues->end(); ++itor)
    {
        sstream << itor->propertyName << "=?";

        if (itor < pModelValues->end() - 1)
        {
            sstream << ",";
        }
    }

    sstream << " where dataId = " << dataId << ";" << std::ends;

    strSQL = sstream.str();
    CHK_SQLITE(sqlite3_prepare_v2(m_pSQLite3, strSQL.c_str(), strSQL.length(), &stmt, NULL), SQLITE_OK);

    for (ModelPropertyVec::iterator itor = pModelValues->begin();
         itor != pModelValues->end(); ++itor)
    {
        switch (itor->propertyType)
        {
            case ModelProperty::TYPE_NUMERIC:
            case ModelProperty::TYPE_INTEGER:
                CHK_SQLITE(sqlite3_bind_int(stmt, i, atoi(itor->propertyValue.c_str())), SQLITE_OK);
                break;

            case ModelProperty::TYPE_REAL:
                CHK_SQLITE(sqlite3_bind_double(stmt, i, atof(itor->propertyValue.c_str())), SQLITE_OK);
                break;

            case ModelProperty::TYPE_TEXT:
                CHK_SQLITE(sqlite3_bind_text(stmt, i, itor->propertyValue.c_str(), itor->propertyValue.size(),
                                             SQLITE_STATIC), SQLITE_OK);
                break;

            default:
                res = SSM_E_FAIL;
                goto CLEANUP;
        }

        i++;
    }

    CHK_SQLITE(sqlite3_step(stmt), SQLITE_DONE);

    CHK_SQLITE(sqlite3_finalize(stmt), SQLITE_OK);

    res = SSM_S_OK;

CLEANUP:
    return res;
}
/*
SSMRESULT CEvaluationEngine::DeleteModel(IN int modelId)
{
    SSMRESULT       res = SSM_E_FAIL;
    std::stringstream sstream;
    std::string     strSQL;
    sqlite3_stmt    *stmt = NULL;
    int             lPos = 0;
    int             rPos = 0;
    int             width = 0;
    IntVec          dataIds;

    sstream << "select lPos, rPos from [ModelRelation] where modelId = " << modelId << ";" << std::ends;

    strSQL = sstream.str();
    sstream.str("");
    CHK_SQLITE(sqlite3_prepare_v2(m_pSQLite3, strSQL.c_str(), strSQL.length(), &stmt, NULL), SQLITE_OK);
    CHK_SQLITE(sqlite3_step(stmt), SQLITE_ROW);
    lPos = sqlite3_column_int(stmt, 0);
    rPos = sqlite3_column_int(stmt, 1);
    width = rPos - lPos + 1;
    CHK_SQLITE(sqlite3_finalize(stmt), SQLITE_OK);

    sstream << "delete from [ModelRelation] where lPos between " << lPos << " and " << rPos <<
        ";update [ModelRelation] set rPos = rPos - " << width << " where rPos > " << rPos <<
        ";update [ModelRelation] set lPos = lPos - " << width << " where lPos > " << lPos << std::ends;
    CHK_SSMRESULT(ExecuteSQL_NoReturn(sstream.str()));
    sstream.str("");

    sstream << "select dataId from [DataRelation] where modelId = " << modelId << ";" << std::ends;
    strSQL = sstream.str();
    CHK_SQLITE(sqlite3_prepare_v2(m_pSQLite3, strSQL.c_str(), strSQL.length(), &stmt, NULL), SQLITE_OK);
    while(sqlite3_step(stmt) == SQLITE_ROW)
    {
        dataIds.push_back(sqlite3_column_int(stmt, 0));
    }
    CHK_SQLITE(sqlite3_finalize(stmt), SQLITE_OK);

    for(std::vector<int>::iterator itor = dataIds.begin();
        itor != dataIds.end(); ++itor)
    {
        CHK_SSMRESULT(DeleteModelData(modelId, *itor));
    }

CLEANUP:
    return res;
}
*/
SSMRESULT CEvaluationEngine::deleteModelData(IN int modelId, IN int dataId)
{
    SSMRESULT res = SSM_E_FAIL;
    std::stringstream sstream;
    std::string strSQL;
    sqlite3_stmt *stmt = NULL;
    int             lPos = 0;
    int             rPos = 0;
    int             width = 0;

    m_mtxDataRelation.lock();

    sstream << "select lPos, rPos from [DataRelation] where modelId = " << modelId << " and dataId = "
            << dataId << ";" << std::ends;

    strSQL = sstream.str();
    sstream.str("");
    CHK_SQLITE(sqlite3_prepare_v2(m_pSQLite3, strSQL.c_str(), strSQL.length(), &stmt, NULL), SQLITE_OK);
    CHK_SQLITE(sqlite3_step(stmt), SQLITE_ROW);
    lPos = sqlite3_column_int(stmt, 0);
    rPos = sqlite3_column_int(stmt, 1);
    width = rPos - lPos + 1;
    CHK_SQLITE(sqlite3_finalize(stmt), SQLITE_OK);

    sstream << "delete from [DataRelation] where lPos between " << lPos << " and " << rPos << ";" <<
            std::ends;
    SSM_CLEANUP_ASSERT(executeSQL_NoReturn(sstream.str()));
    sstream.str("");

    sstream << "update [DataRelation] set rPos = rPos - " << width << " where rPos > " << rPos << ";" <<
            std::ends;
    SSM_CLEANUP_ASSERT(executeSQL_NoReturn(sstream.str()));
    sstream.str("");

    sstream << "update [DataRelation] set lPos = lPos - " << width << " where lPos > " << lPos << ";" <<
            std::ends;
    SSM_CLEANUP_ASSERT(executeSQL_NoReturn(sstream.str()));
    sstream.str("");

    sstream << "delete from [ModelData" << modelId << "] where dataId = " << dataId << ";" << std::ends;
    SSM_CLEANUP_ASSERT(executeSQL_NoReturn(sstream.str()));

CLEANUP:
    m_mtxDataRelation.unlock();
    return res;
}

SSMRESULT CEvaluationEngine::getModelDataSet(IN int modelId, IN int startIndex, IN int count,
        OUT std::vector<ModelPropertyVec> *pDataSet, OUT int *pLastIndex)
{
    SSMRESULT res = SSM_E_FAIL;
    std::stringstream sstream;
    std::string     strSQL;
    int             columnCount = 0;
    sqlite3_stmt    *stmt = NULL;
    const char      *textData = NULL;

    sstream << "select * from [ModelData" << modelId << "] where dataId > " << startIndex << " limit "
            << count << ";" << std::ends;

    strSQL = sstream.str();
    CHK_SQLITE(sqlite3_prepare_v2(m_pSQLite3, strSQL.c_str(), strSQL.length(), &stmt, NULL), SQLITE_OK);

    *pLastIndex = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        ModelPropertyVec modelProperties;

        columnCount = sqlite3_column_count(stmt);
        for (int i = 0; i < columnCount; i++)
        {
            ModelProperty modelData;
            switch (sqlite3_column_type(stmt, i))
            {
                case SQLITE_INTEGER:
                    modelData.propertyType = ModelProperty::TYPE_INTEGER;
                    sstream.str("");
                    sstream << sqlite3_column_int(stmt, i) << std::ends;
                    modelData.propertyValue = sstream.str();
                    break;

                case SQLITE_FLOAT:
                    modelData.propertyType = ModelProperty::TYPE_REAL;
                    sstream.str("");
                    sstream << sqlite3_column_double(stmt, i) << std::ends;
                    modelData.propertyValue = sstream.str();
                    break;

                case SQLITE_BLOB:
                case SQLITE_TEXT:
                    modelData.propertyType = ModelProperty::TYPE_TEXT;
                    textData = (const char *)sqlite3_column_text(stmt, i);
                    if (textData != NULL)
                    {
                        modelData.propertyValue = textData;
                    }
                    break;

                default:
                    modelData.propertyType = ModelProperty::TYPE_TEXT;
                    textData = (const char *)sqlite3_column_text(stmt, i);
                    if (textData != NULL)
                    {
                        modelData.propertyValue = textData;
                    }
                    break;
            };
            modelProperties.push_back(modelData);
        }

        pDataSet->push_back(modelProperties);
        *pLastIndex = sqlite3_column_int(stmt, 0);
    }

    CHK_SQLITE(sqlite3_finalize(stmt), SQLITE_OK);

    res = SSM_S_OK;

CLEANUP:
    return res;
}

SSMRESULT CEvaluationEngine::getModelData(IN int modelId, IN int dataId,
        OUT ModelPropertyVec *pModelProperties)
{
    SSMRESULT       res = SSM_E_FAIL;
    std::stringstream sstream;
    std::string     strSQL;
    int             columnCount = 0;
    sqlite3_stmt    *stmt = NULL;
    const char      *textData = NULL;

    sstream << "select * from [ModelData" << modelId << "] where dataId=" << dataId << ";" << std::ends;

    strSQL = sstream.str();
    CHK_SQLITE(sqlite3_prepare_v2(m_pSQLite3, strSQL.c_str(), strSQL.length(), &stmt, NULL), SQLITE_OK);

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        columnCount = sqlite3_column_count(stmt);
        for (int i = 0; i < columnCount; i++)
        {
            ModelProperty modelData;
            switch (sqlite3_column_type(stmt, i))
            {
                case SQLITE_INTEGER:
                    modelData.propertyType = ModelProperty::TYPE_INTEGER;
                    sstream.str("");
                    sstream << sqlite3_column_int(stmt, i) << std::ends;
                    modelData.propertyValue = sstream.str();
                    break;

                case SQLITE_FLOAT:
                    modelData.propertyType = ModelProperty::TYPE_REAL;
                    sstream.str("");
                    sstream << sqlite3_column_double(stmt, i) << std::ends;
                    modelData.propertyValue = sstream.str();
                    break;

                case SQLITE_BLOB:
                case SQLITE_TEXT:
                    modelData.propertyType = ModelProperty::TYPE_TEXT;
                    textData = (const char *)sqlite3_column_text(stmt, i);
                    if (textData != NULL)
                    {
                        modelData.propertyValue = textData;
                    }
                    break;

                default:
                    modelData.propertyType = ModelProperty::TYPE_TEXT;
                    textData = (const char *)sqlite3_column_text(stmt, i);
                    if (textData != NULL)
                    {
                        modelData.propertyValue = textData;
                    }
                    break;
            };
            pModelProperties->push_back(modelData);
        }
    }

    CHK_SQLITE(sqlite3_finalize(stmt), SQLITE_OK);

    res = SSM_S_OK;
CLEANUP:
    return res;
}
/*
SSMRESULT CEvaluationEngine::GetModelSchema(IN int modelId, OUT ModelPropertyVec *pModelProperties)
{
    SSMRESULT       res = SSM_E_FAIL;
    std::stringstream sstream;
    std::string     strSQL;
    sqlite3_stmt    *stmt = NULL;
    std::string     propertyType;

    sstream << "pragma table_info('ModelData" << modelId << "');" << std::ends;

    strSQL = sstream.str();
    CHK_SQLITE(sqlite3_prepare_v2(m_pSQLite3, strSQL.c_str(), strSQL.length(), &stmt, NULL), SQLITE_OK);

    while(sqlite3_step(stmt) == SQLITE_ROW)
    {
        ModelProperty modelData;
        modelData.propertyName = (const char*)sqlite3_column_text(stmt, 1);
        propertyType = (const char*)sqlite3_column_text(stmt, 2);

        if(propertyType == "integer")
        {
            modelData.propertyType = ModelProperty::Type_INTEGER;
        }
        else if(propertyType == "int")
        {
            modelData.propertyType = ModelProperty::Type_INTEGER;
        }
        else if(propertyType == "real")
        {
            modelData.propertyType = ModelProperty::Type_REAL;
        }
        else if(propertyType == "text")
        {
            modelData.propertyType = ModelProperty::Type_TEXT;
        }
        else
        {
            modelData.propertyType = ModelProperty::Type_TEXT;
        }
        pModelProperties->push_back(modelData);
    }

    CHK_SQLITE(sqlite3_finalize(stmt), SQLITE_OK);

    res = SSM_S_OK;
CLEANUP:
    return res;
}
*/
SSMRESULT CEvaluationEngine::getConditionedModelData(IN int modelId,
        IN ModelConditionVec *pModelConditions, OUT IntVec *pDataIds)
{
    SSMRESULT res = SSM_E_FAIL;
    std::stringstream sstream;
    std::string strSQL;
    sqlite3_stmt *stmt = NULL;

    sstream << "select dataId from [ModelData" << modelId <<
            "] where CURRENT_TIMESTAMP < datetime( strftime('%s', lastTime) + lifetime, 'unixepoch') and ";

    for (ModelConditionVec::iterator itor = pModelConditions->begin();
         itor != pModelConditions->end(); ++itor)
    {
        sstream << itor->modelProperty.propertyName;
        switch (itor->predicate)
        {
            case ModelCondition::PREDICATE_EQ:
                sstream << "=";
                break;

            case ModelCondition::PREDICATE_NEQ:
                sstream << "!=";
                break;

            case ModelCondition::PREDICATE_GT:
                sstream << ">";
                break;

            case ModelCondition::PREDICATE_LT:
                sstream << "<";
                break;

            case ModelCondition::PREDICATE_GTE:
                sstream << ">=";
                break;

            case ModelCondition::PREDICATE_LTE:
                sstream << "<=";
                break;

            default:
                res = SSM_E_FAIL;
                goto CLEANUP;
        }

        switch (itor->modelProperty.propertyType)
        {
            case ModelProperty::TYPE_NUMERIC:
            case ModelProperty::TYPE_INTEGER:
            case ModelProperty::TYPE_REAL:
                sstream << itor->modelProperty.propertyValue;
                break;

            case ModelProperty::TYPE_TEXT:
                sstream << "'" << itor->modelProperty.propertyValue << "'";
                break;

            default:
                res = SSM_E_FAIL;
                goto CLEANUP;
        }

        if (itor < pModelConditions->end() - 1)
        {
            sstream << " and ";
        }
    }

    sstream << ";" << std::ends;

    strSQL = sstream.str();
    CHK_SQLITE(sqlite3_prepare_v2(m_pSQLite3, strSQL.c_str(), strSQL.length(), &stmt, NULL), SQLITE_OK);

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        pDataIds->push_back(sqlite3_column_int(stmt, 0));
    }

    CHK_SQLITE(sqlite3_finalize(stmt), SQLITE_OK);

    res = SSM_S_OK;

CLEANUP:
    return res;
}

SSMRESULT CEvaluationEngine::watchModelData(IN int modelId, IN ModelConditionVec *pModelConditions,
        IN IEvaluationEngineEvent *pEvaluationEngineEvent, OUT int *pTriggerId)
{
    SSMRESULT res = SSM_E_FAIL;
    std::stringstream sstream;
    std::stringstream sstreamCondition;

    for (ModelConditionVec::iterator itor = pModelConditions->begin();
         itor != pModelConditions->end(); ++itor)
    {
        sstreamCondition << "NEW." << itor->modelProperty.propertyName;
        switch (itor->predicate)
        {
            case ModelCondition::PREDICATE_EQ:
                sstreamCondition << "=";
                break;

            case ModelCondition::PREDICATE_NEQ:
                sstreamCondition << "!=";
                break;

            case ModelCondition::PREDICATE_GT:
                sstreamCondition << ">";
                break;

            case ModelCondition::PREDICATE_LT:
                sstreamCondition << "<";
                break;

            case ModelCondition::PREDICATE_GTE:
                sstreamCondition << ">=";
                break;

            case ModelCondition::PREDICATE_LTE:
                sstreamCondition << "<=";
                break;

            default:
                res = SSM_E_FAIL;
                goto CLEANUP;
        }

        switch (itor->modelProperty.propertyType)
        {
            case ModelProperty::TYPE_NUMERIC:
            case ModelProperty::TYPE_INTEGER:
            case ModelProperty::TYPE_REAL:
                sstreamCondition << itor->modelProperty.propertyValue;
                break;

            case ModelProperty::TYPE_TEXT:
                sstreamCondition << "'" << itor->modelProperty.propertyValue << "'";
                break;

            default:
                res = SSM_E_FAIL;
                goto CLEANUP;
        }

        if (itor < pModelConditions->end() - 1)
        {
            sstreamCondition << " and ";
        }
    }

    sstreamCondition << std::ends;

    m_mtxDataRelation.lock();

    sstream << "CREATE TRIGGER WatchInsertModel" << m_iTriggerId << " AFTER INSERT ON [ModelData" <<
            modelId << "] WHEN ";
    sstream << sstreamCondition.str().c_str() << " BEGIN SELECT OnSQLTrigger(" <<
            reinterpret_cast<intptr_t>(this) << ", " << m_iTriggerId << ", NEW.dataId); END;" << std::ends;
    SSM_CLEANUP_ASSERT(executeSQL_NoReturn(sstream.str()));
    sstream.str("");

    sstream << "CREATE TRIGGER WatchUpdateModel" << m_iTriggerId << " AFTER UPDATE ON [ModelData" <<
            modelId << "] WHEN ";
    sstream << sstreamCondition.str().c_str() << " BEGIN SELECT OnSQLTrigger(" <<
            reinterpret_cast<intptr_t>(this) << ", " << m_iTriggerId << ", NEW.dataId); END;" << std::ends;
    SSM_CLEANUP_ASSERT(executeSQL_NoReturn(sstream.str()));

    m_mtxTriggerId.lock();
    pEvaluationEngineEvent->addRef();
    m_mapTriggers[m_iTriggerId] = pEvaluationEngineEvent;
    *pTriggerId = m_iTriggerId++;

CLEANUP:
    m_mtxTriggerId.unlock();
    m_mtxDataRelation.unlock();
    return res;
}

SSMRESULT CEvaluationEngine::dropWatchModelData(IN int triggerId)
{
    SSMRESULT res = SSM_E_FAIL;
    std::stringstream sstream;

    m_mtxDataRelation.lock();

    sstream << "drop trigger WatchInsertModel" << triggerId << ";" << std::ends;
    SSM_CLEANUP_ASSERT(executeSQL_NoReturn(sstream.str()));
    sstream.str("");

    sstream << "drop trigger WatchUpdateModel" << triggerId << ";" << std::ends;
    SSM_CLEANUP_ASSERT(executeSQL_NoReturn(sstream.str()));

    m_mtxTriggerId.lock();
    m_mapTriggers[triggerId]->release();
    m_mapTriggers.erase(triggerId);

CLEANUP:
    m_mtxTriggerId.unlock();
    m_mtxDataRelation.unlock();
    return res;
}

SSMRESULT CEvaluationEngine::getParentDataId(IN int modelId, IN int dataId, IN int parentModelId,
        OUT int *pParentDataId)
{
    SSMRESULT res = SSM_E_FAIL;
    std::stringstream sstream;

    sstream <<
            "select parent.dataId from [DataRelation] as node, [DataRelation] as parent where parent.modelId="
            <<
            parentModelId << " and node.modelId=" << modelId << " and node.dataId=" <<
            dataId << " and node.lPos between parent.lPos and parent.rPos;" << std::ends;

    m_mtxDataRelation.lock();
    SSM_CLEANUP_ASSERT(executeSQL_IntReturn(sstream.str(), pParentDataId));

CLEANUP:
    m_mtxDataRelation.unlock();
    return res;
}

SSMRESULT CEvaluationEngine::getChildDataId(IN int modelId, IN int dataId, IN int ChildModelId,
        OUT IntVec *pChildDataIds)
{
    SSMRESULT res = SSM_E_FAIL;
    std::stringstream sstream;
    std::string strSQL;
    sqlite3_stmt *stmt = NULL;

    sstream <<
            "select child.dataId from [DataRelation] as node, [DataRelation] as child where child.modelId=" <<
            ChildModelId << " and node.modelId=" << modelId << " and node.dataId=" <<
            dataId << " and child.lPos between node.lPos and node.rPos order by child.dataId;" << std::ends;

    strSQL = sstream.str();
    sstream.str("");

    m_mtxDataRelation.lock();
    CHK_SQLITE(sqlite3_prepare_v2(m_pSQLite3, strSQL.c_str(), strSQL.length(), &stmt, NULL), SQLITE_OK);

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        pChildDataIds->push_back(sqlite3_column_int(stmt, 0));
    }

    CHK_SQLITE(sqlite3_finalize(stmt), SQLITE_OK);
    res = SSM_S_OK;

CLEANUP:
    m_mtxDataRelation.unlock();
    return res;
}
/*
SSMRESULT CEvaluationEngine::GetPathToRoot(IN int currentModelId, OUT IntVec *pPath)
{
    SSMRESULT res = SSM_E_FAIL;
    std::stringstream sstream;
    std::string strSQL;
    sqlite3_stmt *stmt = NULL;

    sstream << "select parent.modelId from [ModelRelation] as node, [ModelRelation] as parent where " <<
        "node.lPos between parent.lPos and parent.rPos and node.modelId = " << currentModelId << " order by node.lPos;" << std::ends;

    strSQL = sstream.str();
    sstream.str("");
    CHK_SQLITE(sqlite3_prepare_v2(m_pSQLite3, strSQL.c_str(), strSQL.length(), &stmt, NULL), SQLITE_OK);

    while(sqlite3_step(stmt) == SQLITE_ROW)
    {
        pPath->push_back(sqlite3_column_int(stmt, 0));
    }

    CHK_SQLITE(sqlite3_finalize(stmt), SQLITE_OK);

    res = SSM_S_OK;
CLEANUP:
    return res;
}

SSMRESULT CEvaluationEngine::GetModelId(IN const char *modelName, OUT int *pModelId)
{
    SSMRESULT res = SSM_E_FAIL;
    std::stringstream sstream;

    sstream << "select modelId from [ModelRelation] where modelName = '" << modelName << "';" << std::ends;

    CHK_SSMRESULT(ExecuteSQL_IntReturn(sstream.str(), pModelId));

CLEANUP:
    return res;
}

SSMRESULT CEvaluationEngine::GetModelHierarchy(IN int rootModelId, OUT StringVec *pPath)
{
    SSMRESULT res = SSM_E_FAIL;
    std::stringstream sstream;
    std::string strSQL;
    sqlite3_stmt *stmt = NULL;
    StringVec   lastModels;
    int         currIndex = 0;
    int         lastIndex = 0;

    sstream << "select node.modelName, (COUNT(parent.modelName) - (sub_tree.depth + 1)) as depth from [ModelRelation] as node, "\
        "[ModelRelation] as parent, [ModelRelation] as sub_parent, (select node.modelName, (count(parent.modelName) - 1) as depth from "\
        "[ModelRelation] as node, [ModelRelation] as parent where node.lPos between parent.lPos and parent.rPos and node.modelId = " << rootModelId <<
        " group by node.lPos, node.modelName) as sub_tree where node.lPos between parent.lPos and parent.rPos and node.lPos between "<<
        "sub_parent.lPos and sub_parent.rPos and sub_parent.modelName = sub_tree.modelName group by node.lPos, node.modelName, sub_tree.depth " <<
        "order by node.lPos;" << std::ends;

    strSQL = sstream.str();
    sstream.str("");
    CHK_SQLITE(sqlite3_prepare_v2(m_pSQLite3, strSQL.c_str(), strSQL.length(), &stmt, NULL), SQLITE_OK);

    while(sqlite3_step(stmt) == SQLITE_ROW)
    {
        currIndex = sqlite3_column_int(stmt, 1);

        if(currIndex < lastIndex)

        {
            for(int i=0; i < lastIndex - currIndex + 1; i++)
            {
                lastModels.pop_back();
            }
        }

        lastModels.push_back((const char*)sqlite3_column_text(stmt, 0));

        for(StringVec::iterator itor = lastModels.begin();
            itor != lastModels.end(); ++itor)
        {
            sstream << *itor;

            if(itor < lastModels.end() - 1)
            {
                sstream << ".";
            }
        }

        sstream << std::ends;

        pPath->push_back(sstream.str());

        sstream.str("");

        lastIndex = currIndex;
    }

    CHK_SQLITE(sqlite3_finalize(stmt), SQLITE_OK);

    res = SSM_S_OK;
CLEANUP:
    return res;
}

SSMRESULT CEvaluationEngine::GetDataHierarchy(IN int rootModelId, IN int dataId, OUT StringVec *pPath)
{
    SSMRESULT res = SSM_E_FAIL;
    std::stringstream sstream;
    std::string strSQL;
    sqlite3_stmt *stmt = NULL;

    sstream << "select [ModelRelation].modelName, node.dataId, (COUNT(parent.modelId) - (sub_tree.depth + 1)) as depth "\
        "from [ModelRelation], [DataRelation] as node, [DataRelation] as parent, [DataRelation] as sub_parent, "\
        "(select node.modelId, (count(parent.modelId) - 1) as depth from [DataRelation] as node, [DataRelation] as parent "\
        "where node.lPos between parent.lPos and parent.rPos and node.modelId = " << rootModelId <<
        " and node.dataId = " << dataId << " group by node.lPos, node.modelId) as sub_tree "\
        "where node.lPos between parent.lPos and parent.rPos and node.lPos between sub_parent.lPos and sub_parent.rPos and "\
        "sub_parent.modelId = sub_tree.modelId and [ModelRelation].modelId = node.modelId "\
        "group by node.lPos, [ModelRelation].modelName, node.dataId, sub_tree.depth order by node.lPos;" << std::ends;

    strSQL = sstream.str();
    sstream.str("");
    CHK_SQLITE(sqlite3_prepare_v2(m_pSQLite3, strSQL.c_str(), strSQL.length(), &stmt, NULL), SQLITE_OK);

    while(sqlite3_step(stmt) == SQLITE_ROW)
    {
        sstream << sqlite3_column_text(stmt, 0) << " " << sqlite3_column_int(stmt, 1) << " " << sqlite3_column_int(stmt, 2) << std::ends;
        pPath->push_back(sstream.str());
        sstream.str("");
    }

    CHK_SQLITE(sqlite3_finalize(stmt), SQLITE_OK);

    res = SSM_S_OK;
CLEANUP:
    return res;
}
*/
