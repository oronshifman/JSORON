/* ------------------------------------------*/ 
/* Filename: JSONObject_main.cpp           */
/* Date:     23.07.2024                      */
/* Author:   Oron                            */ 
/* ------------------------------------------*/

#include "JSONObject.h"
#include "generic_test.h"

using namespace JSORON;

JSONObject CreateJson();

void TestObjectCopyCtor(Tester& tester);
void TestObjectCopyAssignment(Tester& tester);
void TestJSONValueCopyAssignment(Tester& tester);
void TestOperatorSquareBrackets(Tester& tester);
void TestJSONValueCasting(Tester& tester);

int main(int argc, char *argv[])
{
	Tester tester;
    
    TestOperatorSquareBrackets(tester);

    TestObjectCopyCtor(tester);

    TestObjectCopyAssignment(tester);

    TestJSONValueCopyAssignment(tester);
    
    TestJSONValueCasting(tester);

    tester.TestAll();

	return 0;
}

JSONObject CreateJson()
{
    typedef JSONObject::JSONArray JSONArray;

	JSONObject json;

    json.Put("intKey", 13);
	json.Put("doubleKey", 13.3);
	json.Put("strKey", "str");

	JSONObject nested_json;
	nested_json.Put("nestedInt", 42);

    JSONArray nested_int_arr;
    nested_int_arr.PushBack(1);
    nested_int_arr.PushBack(2);
    nested_int_arr.PushBack(3);
	nested_json.Put("nestedIntArr", nested_int_arr);

	json.Put("nestedJson", nested_json);

	u64 num_obj = 5;
	JSONArray json_arr;
	for (u64 index = 0; index < num_obj; ++index)
	{
		JSONObject *json_obj = new JSONObject();
		json_obj->Put("num", (s32)index);
		json_arr.PushBack(json_obj);
	}	
	
    json.Put("ArrayOfJsons", json_arr);

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

    tester.AssertEqual(JSONObject::JSONValue(13), JSONObject::JSONValue(json["intKey"]), "TestOperatorSquareBrackets", __LINE__);
    tester.AssertEqual(JSONObject::JSONValue("str"), JSONObject::JSONValue(json["strKey"]), "TestOperatorSquareBrackets", __LINE__);
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


















