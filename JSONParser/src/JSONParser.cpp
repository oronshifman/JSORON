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
    
    JSONObject& JSONParser::Parse(std::ifstream& json_file)
    {
	    u64 file_size = 0;
	    json_file.seekg(0, std::ios_base::end);
	    file_size = json_file.tellg();
	    json_file.seekg(0);

        std::string json_str(file_size, ' ');
        
        json_file.read(&json_str[0], file_size);
	    if (json_file.fail())
        {
            return *(new JSONObject());
        }

        return Parse(json_str); 
    }
    
    JSONObject& JSONParser::Parse(const std::string& json_str)
    {
        // Profiler_TimeFunction; // NOTE(27.10.24): PROFILING

        Lex(json_str);
        curr_tok = tokens.begin();
        return *(_Parse()->json_val);
    }

    JSONObject::JSONValue *JSONParser::_Parse()
    {
        // Profiler_TimeFunction; // NOTE(23.10.24): PROFILING

        if (curr_tok->type == TokenType::PUNCTUATION)
        {
            if (curr_tok->punc_tok == '{')
            {
                return ParseObj();
            }
            else if (curr_tok->punc_tok == '[')
            {
                return ParseArray();
            }
            else 
            {
                return new JSONValue();
            }
        }

        switch (curr_tok->type)
        {
            case TokenType::DOUBLE:
            {
                return new JSONValue(curr_tok->double_tok);
            } break;

            case TokenType::INT:
            {
                return new JSONValue(curr_tok->int_tok);
            } break;

            case TokenType::STR:
            {
                TokenList::iterator curr_tok_next = std::next(curr_tok);
                if (curr_tok_next->type == TokenType::PUNCTUATION &&
                    curr_tok_next->punc_tok == ':')
                {
                    return new JSONValue(JSONObject::ValueType::KEY, curr_tok->str_tok);
                }
                else
                {
                    return new JSONValue(curr_tok->str_tok);
                }
            } break;
        }

        return new JSONValue(JSONObject::ValueType::BAD_TYPE);
    }
    
    JSONObject::JSONValue *JSONParser::ParseObj()
    {
        Profiler_TimeFunction; // NOTE(23.10.24): PROFILING

        JSONValue *obj = new JSONValue(new JSONObject());
        
        ++curr_tok;

        while (!IsEndOfObj(*curr_tok))
        {
            JSONValue *key = _Parse();
            if (key->type == JSONObject::ValueType::KEY)
            {
                ++curr_tok;
                ++curr_tok;

                JSONValue *val = _Parse();
                if (val->type == JSONObject::ValueType::NULL_TYPE ||
                    val->type == JSONObject::ValueType::BAD_TYPE)
                {
                    std::cerr << "Invalid value for key " << key->str_val << "\n";
                    break;
                }

                obj->json_val->Put(key->str_val, val); 
            }

            ++curr_tok;
        }

        return obj;
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

    JSONObject::JSONValue *JSONParser::ParseArray()
    {
        Profiler_TimeFunction; // NOTE(23.10.24): PROFILING

        JSONValue *arr = new JSONValue(*(new JSONArray()));

        ++curr_tok;

        while (!IsEndOfArr(*curr_tok))
        {
            JSONValue *val = _Parse();
            if (val->type != JSONObject::ValueType::NULL_TYPE)
            {
                arr->json_arr.PushBack(val);
            }
        
            ++curr_tok;
        }

        return arr;
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

    void JSONParser::Lex(const std::string& json_str)
    {
        // Profiler_TimeFunction; // NOTE(25.09.24): PROFILING
        
        for (u32 at = 0; at < json_str.size();)
        {
            if (std::isdigit(json_str[at]) || json_str[at] == '-')
            {
                u8 num_digits = LexNumber(json_str, at);

                at += num_digits;

                continue;
            } 

            if (std::ispunct(json_str[at]))
            {
                LexPunctuation(json_str[at]);

                ++at;

                continue;
            }

            if (std::isspace(json_str[at]))
            {
                // Profiler_TimeBlock("Lexing spaces"); // NOTE(27.10.24): PROFILING

                while (std::isspace(json_str[at]))
                {
                    ++at;
                }

                continue;
            }
    
            if (std::isalpha(json_str[at]))
            {
                at += LexString(json_str, at);

                continue;
            }
        }
    }
    
    void JSONParser::LexPunctuation(const char punc)
    {
        // Profiler_TimeFunction; // NOTE(27.10.24): PROFILING

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
    
    u32 JSONParser::LexString(const std::string& json_str, u32 at)
    {
        // Profiler_TimeFunction; // NOTE(27.10.24): PROFILING

        std::string str;

        u32 string_start = at;
        while (json_str[at] != '"')
        {
            ++at;
        }

        u32 count = at - string_start; 
        str.insert(0, json_str, string_start, count);
        tokens.push_back(Token(str));

        return count;
    }
    
    u8 JSONParser::LexNumber(const std::string& json_str, u32 at)
    {
        // Profiler_TimeFunction; // NOTE(27.10.24): PROFILING

        std::string num;
        b8 is_float = 0;

        u64 index = at;
        s8 sign = 1;
        if (json_str[at] == '-')
        {
            sign = -1;
            index = at + 1;
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

        return sign < 0 ? num.size() + 1 : num.size();
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



















