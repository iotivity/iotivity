//******************************************************************
//
// Copyright 2014 Samsung Electronics All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#include "SqliteSubscriptionDatabase.h"
#include "sstream"
#include "sqlite3.h"
#include <iostream>
#include "log.h"


using namespace std;

namespace OC
{
    namespace Cm
    {
        namespace Notification
        {

            SqliteSubscriptionDatabase::SqliteSubscriptionDatabase()
            {
                pDBHandle = NULL ;
            }

            SqliteSubscriptionDatabase::~SqliteSubscriptionDatabase()
            {
                this->deinit();
            }

            bool SqliteSubscriptionDatabase::init(std::string &dbPath)
            {
                if ( dbPath.empty() )
                {
                    return false ;
                }

                if (sqlite3_open(dbPath.c_str(), &pDBHandle) != SQLITE_OK)
                {
                    return false ;
                }

                std::string sqlStatementStr = "CREATE TABLE IF NOT EXISTS MySubscriptions ( requestId INTEGER, deviceId TEXT , subscribedResource TEXT, subscriptionUri TEXT, notificationUri TEXT )"  ;
                if (false == executeSQLStatement(sqlStatementStr))
                {
                    return false ;
                }

                sqlStatementStr = "CREATE TABLE IF NOT EXISTS Subscriptions ( deviceId TEXT , subscribedResource TEXT, subscriptionUri TEXT, notificationUri TEXT )"  ;
                if (false == executeSQLStatement(sqlStatementStr))
                {
                    return false ;
                }

                // Testing only ..

                CE(cout << "Sql DB::clean()" << std::endl);

                clean();

                return true ;
            }

            bool
            SqliteSubscriptionDatabase::deinit()
            {
                if (pDBHandle == NULL)
                {
                    return true ;
                }

                if (SQLITE_OK != sqlite3_close(pDBHandle))
                {
                    return false ;
                }

                pDBHandle = NULL ;

                return true ;
            }

            bool
            SqliteSubscriptionDatabase::savePendingMySubscription( int requestId, OC::Cm::Notification::SubscriptionRecord &subscription )
            {
                bool bRet = false ;

                if ( NULL == pDBHandle )
                {
                    return false ;
                }

                sqlite3_mutex_enter(sqlite3_db_mutex(pDBHandle)) ;

                std::ostringstream sqlStatementStream ;
                sqlStatementStream << "INSERT OR REPLACE INTO MySubscriptions ( requestId, deviceId, subscribedResource, subscriptionUri, notificationUri ) VALUES " ;
                sqlStatementStream << "(" << requestId ;
                sqlStatementStream << ", '" << subscription.m_deviceId << "'" ;
                sqlStatementStream << ", '" << subscription.m_subscribedResource << "'" ;
                sqlStatementStream << ", '" << subscription.m_subscriptionUri << "'" ;
                sqlStatementStream << ", '" << subscription.m_noificationUri << "');" ;

                bRet = executeSQLStatement(sqlStatementStream.str()) ;

                subscription.m_rowId = sqlite3_last_insert_rowid(pDBHandle) ;

                sqlite3_mutex_leave(sqlite3_db_mutex(pDBHandle)) ;

                return bRet ;
            }

            bool
            SqliteSubscriptionDatabase::deletePendingMySubscription( int requestId )
            {
                std::ostringstream sqlStatementStream ;
                sqlStatementStream << "DELETE FROM MySubscriptions WHERE requestId=" << requestId ;

                return executeSQLStatement(sqlStatementStream.str()) ;
            }

            bool
            SqliteSubscriptionDatabase::updateMySubscription(int requestId, std::string &subscriptionUri )
            {
                std::ostringstream sqlStatementStream ;
                sqlStatementStream << "UPDATE MySubscriptions SET requestId=-1, subscriptionUri='" << subscriptionUri << "' WHERE requestId=" << requestId;

                return executeSQLStatement(sqlStatementStream.str()) ;
            }

            bool
            SqliteSubscriptionDatabase::updateMySubscriptionBySubscriptionUri( std::string &subscriptionUri, std::string &resourcePaths )
            {
                return executeSQLStatement("UPDATE MySubscriptions SET subscribedResource='" + resourcePaths + "' WHERE subscriptionUri= '" + subscriptionUri + "'") ;
            }

            bool
            SqliteSubscriptionDatabase::deleteMySubscriptionBySubscriptionUri(std::string &deviceId, std::string &subscriptionUri)
            {
                return executeSQLStatement("DELETE FROM MySubscriptions WHERE deviceId='" + deviceId + "' AND subscriptionUri='" + subscriptionUri + "' ") ;
            }

            void
            SqliteSubscriptionDatabase::clean()
            {
                CE(std::cout << "SqliteSubscriptionDatabase::clean() called => Cleaning DB" << std::endl);
                bool sink = executeSQLStatement("DELETE FROM MySubscriptions");
                if (!sink)
                {
                    std::cout << "Error #1" << std::endl;
                }

                bool sink2 = executeSQLStatement("DELETE FROM Subscriptions");
                if (!sink2)
                {
                    std::cout << "Error #2" << std::endl;
                }

                return;
            }

            bool
            SqliteSubscriptionDatabase::saveSubscription(OC::Cm::Notification::SubscriptionRecord &subscription)
            {
                bool bRet = false ;

                if ( NULL == pDBHandle )
                {
                    return bRet ;
                }

                std::string resourcePath = subscription.m_subscribedResource;
                std::string devId = subscription.m_deviceId;
                std::string notifUri = subscription.m_noificationUri;

                std::string statement;
                if (resourcePath.size() <= 0)
                    return false;
                statement.append("SELECT deviceId, subscribedResource, subscriptionUri, notificationUri, ROWID FROM Subscriptions WHERE ");


                std::string queryString ;

                queryString.append("( deviceId = '");
                queryString.append(devId);
                queryString.append("'  OR deviceId LIKE '% ");
                queryString.append(devId);
                queryString.append("'  OR deviceId LIKE '");
                queryString.append(devId);
                queryString.append(" %' OR deviceId LIKE '% ");
                queryString.append(devId);
                queryString.append(" %' ) ");

                queryString.append( " AND " );

                queryString.append("( notificationUri = '");
                queryString.append(notifUri);
                queryString.append("'  OR notificationUri LIKE '% ");
                queryString.append(notifUri);
                queryString.append("'  OR notificationUri LIKE '");
                queryString.append(notifUri);
                queryString.append(" %' OR notificationUri LIKE '% ");
                queryString.append(notifUri);
                queryString.append(" %' ) ");


                std::list<std::string> resourcePathList = getListfromString(resourcePath);

                for ( std::list<std::string>::const_iterator itr = resourcePathList.begin() ; itr != resourcePathList.end() ; itr++)
                {
                    std::string queryfrag ;


                    queryfrag.append("( subscribedResource = '");
                    queryfrag.append((*itr));
                    queryfrag.append("'  OR subscribedResource LIKE '% ");
                    queryfrag.append((*itr));
                    queryfrag.append("'  OR subscribedResource LIKE '");
                    queryfrag.append((*itr));
                    queryfrag.append(" %' OR subscribedResource LIKE '% ");
                    queryfrag.append((*itr));
                    queryfrag.append(" %' ) ");



                    if (queryString.empty())
                    {
                        queryString.assign(queryfrag) ;
                    }
                    else
                    {
                        queryString.append( " AND " );
                        queryString.append( queryfrag );
                    }
                }

                std::list<OC::Cm::Notification::SubscriptionRecord> subscriptions;
                std::string message = statement + queryString;

                int newRowId = 0;

                for ( std::list<OC::Cm::Notification::SubscriptionRecord>::const_iterator itr = subscriptions.begin(); itr != subscriptions.end(); itr++)
                {
                    if ((*itr).m_subscribedResource.length() == resourcePath.length())
                    {
                        newRowId = (*itr).m_rowId;
                    }
                }

                if (newRowId > 0)
                {
                    subscription.m_rowId = newRowId;
                    return true;
                }

                sqlite3_mutex_enter(sqlite3_db_mutex(pDBHandle)) ;

                std::ostringstream sqlStatementStream ;
                sqlStatementStream << "INSERT OR REPLACE INTO Subscriptions ( deviceId, subscribedResource, subscriptionUri, notificationUri ) VALUES " ;
                sqlStatementStream << "( '" << subscription.m_deviceId << "'" ;
                sqlStatementStream << ", '" << subscription.m_subscribedResource << "'" ;
                sqlStatementStream << ", '" << subscription.m_subscriptionUri << "'" ;
                sqlStatementStream << ", '" << subscription.m_noificationUri << "');" ;

                bRet = executeSQLStatement(sqlStatementStream.str()) ;

                subscription.m_rowId = sqlite3_last_insert_rowid(pDBHandle) ;

                sqlite3_mutex_leave(sqlite3_db_mutex(pDBHandle)) ;

                return bRet ;
            }

            bool
            SqliteSubscriptionDatabase::updateSubscription(OC::Cm::Notification::SubscriptionRecord &subscription)
            {
                std::ostringstream sqlStatementStream ;
                sqlStatementStream << "UPDATE Subscriptions SET deviceId='" << subscription.m_deviceId << "' , subscribedResource='" << subscription.m_subscribedResource << "' , notificationUri='" << subscription.m_noificationUri << "' WHERE ROWID= " << subscription.m_rowId;

                return executeSQLStatement(sqlStatementStream.str()) ;
            }

            bool
            SqliteSubscriptionDatabase::deleteSubscriptionBySubscriptionID(std::string &subscriptionId)
            {
                return executeSQLStatement("DELETE FROM Subscriptions WHERE ROWID=" + subscriptionId ) ;
            }

            bool
            SqliteSubscriptionDatabase::getSubscriptionContainingResource( std::string &resourcePath, std::list<OC::Cm::Notification::SubscriptionRecord>& subscriptions )
            {
                return executeSQLSelectStatement("SELECT deviceId, subscribedResource, subscriptionUri, notificationUri, ROWID FROM Subscriptions WHERE subscribedResource = '" + resourcePath + "'  OR subscribedResource LIKE '% " + resourcePath + "'  OR subscribedResource LIKE '" + resourcePath + " %' OR subscribedResource LIKE '% " + resourcePath + " %'", subscriptions) ;
            }

            bool
            SqliteSubscriptionDatabase::getSubscriptionContainingResourceList( std::list<std::string> resourcePathList, std::list<OC::Cm::Notification::SubscriptionRecord>& subscriptions )
            {
                std::string statement;
                if (resourcePathList.size() <= 0)
                    return false;
                statement.append("SELECT deviceId, subscribedResource, subscriptionUri, notificationUri, ROWID FROM Subscriptions WHERE ");


                std::string queryString ;

                for ( std::list<std::string>::const_iterator itr = resourcePathList.begin() ; itr != resourcePathList.end() ; itr++)
                {
                    std::string queryfrag ;

                    queryfrag.append("( subscribedResource = '");
                    queryfrag.append((*itr));
                    queryfrag.append("'  OR subscribedResource LIKE '% ");
                    queryfrag.append((*itr));
                    queryfrag.append("'  OR subscribedResource LIKE '");
                    queryfrag.append((*itr));
                    queryfrag.append(" %' OR subscribedResource LIKE '% ");
                    queryfrag.append((*itr));
                    queryfrag.append(" %' ) ");

                    if (queryString.empty())
                    {
                        queryString.assign(queryfrag) ;
                    }
                    else
                    {
                        queryString.append( " OR " );
                        queryString.append( queryfrag );
                    }
                }

                return executeSQLSelectStatement( statement + queryString, subscriptions ) ;
            }

            bool
            SqliteSubscriptionDatabase::getMySubscriptionContainingResourceList( std::string deviceUUID, std::list<std::string> resourcePathList, std::list<OC::Cm::Notification::SubscriptionRecord>& subscriptions )
            {
                std::string statement;
                if (resourcePathList.size() <= 0)
                    return false;
                statement.append("SELECT deviceId, subscribedResource, subscriptionUri, notificationUri, ROWID FROM MySubscriptions WHERE deviceId = '" + deviceUUID + "' AND");


                std::string queryString ;

                for ( std::list<std::string>::const_iterator itr = resourcePathList.begin() ; itr != resourcePathList.end() ; itr++)
                {
                    std::string queryfrag ;

                    queryfrag.append("( subscribedResource = '");
                    queryfrag.append((*itr));
                    queryfrag.append("'  OR subscribedResource LIKE '% ");
                    queryfrag.append((*itr));
                    queryfrag.append("'  OR subscribedResource LIKE '");
                    queryfrag.append((*itr));
                    queryfrag.append(" %' OR subscribedResource LIKE '% ");
                    queryfrag.append((*itr));
                    queryfrag.append(" %' ) ");

                    if (queryString.empty())
                    {
                        queryString.assign(queryfrag) ;
                    }
                    else
                    {
                        queryString.append( " OR " );
                        queryString.append( queryfrag );
                    }
                }

                return executeSQLSelectStatement( statement + "( " + queryString + " )", subscriptions ) ;
            }

            bool
            SqliteSubscriptionDatabase::getMySubscriptionByDeviceId( std::string &deviceId, std::list<OC::Cm::Notification::SubscriptionRecord>& subscriptions )
            {
                return executeSQLSelectStatement("SELECT deviceId, subscribedResource, subscriptionUri, notificationUri, ROWID FROM MySubscriptions WHERE deviceId = '" + deviceId + "'", subscriptions) ;
            }

            bool
            SqliteSubscriptionDatabase::getMySubscriptionByResourcePaths( std::string &deviceId, std::string &subscribedResourcePath,
                    std::list<OC::Cm::Notification::SubscriptionRecord>& subscriptionList)
            {
                return executeSQLSelectStatement("SELECT deviceId, subscribedResource, subscriptionUri, notificationUri, ROWID FROM MySubscriptions WHERE deviceId='" + deviceId + "' AND subscribedResource='" + subscribedResourcePath + "' ", subscriptionList) ;
            }

            bool
            SqliteSubscriptionDatabase::getSubscriptions(std::list<OC::Cm::Notification::SubscriptionRecord>& subscriptions)
            {
                return executeSQLSelectStatement("SELECT deviceId, subscribedResource, subscriptionUri, notificationUri, ROWID FROM Subscriptions", subscriptions) ;
            }

            bool
            SqliteSubscriptionDatabase::getSubscriptionBySubscriptionID( std::string &subscriptionId,
                    OC::Cm::Notification::SubscriptionRecord &subscription)
            {
                std::list<OC::Cm::Notification::SubscriptionRecord> subscriptions ;

                if (false == executeSQLSelectStatement("SELECT deviceId, subscribedResource, subscriptionUri, notificationUri, ROWID FROM Subscriptions WHERE ROWID='" + subscriptionId + "' ", subscriptions))
                {
                    return false ;
                }

                if (subscriptions.size() == 0)
                {
                    return false ;
                }

                subscription = subscriptions.front() ;
                return true ;
            }

            bool
            SqliteSubscriptionDatabase::getMySubscriptionBySubscriptionUri( std::string &uri,
                    OC::Cm::Notification::SubscriptionRecord &subscription)
            {
                std::list<OC::Cm::Notification::SubscriptionRecord> subscriptions ;

                if (false == executeSQLSelectStatement("SELECT deviceId, subscribedResource, subscriptionUri, notificationUri, ROWID FROM MySubscriptions WHERE subscriptionUri='" + uri + "'", subscriptions))
                {
                    return false ;
                }

                if (subscriptions.size() == 0)
                {
                    return false ;
                }

                subscription = subscriptions.front() ;
                return true ;
            }

// SELECT Statement will be provided by the caller. caller must include column names in following order
// deviceId, subscribedResource, subscriptionUri, notificationUri, rowId
            bool
            SqliteSubscriptionDatabase::executeSQLSelectStatement( std::string sqlQueryStr, std::list<OC::Cm::Notification::SubscriptionRecord>& subscriptions )
            {
                sqlite3_stmt *sqlStatement = NULL ;

                if (sqlite3_prepare_v2(pDBHandle, sqlQueryStr.c_str(), -1, &sqlStatement, NULL) != SQLITE_OK)
                {
                    return false ;
                }

                while (SQLITE_ROW == sqlite3_step(sqlStatement))
                {
                    OC::Cm::Notification::SubscriptionRecord subscription ;
                    subscription.m_deviceId             = (char *)sqlite3_column_text(sqlStatement, 0) ;
                    subscription.m_subscribedResource = (char *)sqlite3_column_text(sqlStatement, 1) ;
                    subscription.m_subscriptionUri  = (char *)sqlite3_column_text(sqlStatement, 2) ;
                    subscription.m_noificationUri   = (char *)sqlite3_column_text(sqlStatement, 3) ;
                    subscription.m_rowId                =  sqlite3_column_int64(sqlStatement, 4) ;

                    subscriptions.push_back(subscription) ;
                }

                sqlite3_finalize (sqlStatement) ;

                return true ;
            }

            bool
            SqliteSubscriptionDatabase::executeSQLStatement( std::string sqlStatementStr )
            {
                sqlite3_stmt *sqlStatement = NULL ;

                if ( NULL == pDBHandle )
                {
                    return false ;
                }

                if (sqlite3_prepare_v2(pDBHandle, sqlStatementStr.c_str(), -1, &sqlStatement, NULL) != SQLITE_OK)
                {
                    return false ;
                }

                bool bRet = false ;
                if ( sqlite3_step (sqlStatement) == SQLITE_DONE )
                {
                    bRet = true ;
                }
                else
                {
                    bRet = false ;
                }

                sqlite3_finalize (sqlStatement) ;

                return bRet ;
            }



            std::list<std::string>
            SqliteSubscriptionDatabase::getListfromString(std::string resource)
            {
                std::list<std::string> retVal;

                while (resource.at(0) == ' ')
                    resource = resource.substr(1);

                int length = resource.length();

                while (resource.at(length - 1) == ' ')
                {
                    resource = resource.substr(0, length - 1);
                    length = resource.length();
                }

                std::string mToken = "";
                int index = resource.find(' ');

                while (index != (int)std::string::npos)
                {
                    mToken = resource.substr(0, index);
                    resource = resource.substr(index + 1);
                    index = resource.find(' ');
                    retVal.push_back(mToken);
                }

                if (resource.length() > 0)
                {
                    retVal.push_back(resource);
                }

                return retVal;

            }

        } /* namespace Notification */
    } /* namespace Cm */
} /* namespace OC */
