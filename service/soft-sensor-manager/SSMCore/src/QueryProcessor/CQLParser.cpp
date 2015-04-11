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
#include "CQLParser.h"

Token::Token()
{
    number = -1;
    name = "";
    type = Command;
    condition = ModelCondition::PREDICATE_EQ;
}

std::vector<std::string> CCQLParser::tokenize(const std::string &input)
{
    std::vector<std::string> temp;
    temp.push_back(",");
    temp.push_back(" ");

    std::vector<std::string> tokens_temp;
    std::string arrage_query = input;
    arrage_query = check_Predicate(arrage_query);
    for (unsigned int i = 0 ; i < temp.size(); i++)
    {

        tokens_temp.clear();
        tokens_temp = getTokens(arrage_query, temp.at(i));
        arrage_query = "";
        for (unsigned int j = 0  ; j < tokens_temp.size() ; j++)
        {
            arrage_query += tokens_temp.at(j) + " ";
            //cout << "element = " << tokens_temp.at(j) << endl;
        }
    }
    return tokens_temp;
}

bool CCQLParser::parse(std::string input, Token *root)
{
    std::vector<std::string> tokens = tokenize(input);
    bool flag;//get,sub,if
    bool value_flag = false;
    bool isCondition = false;

    for (unsigned int i = 0 ; i < tokens.size() ; i++)
    {
        if ( tolower(tokens.at(i)) == "get" || tolower(tokens.at(i)) == "subscribe"
             || tolower(tokens.at(i)) == "if")
        {
            Token temp;

            if (i != tokens.size() - 1 && tolower(tokens.at(i)) == "subscribe"
                && tolower(tokens.at(i + 1)) == "if")
            {
                temp.type = Command;
                temp.name = tokens.at(i);
                root->child_token.push_back(temp);
                temp.name = tokens.at(i + 1);

                //Token temp1;
                //temp1.type = Command;
                //temp1.name = tokens.at(i+1);




                i++;
                //temp1 = temp;
                flag = false;
                isCondition = true;
            }
            else
            {
                /*
                temp.type = Command;
                temp.name = tokens.at(i);
                root->child_token.push_back(temp);
                */
                temp.type = Command;
                temp.name = tokens.at(i);
                if (tokens.at(i) == "if")
                {
                    flag = false;
                    isCondition = true;
                }
                else
                {
                    flag = true;
                }
            }

            SORT lastType = Command;
            while (1)
            {
                //int count = 0;
                i++;

                if (value_flag == true)
                {
                    value_flag = false;
                    lastType = Context;
                    i++;
                }

                if ( i >= tokens.size() || tolower(tokens.at(i) ) == "if")
                {
                    i--;
                    break;
                }

                //enum condition {EQ,GTE,GT,LT,LTE,NEQ};
                Token temp1;
                temp1.name = tokens.at(i);


                if (tokens.at(i) == "=" || tokens.at(i) == "==")
                {
                    temp1.condition = ModelCondition::PREDICATE_EQ;
                    temp1.type = Condi;
                    value_flag = true;

                }
                else if (tokens.at(i) == ">=")
                {
                    temp1.condition = ModelCondition::PREDICATE_GTE;
                    temp1.type = Condi;
                    value_flag = true;
                }
                else if (tokens.at(i) == ">")
                {
                    temp1.condition =  ModelCondition::PREDICATE_GT;
                    temp1.type = Condi;
                    value_flag = true;
                }
                else if (tokens.at(i) == "<")
                {
                    temp1.condition = ModelCondition::PREDICATE_LT;
                    temp1.type = Condi;
                    value_flag = true;
                }
                else if (tokens.at(i) == "<=")
                {
                    temp1.condition = ModelCondition::PREDICATE_LTE;
                    temp1.type = Condi;
                    value_flag = true;
                }
                else if (tokens.at(i) == "!=")
                {
                    temp1.condition = ModelCondition::PREDICATE_NEQ;
                    temp1.type = Condi;
                    value_flag = true;
                }
                else
                {
                    temp1.type = Context;

                    //if current context token is reserved keyword, that return error
                    if (tolower(tokens.at(i)) == "get" || tolower(tokens.at(i)) == "subscribe")
                    {
                        return false;
                    }
                }

                if (flag == true)
                {

                    if (tolower(tokens.at(i)) == "and" || tolower(tokens.at(i)) == "or")
                    {
                        temp1.type = And_or;
                        flag = false;
                    }

                    if (split(tokens.at(i), &temp1, flag ) != true) //false -> Property
                    {
                        return false;
                    }
                }
                else
                {
                    //token count not matched, return and let grammer checker detects
                    if (tokens.size() < i + 3)
                    {
                        return false;
                    }

                    if (split(tokens.at(i), &temp1, flag , tokens.at(i + 1),
                              tokens.at(i + 2)) != true) //false -> Property
                    {
                        return false;
                    }
                    flag = true;
                }

                if (isCondition && lastType == temp1.type)
                {
                    return false;
                }

                lastType = temp1.type;

                temp.child_token.push_back(temp1);
            }
            root->child_token.push_back(temp);
        }
    }

    return true;
}

std::string CCQLParser::tolower(std::string str)
{
    for (unsigned int i = 0 ; i < str.size() ; i++)
    {
        if (str[i] <= 'Z' && str[i] >= 'A')
        {
            str[i] += 32;
        }
    }

    return str;
}

std::vector<std::string> CCQLParser::getTokens(const std::string &str,
        const std::string &delimiters)
{
    std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    std::string::size_type pos     = str.find_first_of(delimiters, lastPos);
    std::vector<std::string> tokens;

    while (std::string::npos != pos || std::string::npos != lastPos)
    {
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        lastPos = str.find_first_not_of(delimiters, pos);
        pos = str.find_first_of(delimiters, lastPos);
    }

    if (tokens.size() == 0)
        tokens.push_back("\"\"");

    return tokens;
}

void CCQLParser::check_index(std::string input, Token *token)
{
    std::vector<std::string> tokens = getTokens(input, "[");

    if (tokens.size() == 2)
    {
        if (tolower(tokens.at(1)) == "all]")
        {
            token->name = tokens.at(0);
            token->number = INDEX_ALL;

        }
        else if (tolower(tokens.at(1)) == "this]")
        {
            token->name = tokens.at(0);
            token->number = INDEX_THIS;

        }
        else
        {
            int num = atoi(tokens.at(1).c_str());
            token->name = tokens.at(0);
            token->number = num;
        }
    }
}

bool CCQLParser::split(std::string input, Token *root, bool flag, std::string arg1,
                       std::string arg2)
{
    std::vector<std::string> tokens = getTokens(input, ".");
    Token *temp_token = root;

    for (unsigned int j = 0 ; j < tokens.size() ; j++)
    {
        if (j == 0 )
        {
            temp_token->name = tokens.at(j);
            check_index(tokens.at(j), temp_token);
        }
        else
        {
            if (j == tokens.size() - 1 && flag == false)
            {
                ModelProperty property;
                property.propertyName = tokens.at(j);

                if (arg2[0] == '\"')
                {
                    std::vector<std::string> tokens_temp = getTokens(arg2, "\"");

                    property.propertyValue = tokens_temp.at(0);
                    property.propertyType = ModelProperty::TYPE_TEXT;

                    temp_token->model_property = property;

                    //root->child_token.push_back(temp1);
                }

                else
                {
                    //temp1->number = atof(arg2.c_str());

                    // 0 text /1 integer /2 real
                    if (check_number(arg2) == TYPEINTEGER)
                    {
                        property.propertyType =  ModelProperty::TYPE_INTEGER;
                    }
                    else if (check_number(arg2) == TYPEREAL)
                    {
                        property.propertyType = ModelProperty::TYPE_REAL;
                    }
                    else
                    {
                        //unknown type raise error
                        return false;
                    }
                    property.propertyValue = arg2;

                    temp_token->model_property = property;
                }
            }
            else
            {
                Token temp1;
                temp1.type = Context;
                temp1.name = tokens.at(j);
                check_index(tokens.at(j), &temp1);
                temp_token->child_token.push_back(temp1);
                temp_token = &(temp_token->child_token.back());
            }
        }
    }

    return true;
}

int CCQLParser::check_number(std::string &str)
{
    int flag = 0; // 0 text /1 integer /2 real
    int dotCount = 0;

    std::string::const_iterator it = str.begin();
    while (it != str.end())
    {
        if (*it == '.')
        {
            dotCount++;
        }
        else if (isdigit(*it) == false)
        {
            //this is text
            dotCount = 2;
            break;
        }

        ++it;
    }

    if (dotCount == 0)
    {
        flag = 1;
    }
    else if (dotCount == 1)
    {
        flag = 2;
    }

    return flag;
}

std::string CCQLParser::check_Predicate(std::string input)
{
    std::string temp = "";
    for (unsigned int i = 0 ; i < input.size() ; i++)
    {
        if (i == 0)
        {
            temp += input[0];
            continue;
        }

        switch (input[i])
        {
            case '=':
                if (input[i - 1] != '=' && input[i - 1] != '!' && input[i - 1] != '>' && input[i - 1] != '<'
                    && input[i - 1] != ' ')
                {
                    temp += ' ';
                }
                temp += input[i];
                break;
            case '!':
            case '<':
            case '>':
                if (input[i - 1] != ' ')
                {
                    temp += ' ';
                }
                temp += input[i];
                break;
            case ' ':
                temp += input[i];
                break;
            default:
                if (input[i - 1] == '=' || input[i - 1] == '<' || input[i - 1] == '>')
                {
                    temp += ' ';
                }
                temp += input[i];
                break;
        }
    }

    return temp;
}

bool CCQLParser::check_grammer(Token *token)
{
    if (token->child_token.size() == 1 && tolower(token->child_token.at(0).name) == "get")
    {
        if (token->child_token.at(0).child_token.size() > 0)
        {
            return true;
        }
        else
        {
            return false;
        }

    }
    else if (token->child_token.size() == 2)
    {
        if (token->child_token.at(1).child_token.size() > 0)
        {
            if (tolower(token->child_token.at(0).name) == "subscribe")
            {
                return true;
            }
            else if (tolower(token->child_token.at(0).name) == "get")
            {
                if (token->child_token.at(0).child_token.size() > 0)
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                return false;
            }

        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}
