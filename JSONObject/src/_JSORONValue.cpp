/* ------------------------------------------*/ 
/* Filename: JSORONValue.cpp                 */
/* Date:     23.07.2024                      */
/* Author:   Oron                            */ 
/*         FOR TESTING JSORONValue ONLY      */
/* ------------------------------------------*/

#include <vector>

#include "my_int.h"
#include "_JSORONValue.h"

JSORONValue::JSORONValue(const JSORONObject &value)
{
    // NOTE(26.07.24): should contain the following two line!
    // type = JSORONType::JSORON_OBJECT;
    // json_val = new JSORONObject(value);
}

JSORONValue::~JSORONValue()
{
    switch (type)
    {
        case JSORONType::NULL_TYPE:
        case JSORONType::INT:
        case JSORONType::FLOAT:
        case JSORONType::NUM_JSORON_TYPES:
        {

        } break;

        case JSORONType::STR:
        {
            str_val.~basic_string();
        } break;
        
        case JSORONType::JSORON_OBJECT:
        {
            delete json_val;
        } break;

        case JSORONType::INT_ARR:
        {
            int_arr.~vector<s32>();
        } break;

        case JSORONType::FLOAT_ARR:
        {
            float_arr.~vector<f64>();
        } break;

        case JSORONType::STR_ARR:
        {
            str_arr.~vector<std::string>();
        } break;

        case JSORONType::OBJ_ARR:
        {
            // NOTE(26.07.24): should contain the following two line!
            // for (JSORONObject *obj : obj_arr)
            // {
            //     obj->~JSORONObject();
            // }
            // obj_arr.~vector<JSORONObject*>();
        } break;
    }
}

