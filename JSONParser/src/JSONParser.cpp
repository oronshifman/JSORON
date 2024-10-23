/* ------------------------------------------*/ 
/* Filename: JSONParser.cpp                  */
/* Date:     24.07.2024                      */
/* Author:   Oron                            */ 
/* ------------------------------------------*/

#include <fstream>
#include <string>
#include <list>
#include <iostream>

#include "JSONParser.h"
#include "JSONObject.h"
#include "my_int.h"
#include "profiler.h"

namespace JSORON
{
    typedef std::list<JSONParser::Token> TokenList;
    
    JSONParser::Token::Token(const Token& other) : type(other.type)
    {
        AssignTokByType(*this, other, other.type);
    }

    JSONParser::Token& JSONParser::Token::operator=(const JSONParser::Token& other)
    {
        if (this == &other)
        {
            return *this;
        }

        type = other.type;
        AssignTokByType(*this, other, other.type);
        return *this;
    }        

    void JSONParser::Token::AssignTokByType(Token& dest, const Token& src, TokenType type)
    {
        switch (type)
        {
            case JSONParser::TokenType::STR:
            {
                new (&dest.str_tok) std::string(src.str_tok);
            } break;

            case JSONParser::TokenType::PUNCTUATION:
            {
                dest.punc_tok = src.punc_tok;
            } break;

            case JSONParser::TokenType::INT:
            {
                dest.int_tok = src.int_tok;
            } break;

            case JSONParser::TokenType::DOUBLE:
            {
                dest.double_tok = src.double_tok;
            } break;

            case JSONParser::TokenType::NULL_TYPE:
            {
            } break;
        }
    }

    JSONParser::Token::~Token()
    {
        switch(type)
        {
            case JSONParser::TokenType::NULL_TYPE:
            case JSONParser::TokenType::INT:
            case JSONParser::TokenType::DOUBLE:
            case JSONParser::TokenType::PUNCTUATION:
            {
            } break;
    
            case JSONParser::TokenType::STR:
            {
                str_tok.~basic_string();
            } break;
        }
    }
    
    const JSONObject JSONParser::bad_obj = JSONObject();
    
    JSONObject JSONParser::Parse(std::ifstream& json_file)
    {
	    u64 file_size = 0;
	    json_file.seekg(0, std::ios_base::end);
	    file_size = json_file.tellg();
	    json_file.seekg(0);

        std::string json_str(file_size, ' ');
        
        json_file.read(&json_str[0], file_size);
	    if (json_file.fail())
        {
            return bad_obj;
        }

        return Parse(json_str); 
    }
    
    JSONObject JSONParser::Parse(const std::string& json_str)
    {
        Profiler_TimeFunction; // NOTE(25.09.24): PROFILING

        Lex(json_str);
        return *(_Parse().json_val);
    }

    JSONObject::JSONValue JSONParser::_Parse()
    {
        Profiler_TimeFunction; // NOTE(23.10.24): PROFILING

        Token curr_tok = tokens.front();
        tokens.erase(tokens.begin());

        if (curr_tok.type == TokenType::PUNCTUATION)
        {
            if (curr_tok.punc_tok == '{')
            {
                return ParseObj();
            }
            else if (curr_tok.punc_tok == '[')
            {
                return ParseArray();
            }
            else 
            {
                return JSONValue();
            }
        }

        switch (curr_tok.type)
        {
            case TokenType::DOUBLE:
            {
                return JSONValue(curr_tok.double_tok);
            } break;

            case TokenType::INT:
            {
                return JSONValue(curr_tok.int_tok);
            } break;

            case TokenType::STR:
            {
                if (tokens.front().type == TokenType::PUNCTUATION &&
                    tokens.front().punc_tok == ':')
                {
                    tokens.erase(tokens.begin());
                    return JSONValue(JSONObject::ValueType::KEY, curr_tok.str_tok);
                }
                else
                {
                    return JSONValue(curr_tok.str_tok);
                }
            } break;
        }

        return JSONObject::bad_value;
    }
    
    JSONObject::JSONValue JSONParser::ParseObj()
    {
        Profiler_TimeFunction; // NOTE(23.10.24): PROFILING

        JSONObject obj;
        
        Token curr_tok = tokens.front();
        while (!IsEndOfObj(curr_tok))
        {
            JSONValue key = _Parse();
            if (key.type == JSONObject::ValueType::KEY)
            {
                JSONValue val = _Parse();
                if (val.type == JSONObject::ValueType::NULL_TYPE ||
                    val.type == JSONObject::ValueType::BAD_TYPE)
                {
                    std::cerr << "Invalid value for key " << key.str_val << "\n";
                    break;
                }
                obj.Put(key.str_val, val); 
            }
            curr_tok = tokens.front();
        }

        return JSONValue(obj);
    }

    b8 JSONParser::IsEndOfObj(const Token& tok)
    {
        if (tok.type == TokenType::PUNCTUATION &&
            tok.punc_tok == '}')
        {
            return 1;
        }

        return 0;
    }

    JSONObject::JSONValue JSONParser::ParseArray()
    {
        Profiler_TimeFunction; // NOTE(23.10.24): PROFILING

        JSONArray arr;

        Token curr_tok = tokens.front();
        while (!IsEndOfArr(curr_tok))
        {
            JSONValue val = _Parse();
            // NOTE(20.08.24): val might be NULL_TYPE. figure out what are the cases this could happen
            if (val.type != JSONObject::ValueType::NULL_TYPE)
            {
                arr.PushBack(val);
            }
        
            curr_tok = tokens.front();
        }

        return JSONValue(arr);
    }
    
    b8 JSONParser::IsEndOfArr(const Token& tok)
    {
        if (tok.type == TokenType::PUNCTUATION &&
            tok.punc_tok == ']')  
        {
            return 1;
        }

        return 0;
    }

    void JSONParser::Lex(std::string json_str)
    {
        while (!json_str.empty())
        {
            if (std::isdigit(json_str[0]) || json_str[0] == '-')
            {
                u8 num_digits = LexNumber(json_str);

                if (json_str[0] == '-')
                {
                    json_str.erase(json_str.begin());
                }

                json_str.erase(0, num_digits);

                continue;
            } 

            if (std::ispunct(json_str[0]))
            {
                LexPunctuation(json_str[0]);
                json_str.erase(json_str.begin());
                continue;
            }

            if (std::isspace(json_str[0]))
            {
                while (std::isspace(json_str[0]))
                {
                    json_str.erase(json_str.begin());
                }

                continue;
            }
    
            if (std::isalpha(json_str[0]))
            {
                LexString(json_str);
                json_str.erase(0, tokens.back().str_tok.size());
                continue;
            }
        }
    }
    
    void JSONParser::LexPunctuation(const char punc)
    {
        switch (punc)
        {
            case '{':
            {
                tokens.push_back(Token('{'));
            } break;
    
            case '}':
            {
                tokens.push_back(Token('}'));
            } break;
    
            case '[':
            {
                tokens.push_back(Token('['));
            } break;
    
            case ']':
            {
                tokens.push_back(Token(']'));
            } break;
    
            case ':':
            {
                tokens.push_back(Token(':'));
            } break;
            
            case ',':
            {
                tokens.push_back(Token(','));
            } break;
            
            case '"':
            {
            } break;
        }
    }
    
    void JSONParser::LexString(const std::string& json_str)
    {
        std::string str;

        auto iter = json_str.begin();
        while (*iter != '"')
        {
            ++iter;
        }
            
        str.insert(str.begin(), json_str.begin(), iter);
        tokens.push_back(Token(str));
    }
    
    u8 JSONParser::LexNumber(const std::string& json_str)
    {
        std::string num;
        b8 is_float = 0;

        u64 index = 0;
        s8 sign = 1;
        if (json_str[0] == '-')
        {
            sign = -1;
            index = 1;
        }

        while (1)
        {
            num += json_str[index];
            
            ++index;
            if (json_str[index] == '.')
            {
                if (!is_float)
                {
                    is_float = 1;
                    continue;
                }
                else
                {
                    // TODO(20.08.24): error
                    std::cerr << "Error while parsing number. unexpected '.'\n";
                    break;
                }
            }

            if (!std::isdigit(json_str[index]))
            {
                break;
            }
        }

        if (is_float)
        {
            f64 new_float = std::stod(num);
            new_float *= sign;
            tokens.push_back(Token(new_float));
        }
        else 
        {
            s32 new_int = std::stoi(num);
            new_int *= sign;
            tokens.push_back(Token(new_int));
        }

        return num.size();
    }

    bool operator==(const JSONParser& lhs, const JSONParser& rhs)
    {
        if (&lhs == &rhs)
        {
            return 1;
        }

        for (auto lhs_iter = lhs.tokens.begin(), rhs_iter = rhs.tokens.begin();
             lhs_iter != lhs.tokens.end() && rhs_iter != rhs.tokens.end();
             ++lhs_iter, ++rhs_iter)
        {
            if (*lhs_iter != *rhs_iter)
            {
                return 0;
            }
        }

        return 1;
    }
    
    bool operator!=(const JSONParser& lhs, const JSONParser& rhs)
    {
        return !(lhs == rhs);
    }

    bool operator==(const JSONParser::Token& lhs, const JSONParser::Token& rhs)
    {
        if (&lhs == &rhs)
        {
            return 1;
        }

        if (lhs.type != rhs.type)
        {
            return 0;
        }

        switch (lhs.type)
        {
            case JSONParser::TokenType::NULL_TYPE:
            {
                return 1;
            } break;

            case JSONParser::TokenType::DOUBLE:
            {
                return lhs.double_tok == rhs.double_tok;
            } break;

            case JSONParser::TokenType::INT:
            {
                return lhs.int_tok == rhs.int_tok;
            } break;

            case JSONParser::TokenType::STR:
            {
                return lhs.str_tok == rhs.str_tok;
            } break;
            
            case JSONParser::TokenType::PUNCTUATION:
            {
                return lhs.punc_tok == rhs.punc_tok;
            } break;

            default:
            {
                // TODO(17.8.24): output an error
                return 0;
            } break;
        }
    }

    bool operator!=(const JSONParser::Token& lhs, const JSONParser::Token& rhs)
    {
        return !(lhs == rhs);
    }
    
    std::ostream& operator<<(std::ostream& out, const JSONParser::Token& tok)
    {
        switch (tok.type)
        {
            case JSONParser::TokenType::NULL_TYPE:
            {
                out << "type: NULL_TYPE";
            } break;

            case JSONParser::TokenType::INT:
            {
                out << "type: INT, val: " << tok.int_tok;
            } break;

            case JSONParser::TokenType::DOUBLE:
            {
                out << "type: DOUBLE, val: " << tok.double_tok;
            } break;

            case JSONParser::TokenType::STR:
            {
                out << "type: STR, val: " << tok.str_tok;
            } break;

            case JSONParser::TokenType::PUNCTUATION:
            {
                out << "type: PUNCTUATION, val: " << tok.punc_tok;
            } break;

            default:
            {
                out << "Unknown Token type";
            } break;
        }

        return out;
    }
        
   std::ostream& operator<<(std::ostream& out, const TokenList& toks)
   {
       out << "[";
       for (auto& tok : toks)
       {
           out << tok << (tok == toks.back() ? "" : ",");
       }
       out << "]";

       return out;
   }

} // namespace JSORON



















