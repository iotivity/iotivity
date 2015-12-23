//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
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
package oic.ctt.ui.util;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;

import org.slf4j.Logger;

import oic.ctt.ui.UIConst;

/**
 * DatabaseUtil provides common database functionality like creation connection
 * table, modification table,retrieving database database information
 */
public class DatabaseUtil {
    private static Logger       logger                = CTLogger.getInstance();
    public static final String  EXTENTION_DB          = ".db";
    public static final String  JDBC_SQLITE           = "jdbc:sqlite:";
    private static final String JDBC_DRIVER_NAME      = "org.sqlite.JDBC";
    private static final String COMMON_CONNECTION_URL = JDBC_SQLITE
                                                              + UIConst.ROOT_PATH
                                                              + UIConst.TESTPLAN_PATH;
    public static final String  TABLE_NAME            = "tcinfo";
    public static final String  DEFAULT_INSERT_VALUES = "?, ?, ?, ?, ?";

    /**
     * This method closes opened data base objects
     * 
     * @param databaseObjects
     *            this is a set of databaseObjects have to be closed
     */
    public static void closeDataBaseObjects(AutoCloseable[] databaseObjects) {

        for (AutoCloseable dataBaseObject : databaseObjects) {
            try {
                if (dataBaseObject != null)
                    dataBaseObject.close();
            } catch (Exception e) {
            }
        }
    }

    /**
     * Attempts to establish a connection to the given database URL.
     * 
     * @param url
     *            a database url of the form
     * @return a connection to the URL
     * @exception SQLException
     *                if a database access error occurs or the url is null
     */
    public static Connection createDBConnection(String url) {
        logger.info(COMMON_CONNECTION_URL + "/" + url + "/" + url
                + EXTENTION_DB);
        Connection conn = null;
        if (url.isEmpty() || url == null) {
            logger.info("Value of url is : " + url);
            throw new NullPointerException(
                    "Could not create database connection with null or empty url ");
        } else {
            registerJDBCDriver();
            try {
                conn = DriverManager.getConnection(COMMON_CONNECTION_URL + "/"
                        + url + "/" + url + EXTENTION_DB);
            } catch (SQLException sqlException) {
                logger.error(sqlException.getLocalizedMessage());
            }
        }
        return conn;
    }

    /**
     * Java ClassLoader has initialized (and loaded) the Driver class
     * 
     * @param nothing
     * @exception ClassNotFoundException
     *                if the class cannot be located
     */
    public static void registerJDBCDriver() {
        try {
            Class.forName(JDBC_DRIVER_NAME);
        } catch (ClassNotFoundException ex) {
            logger.error(ex.getLocalizedMessage());
        }
    }

    /**
     * Used to create a new database table
     * 
     * @param tableName
     *            this is table to be created
     * @param columns
     *            list of table columns with name and data type
     * @param conn
     *            database connection object that created previously
     * @return nothing
     * @throws SQLException
     */
    public static void createDBTable(String tableName, String columns,
            Connection conn) {
        Statement statement = null;
        deleteDBTable(tableName, conn);
        String createTableQuery = String.format("CREATE TABLE %s (%s);",
                tableName, columns);
        try {
            statement = conn.createStatement();
            statement.executeUpdate(createTableQuery);
            logger.info(tableName + " created successfully");
        } catch (Exception ex) {
            logger.error(ex.getLocalizedMessage());
        } finally {
            AutoCloseable[] closeableObjects = { statement };
            closeDataBaseObjects(closeableObjects);
        }
    }

    /**
     * Used to delete a existing table
     * 
     * @param tableName
     *            table to be deleted
     * @param conn
     *            database connection object that created previously
     * @return nothing
     * @throws SQLException
     */
    public static void deleteDBTable(String tableName, Connection conn) {
        String deleteTableQuery = String.format("drop table if exists %s",
                tableName);
        Statement statement = null;
        try {
            statement = conn.createStatement();
            statement.executeUpdate(deleteTableQuery);
        } catch (SQLException sqlException) {
            logger.error(sqlException.getLocalizedMessage());
        } finally {
            AutoCloseable[] closeableObjects = { statement };
            closeDataBaseObjects(closeableObjects);
        }
    }

    /**
     * Used to update a existing table
     * 
     * @param conn
     *            database connection object that created previously
     * @param updateQuery
     *            database query
     * @return prepUpdate a PreparedStatement object
     * @throws SQLException
     */
    public static PreparedStatement updateTable(Connection conn,
            String updateQuery) throws SQLException {
        PreparedStatement prepUpdate = null;
        prepUpdate = conn.prepareStatement(updateQuery);
        return prepUpdate;
    }

    /**
     * Used to insert into a existing table
     * 
     * @param conn
     *            database connection object that created previously
     * @param tableName
     *            the table values will be inserted into
     * @param values
     *            values to be inserted
     * @return prepUpdate a PreparedStatement object
     * @throws SQLException
     */
    public static PreparedStatement insertIntoTable(Connection conn,
            String tableName, String values) throws SQLException {
        PreparedStatement prepInsert = null;
        String insertQuery = String.format("INSERT INTO %s VALUES (%s);",
                tableName, values);
        prepInsert = conn.prepareStatement(insertQuery);
        return prepInsert;
    }

    /**
     * Used to Retrieve database information based on query
     * 
     * @param conn
     *            database connection object that created previously
     * @param query
     *            relevant query to get information
     * @return prepUpdate a PreparedStatement object having table rows
     * @throws SQLException
     */
    public static ResultSet retrieveQueryResult(Connection conn, String query) {

        ResultSet resultSet = null;
        Statement statement = null;
        try {
            statement = conn.createStatement();
            resultSet = statement.executeQuery(query);
        } catch (SQLException sqlException) {
            logger.error(sqlException.getLocalizedMessage());
        }
        return resultSet;

    }
}
