/* ------------------------------------------*/ 
/* Filename: test_JSONParser.cpp             */
/* Date:     24.07.2024                      */
/* Author:   Oron                            */ 
/* ------------------------------------------*/

#include <vector>
#include <list>
#include <iostream>

#include "JSONParser.h"
#include "JSONObject.h"
#include "generic_test.h"
#include "profiler.h"

using namespace JSORON;

JSONObject simple_json1;
JSONObject simple_json2;
JSONObject simple_json3;
JSONObject real_use_json;

void InitSimpleJson1();
void InitSimpleJson2();
void InitSimpleJson3();
void InitRealUseJson1();

void TestLexer1(Tester& tester);
void TestLexer2(Tester& tester);
void TestLexer3(Tester& tester);

void TestParser1(Tester& tester);
void TestParser2(Tester& tester);
void TestParser3(Tester& tester);

void TestRealJson_Lex(Tester& tester);
void TestRealJson_Parse(Tester& tester);

void TestParseFromFile(Tester& tester);

void PrintTokenList(JSONParser::TokenList token_list);

parser_profiling_data pd = {0};

int main(int argc, char *argv[])
{
    Tester tester;
    
    // TestLexer1(tester);
    // TestLexer2(tester);
    // TestLexer3(tester);

    // TestParser1(tester);
    // TestParser2(tester);
    // TestParser3(tester);
    
    // TestRealJson_Lex(tester);
    // TestRealJson_Parse(tester);

    TestParseFromFile(tester);

    tester.TestAll();

    std::cout << "\n"
              << "Lexing: " << pd.lexing << "\n"
              << "Parsing: " << pd.parsing - pd.lexing << "\n";

	return 0;
}

void TestParseFromFile(Tester& tester)
{
    std::ifstream json_file;
    // json_file.open("/home/oron/git/perfaware/part2/JSORON/JSONParser/test/uniform_234889_4_points.json");
    // if (json_file.good())
    // {
    //     JSONParser parser;
    //     JSONObject obj = parser.Parse(json_file);
    //     tester.AssertEqual(obj, real_use_json, "TestParseFromFile", __LINE__);
    //     json_file.close();
    // }
    json_file.open("/home/oron/git/perfaware/part2/JSORON/JSONParser/test/uniform_4320980_10000_points.json");
    if (json_file.good())
    {
        JSONParser parser;
        // JSONObject obj = parser.Parse(json_file); // NOTE(15.09.24): NOT Profiled

        parser.InitProfilingData(&pd);
        JSONObject obj = parser.ProfiledParse(json_file); // NOTE(15.09.24): Profiled
        tester.AssertEqual("stress", "stress", "TestParseFromFile", __LINE__);
        json_file.close();
    }
}

void TestRealJson_Lex(Tester& tester)
{
    JSONParser parser;
    parser.Lex("{\"pairs\":[{\"x0\":-24.136337,\"y0\":75.754684,\"x1\":-127.218956,\"y1\":-25.416527}, {\"x0\":25.535736,\"y0\":-43.788517,\"x1\":-67.682999,\"y1\":82.133118}, {\"x0\":-108.825356,\"y0\":-80.391953,\"x1\":93.193268,\"y1\":-5.138481}, {\"x0\":150.926361,\"y0\":63.822083,\"x1\":-58.930611,\"y1\":72.343033}]}");
    std::list<JSONParser::Token> expected{JSONParser::Token('{'), 
                                          JSONParser::Token("pairs"), 
                                          JSONParser::Token(':'),
                                          JSONParser::Token('['),
                                          JSONParser::Token('{'),
                                          JSONParser::Token("x0"),
                                          JSONParser::Token(':'),
                                          JSONParser::Token(-24.136337f),
                                          JSONParser::Token(','),
                                          JSONParser::Token("y0"),
                                          JSONParser::Token(':'),
                                          JSONParser::Token(75.754684f),
                                          JSONParser::Token(','),
                                          JSONParser::Token("x1"),
                                          JSONParser::Token(':'),
                                          JSONParser::Token(-127.218956f),
                                          JSONParser::Token(','),
                                          JSONParser::Token("y1"),
                                          JSONParser::Token(':'),
                                          JSONParser::Token(-25.416527f),
                                          JSONParser::Token('}'),
                                          JSONParser::Token(','),

                                          JSONParser::Token('{'),
                                          JSONParser::Token("x0"),
                                          JSONParser::Token(':'),
                                          JSONParser::Token(25.535736f),
                                          JSONParser::Token(','),
                                          JSONParser::Token("y0"),
                                          JSONParser::Token(':'),
                                          JSONParser::Token(-43.788517f),
                                          JSONParser::Token(','),
                                          JSONParser::Token("x1"),
                                          JSONParser::Token(':'),
                                          JSONParser::Token(-67.682999f),
                                          JSONParser::Token(','),
                                          JSONParser::Token("y1"),
                                          JSONParser::Token(':'),
                                          JSONParser::Token(82.133118f),
                                          JSONParser::Token('}'),
                                          JSONParser::Token(','),
                                          
                                          JSONParser::Token('{'),
                                          JSONParser::Token("x0"),
                                          JSONParser::Token(':'),
                                          JSONParser::Token(-108.825356f),
                                          JSONParser::Token(','),
                                          JSONParser::Token("y0"),
                                          JSONParser::Token(':'),
                                          JSONParser::Token(-80.391953f),
                                          JSONParser::Token(','),
                                          JSONParser::Token("x1"),
                                          JSONParser::Token(':'),
                                          JSONParser::Token(93.193268f),
                                          JSONParser::Token(','),
                                          JSONParser::Token("y1"),
                                          JSONParser::Token(':'),
                                          JSONParser::Token(-5.138481f),
                                          JSONParser::Token('}'),
                                          JSONParser::Token(','),
                                          
                                          JSONParser::Token('{'),
                                          JSONParser::Token("x0"),
                                          JSONParser::Token(':'),
                                          JSONParser::Token(150.926361f),
                                          JSONParser::Token(','),
                                          JSONParser::Token("y0"),
                                          JSONParser::Token(':'),
                                          JSONParser::Token(63.822083f),
                                          JSONParser::Token(','),
                                          JSONParser::Token("x1"),
                                          JSONParser::Token(':'),
                                          JSONParser::Token(-58.930611f),
                                          JSONParser::Token(','),
                                          JSONParser::Token("y1"),
                                          JSONParser::Token(':'),
                                          JSONParser::Token(72.343033f),
                                          JSONParser::Token('}'),

                                          JSONParser::Token(']'),
                                          JSONParser::Token('}')};

    tester.AssertEqual(parser.tokens, expected, "TestRealJson_Lex", __LINE__);
}

void TestRealJson_Parse(Tester& tester)
{
    InitRealUseJson1();

    JSONParser parser;
    JSONObject obj = parser.Parse("{\"pairs\":[{\"x0\":-24.136337,\"y0\":75.754684,\"x1\":-127.218956,\"y1\":-25.416527}, {\"x0\":25.535736,\"y0\":-43.788517,\"x1\":-67.682999,\"y1\":82.133118}, {\"x0\":-108.825356,\"y0\":-80.391953,\"x1\":93.193268,\"y1\":-5.138481}, {\"x0\":150.926361,\"y0\":63.822083,\"x1\":-58.930611,\"y1\":72.343033}]}");

    tester.AssertEqual(obj, real_use_json, "TestRealJson_Parse", __LINE__);
} 

void TestParser1(Tester& tester)
{
    InitSimpleJson1();

    JSONParser parser;
    JSONObject obj = parser.Parse("{ \"intKey\": 2 }");

    tester.AssertEqual(obj, simple_json1, "TesterParser1", __LINE__);
}

void TestParser2(Tester& tester)
{
    InitSimpleJson2();
    
    JSONParser parser;
    JSONObject obj = parser.Parse("{ \"nestedJson\": {\"intKey\": 2}, \"intKey\": 42 }");

    tester.AssertEqual(obj, simple_json2, "TesterParser2", __LINE__);
}

void TestParser3(Tester& tester)
{
    InitSimpleJson3();

    JSONParser parser;
    JSONObject obj = parser.Parse("{ \"jsonArray\": [{\"json1\": 1},{\"json2\": 2}]}");

    tester.AssertEqual(obj, simple_json3, "TesterParser3", __LINE__);
}

void TestLexer1(Tester& tester)
{
    JSONParser parser;
    parser.Lex("{ \"intKey\": 2 }");
    std::list<JSONParser::Token> expected{JSONParser::Token('{'), 
                                          JSONParser::Token("intKey"), 
                                          JSONParser::Token(':'),
                                          JSONParser::Token(2),
                                          JSONParser::Token('}')};

    tester.AssertEqual(parser.tokens, expected, "TestLexer1", __LINE__);
}


void TestLexer2(Tester& tester)
{
    JSONParser parser;
    parser.Lex("{ \"nestedJson\": {\"nestedInt\": 2}, \"intKey\": 42 }");
    std::list<JSONParser::Token> expected{JSONParser::Token('{'), 
                                          JSONParser::Token("nestedJson"), 
                                          JSONParser::Token(':'),
                                          JSONParser::Token('{'),
                                          JSONParser::Token("nestedInt"),
                                          JSONParser::Token(':'),
                                          JSONParser::Token(2),
                                          JSONParser::Token('}'),
                                          JSONParser::Token(','),
                                          JSONParser::Token("intKey"), 
                                          JSONParser::Token(':'),
                                          JSONParser::Token(42),
                                          JSONParser::Token('}')};

    tester.AssertEqual(parser.tokens, expected, "TestLexer2", __LINE__);
}

void TestLexer3(Tester& tester)
{
    JSONParser parser;
    parser.Lex("{ \"jsonArray\": [{\"json1\": 1},{\"json2\": 2}]}");
    std::list<JSONParser::Token> expected{JSONParser::Token('{'), 
                                          JSONParser::Token("jsonArray"), 
                                          JSONParser::Token(':'),
                                          JSONParser::Token('['),
                                          JSONParser::Token('{'),
                                          JSONParser::Token("json1"), 
                                          JSONParser::Token(':'),
                                          JSONParser::Token(1),
                                          JSONParser::Token('}'),
                                          JSONParser::Token(','),
                                          JSONParser::Token('{'),
                                          JSONParser::Token("json2"), 
                                          JSONParser::Token(':'),
                                          JSONParser::Token(2),
                                          JSONParser::Token('}'),
                                          JSONParser::Token(']'),
                                          JSONParser::Token('}')};

    tester.AssertEqual(parser.tokens, expected, "TestLexer3", __LINE__);
}

void InitSimpleJson1()
{
    simple_json1.Put("intKey", 2);
}

void InitSimpleJson2()
{
    JSONObject nested_json;
    nested_json.Put("intKey", 2);
    simple_json2.Put("nestedJson", nested_json);
}

void InitSimpleJson3()
{
    JSONArray json_arr;
    
    JSONObject *json1 = new JSONObject();
    json1->Put("json1", 1);
    JSONObject *json2 = new JSONObject();
    json2->Put("json2", 2);

    json_arr.PushBack(json1);
    json_arr.PushBack(json2);

    simple_json3.Put("jsonArray", json_arr);

    delete json1;
    delete json2;
}

void InitRealUseJson1()
{
    JSONArray json_arr;
    
    JSONObject *json1 = new JSONObject();
    json1->Put("x0", -24.136337f);
    json1->Put("y0", 75.754684f);
    json1->Put("x1", -127.218956f);
    json1->Put("y1", -25.416527f);

    JSONObject *json2 = new JSONObject();
    json2->Put("x0", 25.535736f);
    json2->Put("y0", -43.788517f);
    json2->Put("x1", -67.682999f);
    json2->Put("y1", 82.133118f);

    JSONObject *json3 = new JSONObject();
    json3->Put("x0", -108.825356f);
    json3->Put("y0", -80.391953f);
    json3->Put("x1", 93.193268f);
    json3->Put("y1", -5.138481f);

    JSONObject *json4 = new JSONObject();
    json4->Put("x0", 150.926361f);
    json4->Put("y0", 63.822083f);
    json4->Put("x1", -58.930611f);
    json4->Put("y1", 72.343033f);

    json_arr.PushBack(json1);
    json_arr.PushBack(json2);
    json_arr.PushBack(json3);
    json_arr.PushBack(json4);

    real_use_json.Put("pairs", json_arr);

    delete json1;
    delete json2;
    delete json3;
    delete json4;
}

void PrintTokenList(JSONParser::TokenList token_list)
{
    std::cout << "[";

    for (JSONParser::Token tok : token_list)
    {
        switch (tok.type)
        {
            case JSONParser::TokenType::NULL_TYPE:
            case JSONParser::TokenType::NUM_TOKEN_TYPES:
            {
            } break;

            case JSONParser::TokenType::STR:
            {
                std::cout << tok.str_tok;
            } break;

            case JSONParser::TokenType::PUNCTUATION:
            {
                std::cout << tok.punc_tok;
            } break;
            
            case JSONParser::TokenType::INT:
            {
                std::cout << tok.int_tok;
            } break;

            case JSONParser::TokenType::DOUBLE:
            {
                std::cout << tok.double_tok;
            } break;
        }
    }
    
    std::cout << "]\n";
}