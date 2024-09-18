/* ------------------------------------------*/ 
/* Filename: JSONObject_main.cpp           */
/* Date:     23.07.2024                      */
/* Author:   Oron                            */ 
/* ------------------------------------------*/

#include "JSONObject.h"
#include "generic_test.h"
#include "profiler.h"

using namespace JSORON;

JSONObject CreateJson();

void TestObjectCopyCtor(Tester& tester);
void TestObjectCopyAssignment(Tester& tester);
void TestJSONValueCopyAssignment(Tester& tester);
void TestOperatorSquareBrackets(Tester& tester);
void TestJSONValueCasting(Tester& tester);

struct profiling_data
{
    u64 json_creation;
    u64 test_op_sqr_brck;
    u64 test_obj_cp_ctor;
    u64 test_obj_cp_op;
    u64 test_json_val_assign;
    u64 test_json_val_cast;
    u64 total_cpu;
};

profiling_data pd = {0};

int main(int argc, char *argv[])
{
	Tester tester;

    // JSONObject obj = CreateJson();

    TestOperatorSquareBrackets(tester);

    TestObjectCopyCtor(tester);

    TestObjectCopyAssignment(tester);

    TestJSONValueCopyAssignment(tester);
    
    TestJSONValueCasting(tester);

    tester.TestAll();

    std::cout << "\n"
              << "JSON Creation: " << pd.json_creation << "\n";

	return 0;
}

JSONObject CreateJson()
{
    typedef JSONObject::JSONArray JSONArray;

    /****************/
    // pd.json_creation = profiler::StartCPU();
    /****************/

	JSONObject json;

    json.Put("intKey", 13);
	json.Put("doubleKey", 13.3f);
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
        delete json_obj;
	}	
	
    json.Put("ArrayOfJsons", json_arr);

    /****************/
    // pd.json_creation = profiler::EndCPU(pd.json_creation);
    /****************/

    return json;
}

void TestObjectCopyCtor(Tester& tester)
{
    JSONObject json1 = CreateJson();

    /****************/
    pd.test_obj_cp_ctor = profiler::StartCPU();
    /****************/
    
    JSONObject json2(json1);

    /****************/
    pd.test_obj_cp_ctor = profiler::EndCPU(pd.test_obj_cp_ctor);
    /****************/

    tester.AssertEqual(json2, json1, "TestObjectCopyCtor", __LINE__);
}

void TestObjectCopyAssignment(Tester& tester)
{
    JSONObject json1 = CreateJson();

    /****************/
    pd.test_obj_cp_op = profiler::StartCPU();
    /****************/
    
    JSONObject json2 = json1;

    /****************/
    pd.test_obj_cp_op = profiler::EndCPU(pd.test_obj_cp_op);
    /****************/

    tester.AssertEqual(json2, json1, "TestObjectCopyCtor", __LINE__);
}

void TestJSONValueCopyAssignment(Tester& tester)
{
    JSONObject json = CreateJson();


    /****************/
    u64 start = profiler::StartCPU();
    /****************/
    json["intKey"] = 42;
    JSONObject::JSONValue result = json["intKey"];
    /****************/
    pd.test_json_val_assign = profiler::EndCPU(start);
    /****************/
    tester.AssertEqual(result, JSONObject::JSONValue(42), "TestJSONValueCopyAssignment", __LINE__);

    /****************/
    start = profiler::StartCPU();
    /****************/
    json["strKey"] = json["strKey"];
    result = json["strKey"];
    /****************/
    pd.test_json_val_assign += profiler::EndCPU(start);
    /****************/
    tester.AssertEqual(result, JSONObject::JSONValue(42), "TestJSONValueCopyAssignment", __LINE__);
    
    /****************/
    start = profiler::StartCPU();
    /****************/
    json["intKey"] = "not an int!";
    result = json["intKey"];
    /****************/
    pd.test_json_val_assign += profiler::EndCPU(start);
    /****************/
    tester.AssertEqual(result, JSONObject::JSONValue("not an int!"), "TestJSONValueCopyAssignment", __LINE__);

    /****************/
    start = profiler::StartCPU();
    /****************/
    json["intKey"] = json["nestedJson"];
    result = json["intKey"];
    /****************/
    pd.test_json_val_assign += profiler::EndCPU(start);
    /****************/
    tester.AssertEqual(result, JSONObject::JSONValue(json["nestedJson"]), "TestJSONValueCopyAssignment", __LINE__);
}

void TestOperatorSquareBrackets(Tester& tester)
{
    JSONObject json = CreateJson();

    tester.AssertEqual(JSONObject::JSONValue(13), JSONObject::JSONValue(json["intKey"]), "TestOperatorSquareBrackets", __LINE__);
    tester.AssertEqual(JSONObject::JSONValue("str"), JSONObject::JSONValue(json["strKey"]), "TestOperatorSquareBrackets", __LINE__);

    const JSONObject const_obj = CreateJson();

    tester.AssertEqual(JSONObject::JSONValue(13), JSONObject::JSONValue(const_obj["intKey"]), "TestOperatorSquareBrackets", __LINE__);
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



















