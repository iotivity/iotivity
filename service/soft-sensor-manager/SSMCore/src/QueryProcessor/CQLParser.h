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
#ifndef _CQLParser_H_
#define _CQLParser_H_

#include "SSMInterface/SSMCore.h"
#include "Common/PlatformLayer.h"
#include "EvaluationEngine.h"

#define TYPETEXT 0
#define TYPEINTEGER 1
#define TYPEREAL 2
#define INDEX_ALL 9999
#define INDEX_THIS -2

enum SORT {Command, Context, Property, And_or, Condi, Value};

class Token
{
    public:
        Token();

        enum SORT type;
        enum ModelCondition::Predicate condition;
        std::string name;
        int number;
        std::vector<Token> child_token;
        ModelProperty model_property;
};

/**
* @class    CCQLParser
* @brief    CQLParser Interface
*            This class parses ContextQuery Language.
*
* @see
*           string contextQuery;
*           CQLParser tokenizer;
*           tokenizer.Parser(contextQuery, &token);
*
* Copyright 2013 by Samsung Electronics, Inc.,
*
* This software is the confidential and proprietary information
* of Samsung Electronics, Inc. ("Confidential Information").  You
* shall not disclose such Confidential Information and shall use
* it only in accordance with the terms of the license agreement
* you entered into with Samsung.
*/
class CCQLParser
{

    public:
        /**
        * @fn parse
        * @brief parse ContextQuery
        *
        * @param [in] std::string input - Entered ContetxQuery
        * @param [out] Token* root - parsed result
        * @return bool
        * @warning
        * @exception
        * @see
        */
        bool parse(std::string input, Token *root);

        /**
        * @fn tolower
        * @brief convert lowercase
        *
        * @param [in] std::string str - Entered string
        * @return std::string
        * @warning
        * @exception
        * @see
        */
        static std::string tolower(std::string str);

        /**
        * @fn check_Query_grammer
        * @brief check grammer
        *
        * @param [in] Token* token - parsed token
        * @return boolean
        * @warning
        * @exception
        * @see
        */
        bool check_grammer(Token *token);

    private:
        /**
        * @fn tokenize
        * @brief split sentence
        *
        * @param [in] const std::string& input - Entered ContetxQuery
        * @return std::vector<std::string>
        * @warning
        * @exception
        * @see
        */
        std::vector<std::string> tokenize(const std::string &input);

        /**
        * @fn getTokens
        * @brief this function divides string into tokens based on a delimiter
        *
        * @param [in] const std::string& str - entered string
        * @param [in] const std::string& delimiters = " " - token delimiter , default = " "
        * @return std::vector<std::string>
        * @warning
        * @exception
        * @see
        */
        static std::vector<std::string> getTokens(const std::string &str,
                const std::string &delimiters = " ");

        /**
        * @fn check_index
        * @brief this function checks the index.
        *
        * @param [in] const std::string input - entered string
        * @param [out] Token* token - if index exists, The index has been entered into the token.
        * @return none
        * @warning
        * @exception
        * @see
        */
        void check_index(std::string input, Token *token);

        /**
        * @fn split
        * @brief this function splits the token to the smaller elements.
        *
        * @param [in] std::string input - entered string
        * @param [in] Token* root - root token
        * @param [in] bool flag - flag to distinguish token
        * @param [in] std::string arg1 - next token, default = ""
        * @param [in] std::string arg2 - next two token, default = ""
        * @return bool
        * @warning
        * @exception
        * @see
        */
        bool split(std::string input, Token *root, bool flag, std::string arg1 = "",
                   std::string arg2 = "");

        /**
        * @fn check_number
        * @brief this function distinguishes string type
        *
        * @param [in] std::string & str - entered string
        * @return int
        * @warning
        * @exception
        * @see
        */
        int check_number(std::string &str);

        /**
        * @fn check_Predicate
        * @brief this function checks predicate
        *
        * @param [in] std::string input - entered string
        * @return std::string
        * @warning
        * @exception
        * @see
        */
        std::string check_Predicate(std::string input);
};

#endif /*_CQLParser_H_*/
