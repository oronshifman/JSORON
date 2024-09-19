/* ------------------------------------------*/ 
/* Filename: JSONParser.h                    */
/* Date:     24.07.2024                      */
/* Author:   Oron                            */ 
/* ------------------------------------------*/

#ifndef __JSONPARSER_H__
#define __JSONPARSER_H__

#include <fstream>
#include <list>

#include "JSONObject.h"
#include "my_int.h"

namespace JSORON
{
    JSONObject bad_obj = JSONObject();

#ifdef PROFILING
    struct parser_profiling_data
    {
        u64 lexing;
        u64 parsing;
        u64 erasing;
        u64 create_keys;
    }; 
#endif /* PROFILING */

    class JSONParser 
    {
#ifndef NDEBUG
    public:
#endif /* NDEBUG */    
        enum class TokenType
        {
            NULL_TYPE,
    
            STR,
            PUNCTUATION,
            INT,
            DOUBLE,
    
            NUM_TOKEN_TYPES
        };
    
        class Token
        {
        public:
            TokenType type;
    
            union
            {
                std::string str_tok;
                char punc_tok;
                s32 int_tok;
                f32 double_tok;
            };
    
            Token() : type(TokenType::NULL_TYPE) {}
            Token(const Token& other);
            Token& operator=(const Token& other);

            Token(const std::string str_tok) : type(TokenType::STR), str_tok(str_tok) {}
            Token(const char punc_tok) : type(TokenType::PUNCTUATION), punc_tok(punc_tok) {}
            Token(const s32 int_tok) : type(TokenType::INT), int_tok(int_tok) {}
            Token(const f32 double_tok) : type(TokenType::DOUBLE), double_tok(double_tok) {}
    
            ~Token();

            friend bool operator==(const JSONParser& lhs, const JSONParser& rhs);
            friend bool operator!=(const JSONParser& lhs, const JSONParser& rhs);
            friend bool operator==(const Token& lhs, const Token& rhs);
            friend bool operator!=(const Token& lhs, const Token& rhs);
            friend std::ostream& operator<<(std::ostream& out, const Token& tok);

        private:
            void AssignTokByType(Token& dest, const Token& src, TokenType type);
        };
    
    public:
        typedef std::list<Token> TokenList;

#ifdef PROFILING          
        void InitProfilingData(parser_profiling_data* pd);
#endif /* PROFILING */
           
        JSONObject Parse(const std::string& json_str);
        JSONObject Parse(std::ifstream& json_file); 
#ifdef PROFILING          
        JSONObject& ProfiledParse(std::ifstream& json_file);
        JSONObject& ProfiledParse(const std::string& json_str); 
#endif /* PROFILING */
        
        friend bool operator==(const JSONParser& lhs, const JSONParser& rhs);
        friend bool operator!=(const JSONParser& lhs, const JSONParser& rhs);
        friend std::ostream& operator<<(std::ostream& out, const Token& tok);
        friend std::ostream& operator<<(std::ostream& out, const TokenList& toks);
#ifdef NDEBUG
    private:
#endif /* NDEBUG */
    
        void Lex(std::string json_str);

        void LexPunctuation(const char punc);
        void LexString(const std::string& json_str);
        u8 LexNumber(const std::string& json_str);

        b8 IsEndOfObj(const Token& tok);
        b8 IsEndOfArr(const Token& tok);

// #ifdef PROFILING
        JSONObject::JSONValue& _Parse();
        JSONObject::JSONValue& ParseObj();
        JSONObject::JSONValue& ParseArray();
// #else
#ifdef TEMP
        JSONObject::JSONValue _Parse();
        JSONObject::JSONValue ParseObj();
        JSONObject::JSONValue ParseArray();
#endif /* PROFILING */

        TokenList tokens;

#ifdef PROFILING          
        parser_profiling_data* pd;
#endif /* PROFILING */
    };
}

#endif /* JSONPARSER_H */

