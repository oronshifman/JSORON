/* ------------------------------------------*/ 
/* Filename: JSONObject_main.cpp             */
/* Date:     23.07.2024                      */
/* Author:   Oron                            */ 
/* ------------------------------------------*/

#include "JSONObject.h"
#include "generic_test.h"
#include "profiler.h"

using namespace JSORON;

JSONObject CreateJson()
{
    typedef JSONObject::JSONArray JSONArray;

	JSONObject json;

    json["intKey"] = 13;
	json["doubleKey"] = 13.3f;
	json["strKey"] = "str";

	JSONObject nested_json;
	nested_json["nestedInt"] = 42;

    JSONArray nested_int_arr;
    nested_int_arr.PushBack(1);
    nested_int_arr.PushBack(2);
    nested_int_arr.PushBack(3);
	nested_json["nestedIntArr"] = nested_int_arr;

	json["nestedJson"] = nested_json;

	u64 num_obj = 5;
	JSONArray json_arr;
	for (u64 index = 0; index < num_obj; ++index)
	{
		JSONObject *json_obj = new JSONObject();
		(*json_obj)["num"] = (s32)index;
		json_arr.PushBack(json_obj);
	}	
	
    json["ArrayOfJsons"] = json_arr;

    return json;
}

void TestObjectCopyCtor(Tester& tester)
{
    JSONObject json1 = CreateJson();
    JSONObject json2(json1);

    tester.AssertEqual(json2, json1, "TestObjectCopyCtor", __LINE__);
}

void TestObjectCopyAssignment(Tester& tester)
{
    JSONObject json1 = CreateJson();
    JSONObject json2 = json1;

    tester.AssertEqual(json2, json1, "TestObjectCopyCtor", __LINE__);
}

void TestJSONValueCopyAssignment(Tester& tester)
{
    JSONObject json = CreateJson();

    json["intKey"] = 42;
    tester.AssertEqual(json["intKey"], JSONObject::JSONValue(42), "TestJSONValueCopyAssignment", __LINE__);

    json["strKey"] = json["strKey"];
    tester.AssertEqual(json["intKey"], JSONObject::JSONValue(42), "TestJSONValueCopyAssignment", __LINE__);
    
    json["intKey"] = "not an int!";
    tester.AssertEqual(json["intKey"], JSONObject::JSONValue("not an int!"), "TestJSONValueCopyAssignment", __LINE__);

    json["intKey"] = json["nestedJson"];
    tester.AssertEqual(json["intKey"], JSONObject::JSONValue(json["nestedJson"]), "TestJSONValueCopyAssignment", __LINE__);
}

void TestOperatorSquareBrackets(Tester& tester)
{
    JSONObject json = CreateJson();

    tester.AssertEqual(JSONObject::JSONValue(json["intKey"]), JSONObject::JSONValue(13), "TestOperatorSquareBrackets", __LINE__);
    tester.AssertEqual(JSONObject::JSONValue(json["strKey"]), JSONObject::JSONValue("str"), "TestOperatorSquareBrackets", __LINE__);
}

void TestJSONValueCasting(Tester& tester)
{
    typedef JSONObject::JSONArray JSONArray;

    JSONObject json = CreateJson();

    s32 int_val = json["intKey"];
    JSONArray array_of_jsons = json["ArrayOfJsons"];

    tester.AssertEqual(int_val, 13, "TestJSONValueCasting", __LINE__);

    for (auto new_iter = array_of_jsons.begin(), og_iter = json["ArrayOfJsons"].json_arr.begin();
         new_iter != array_of_jsons.end() && og_iter != json["ArrayOfJsons"].json_arr.end();
         ++new_iter, ++og_iter)
    {
        tester.AssertEqual(*new_iter, *og_iter, "TestJSONValueCasting", __LINE__);
    }
}

void TestJSONArrayIterator(Tester& tester)
{
    typedef JSONObject::JSONArray JSONArray;

    JSONObject json = CreateJson();

    JSONArray json_arr = json["ArrayOfJsons"];
    int count1 = 0;
    for (auto& iter : json_arr)
    {
        tester.AssertEqual((int)iter["num"], count1++, "TestJSONArrayIterator", __LINE__);
    }

    const JSONArray& const_json_arr = json["ArrayOfJsons"];
    int count2 = 0;
    for (auto& iter : const_json_arr)
    {
        tester.AssertEqual((int)iter.At("num"), count2++, "TestJSONArrayIterator", __LINE__);
    }
}

static JSONObject Create4PointJson(void)
{
    JSONArray pairs;
    JSONObject elm1;
    elm1["x0"] = -24.136337;
    elm1["y0"] = 75.754684;
    elm1["x1"] = -127.218956;
    elm1["y1"] = -25.416527;
    JSONObject elm2;
    elm2["x0"] = 25.535736;
    elm2["y0"] = -43.788517;
    elm2["x1"] = -67.682999;
    elm2["y1"] = 82.133118;
    JSONObject elm3;
    elm3["x0"] = -108.825356;
    elm3["y0"] = -80.391953;
    elm3["x1"] = 93.193268;
    elm3["y1"] = -5.138481;
    JSONObject elm4;
    elm4["x0"] = 150.926361;
    elm4["y0"] = 63.822083;
    elm4["x1"] = -58.930611;
    elm4["y1"] = 72.343033;

    pairs.PushBack(elm1);
    pairs.PushBack(elm2);
    pairs.PushBack(elm3);
    pairs.PushBack(elm4);

    JSONObject expected;
    expected["pairs"] = pairs;

    return expected;
}

void TestRealJsonParseString(Tester& tester)
{
    JSONObject obj;
    obj.Parse("{\"pairs\":[{\"x0\":-24.136337,\"y0\":75.754684,\"x1\":-127.218956,\"y1\":-25.416527}, {\"x0\":25.535736,\"y0\":-43.788517,\"x1\":-67.682999,\"y1\":82.133118}, {\"x0\":-108.825356,\"y0\":-80.391953,\"x1\":93.193268,\"y1\":-5.138481}, {\"x0\":150.926361,\"y0\":63.822083,\"x1\":-58.930611,\"y1\":72.343033}]}");

    JSONObject expected = Create4PointJson();

    tester.AssertEqual(obj, expected, "TestRealJson_Lex", __LINE__);
}

void TestParseFile(Tester& tester)
{
    std::string file_path("../../haversine_jsons/4_points.json");
    std::ifstream json_file(file_path);
    if (json_file.good())
    {
        JSONObject from_file;
        {
            Profiler_TimeBlock("Parsing 4_points.json");
            from_file.Parse(json_file);
        }
        JSONObject expected = Create4PointJson();
        tester.AssertEqual(from_file, expected, "TestParseFromFile", __LINE__);

        json_file.close();
    }
    else
    {
        std::cout << "file not found: " << file_path << "\n";
    }

    json_file.open("../../haversine_jsons/uniform_4320980_10000_points.json");
    if (json_file.good())
    {
        JSONObject from_file;
        {
            Profiler_TimeBlock("Parsing uniform_4320980_10000_points.json");
            from_file.Parse(json_file);
        }
        tester.AssertEqual("stress", "stress", "TestParseFromFile", __LINE__);
        json_file.close();
    }
}

int main(int argc, char *argv[])
{
    Profiler::BeginProfiling();

	Tester tester;
    
    TestOperatorSquareBrackets(tester);

    TestObjectCopyCtor(tester);

    TestObjectCopyAssignment(tester);

    TestJSONValueCopyAssignment(tester);

    TestJSONValueCasting(tester);

    TestJSONArrayIterator(tester);

    TestRealJsonParseString(tester);

    TestParseFile(tester);

    tester.TestAll();

    Profiler::EndProfilingAndPrint();

	return 0;
}
