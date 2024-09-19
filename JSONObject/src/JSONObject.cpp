/* ------------------------------------------*/ 
/* Filename: JSONObject.cpp                  */
/* Date:     23.07.2024                      */
/* Author:   Oron                            */ 
/* ------------------------------------------*/

#include <ostream>
#include <string>
#include <iostream>
#include <assert.h>

#include "JSONObject.h"

namespace JSORON
{

JSONObject::JSONArray::JSONArray(const JSONObject::JSONArray& other)
{
    array.assign(other.array.begin(), other.array.end());
}

JSONObject::JSONArray& JSONObject::JSONArray::operator=(const JSONObject::JSONArray& other)
{
    array.assign(other.array.begin(), other.array.end());
    return *this;
}

JSONObject::JSONArray::~JSONArray()
{
    array.clear();
}

JSONObject::JSONValue JSONObject::JSONArray::Erase(u64 index)
{
    assert(index < array.size());

    auto iter = array.erase(std::next(array.begin(), index));
    return *iter;
}

JSONObject::JSONValue JSONObject::JSONArray::At(u64 index) const
{
    assert(index < array.size());
    
    return array[index];
}

u64 JSONObject::JSONArray::Size() const
{
    return array.size();
}

std::vector<JSONObject::JSONValue>::iterator JSONObject::JSONArray::begin()
{
    return array.begin();
}

std::vector<JSONObject::JSONValue>::iterator JSONObject::JSONArray::end()
{
    return array.begin();
}

JSONObject::JSONValue JSONObject::bad_value(JSONObject::ValueType::BAD_TYPE);

JSONObject::JSONValue::JSONValue(const JSONValue &value)
{
    AssignValueByType(value);
}

JSONObject::JSONValue::JSONValue(const JSONObject &value) : type(ValueType::JSON_OBJECT)
{
    json_val = new JSONObject(value);
}

JSONObject::JSONValue::JSONValue(const JSONObject *value) : type(ValueType::JSON_OBJECT)
{
    json_val = new JSONObject(*value);
}

JSONObject::JSONValue& JSONObject::JSONValue::operator=(const JSONValue& other)
{
    if (this == &other)
    {
        return *this;
    }
    
    AssignValueByType(other);
    return *this;
}

JSONObject::JSONValue::operator int() const
{
    if (type == JSONObject::ValueType::INT)
    {
        return int_val;
    }
    else
    {
        throw std::bad_cast();
    }
}

JSONObject::JSONValue::operator double() const
{
    if (type == JSONObject::ValueType::DOUBLE)
    {
        return double_val;
    } else
    {
        throw std::bad_cast();
    }
}

JSONObject::JSONValue::operator std::string() const
{
    if (type == JSONObject::ValueType::STR)
    {
        return str_val;
    }
    else
    {
        throw std::bad_cast();
    }
}

JSONObject::JSONValue::operator JSONObject*() const
{
    if (type == JSONObject::ValueType::JSON_OBJECT)
    {
        return json_val;
    }
    else
    {
        throw std::bad_cast();
    }
}

JSONObject::JSONValue::operator JSONArray() const
{
    if (type == JSONObject::ValueType::ARR)
    {
        return json_arr;
    }
    else
    {
        throw std::bad_cast();
    }
}

JSONObject::JSONValue JSONObject::JSONValue::operator[](u64 index)
{
    switch (type)
    {
        case JSONObject::ValueType::BAD_TYPE:
        case JSONObject::ValueType::NULL_TYPE:
        case JSONObject::ValueType::INT:
        case JSONObject::ValueType::DOUBLE:
        case JSONObject::ValueType::STR:
        case JSONObject::ValueType::JSON_OBJECT:
        {
            return *this;
        } break; 

        case JSONObject::ValueType::ARR:
        {
            return json_arr.At(index);
        } break;
    }
}

JSONObject::JSONValue JSONObject::JSONValue::operator[](std::string key)
{
    if (type == JSONObject::ValueType::JSON_OBJECT)
    {
        return (*json_val)[key];
    }

    return JSONObject::bad_value;
}

void JSONObject::JSONValue::PrintValueByType(u8 indent, std::ostream& out) const
{
        switch (type)
        {
            case JSONObject::ValueType::BAD_TYPE:
            case JSONObject::ValueType::NULL_TYPE:
            {
            } break;

            case JSONObject::ValueType::INT:
            {
                out << int_val << "\n";
            } break;

            case JSONObject::ValueType::DOUBLE:
            {
                out << double_val << "\n";
            } break;

            case JSONObject::ValueType::STR:
            {
                out << "\"" << str_val << "\"" << "\n";
            } break;

            case JSONObject::ValueType::JSON_OBJECT:
            {
                out << "{\n";
                json_val->RecPrint(indent + 1, out);
                out << std::string(indent, '\t') << "}\n";
            } break;

            case JSONObject::ValueType::ARR:
            {
                out << "[";
                for (u64 index = 0; index < json_arr.Size() ; ++index)
                {
                    out << json_arr.At(index) << 
                           (index == json_arr.Size() - 1 ? "]" : ",");
                }
                out << "\n";
            } break;

            case JSONObject::ValueType::NUM_JSON_TYPES:
            {
            } break;
        }
}

void JSONObject::JSONValue::AssignValueByType(const JSONValue& src)
{
    switch (src.type)
    {   
        case JSONObject::ValueType::INT:
        {
            type = ValueType::INT;
            int_val = src.int_val;
        } break;

        case JSONObject::ValueType::DOUBLE:
        {
            type = ValueType::DOUBLE;
            double_val = src.double_val;
        } break;

        case JSONObject::ValueType::STR:
        {
            type = ValueType::STR;
            new (&str_val) std::string(src.str_val);
        } break;

        case JSONObject::ValueType::JSON_OBJECT:
        {
            type = ValueType::JSON_OBJECT;
            json_val = new JSONObject(*src.json_val);
        } break;

        case JSONObject::ValueType::ARR:
        {
            type = ValueType::ARR;
            new (&json_arr) JSONArray(src.json_arr);
        } break;

        case JSONObject::ValueType::NULL_TYPE:
        case JSONObject::ValueType::NUM_JSON_TYPES:
        {
            type = ValueType::NULL_TYPE;
        } break;
        
        case JSONObject::ValueType::BAD_TYPE:
        {
        } break;
    }
}


JSONObject::JSONValue::~JSONValue()
{
    switch (type)
    {
        case ValueType::NULL_TYPE:
        case ValueType::INT:
        case ValueType::DOUBLE:
        case ValueType::NUM_JSON_TYPES:
        {
            type = ValueType::NULL_TYPE;
        } break;

        case ValueType::STR:
        {
            str_val.~basic_string();
        } break;
        
        case ValueType::JSON_OBJECT:
        {
            delete json_val;
        } break;

        case ValueType::ARR:
        {
            json_arr.~JSONArray();
        } break;

        case JSONObject::ValueType::BAD_TYPE:
        {
        } break;
    }
}

JSONObject::JSONObject(const JSONObject& other) : insertion_order(other.insertion_order)
{
    for (auto& pair : other.json)
    {
        JSONValue *copy_value = new JSONValue(*(pair.second));
        json.insert({pair.first, copy_value});
    }
}

JSONObject& JSONObject::operator=(const JSONObject& other)
{
    if (this == &other)
    {
        return *this;
    }
    
    json.clear();
    json.insert(other.json.begin(), other.json.end());
   
    insertion_order.assign(other.insertion_order.begin(), other.insertion_order.end());

    return *this;
}

JSONObject::~JSONObject()
{
    for (auto& pair : json)
    {
        delete pair.second;
    }
    json.clear();
    insertion_order.clear();
}


JSONObject& JSONObject::AddObj(const std::string &key)
{
    JSONObject *new_obj = new JSONObject();
    return *new_obj;
}   

void JSONObject::Remove(std::string key)
{
    // TODO(28.07.24): impl
}

JSONObject::JSONValue& JSONObject::operator[](std::string key)
{
    JSONIter value = json.find(key);
    if (value == json.end())
    {
        return bad_value;
    }
    return *(value->second);
}

bool operator==(const JSONObject::JSONArray& lhs, const JSONObject::JSONArray& rhs)
{
    if (&lhs == &rhs)
    {
        return 1;
    }
    
    for (auto lhs_iter = lhs.array.begin(), rhs_iter = rhs.array.begin();
         lhs_iter != lhs.array.end() && rhs_iter != rhs.array.end();
         ++lhs_iter, ++rhs_iter)
    {
        if (*lhs_iter != *rhs_iter)
        {
            return 0;
        }
    }

    return 1;
}


bool operator!=(const JSONObject::JSONArray& lhs, const JSONObject::JSONArray& rhs)
{
    return !(lhs == rhs);
}

// TODO(20.08.24): fix equality check! currently not working!
bool operator==(const JSONObject& lhs, const JSONObject& rhs)
{
    if (&lhs == &rhs)
    {
        return 1;
    }
    
    for (auto lhs_iter = lhs.insertion_order.begin(), rhs_iter = rhs.insertion_order.begin();
         lhs_iter != lhs.insertion_order.end() && rhs_iter != rhs.insertion_order.end();
         ++lhs_iter, ++rhs_iter)
    {
        if (*lhs_iter != *rhs_iter)
        {
            return 0;
        }

        if (*(lhs.json.at(*lhs_iter)) != *(rhs.json.at(*rhs_iter)))
        {
            return 0;
        }
    }

    return 1;
}

bool operator!=(const JSONObject& lhs, const JSONObject& rhs)
{
    return !(lhs == rhs);
}

bool operator==(const JSONObject::JSONValue& lhs, const JSONObject::JSONValue& rhs)
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
        case JSONObject::ValueType::BAD_TYPE:
        case JSONObject::ValueType::NULL_TYPE:
        {
            return 1;
        } break;

        case JSONObject::ValueType::INT:
        {
            return lhs.int_val == rhs.int_val;
        } break;
        
        case JSONObject::ValueType::DOUBLE:
        {
            return lhs.double_val == rhs.double_val;
        } break;
        
        case JSONObject::ValueType::STR:
        {
            return lhs.str_val == rhs.str_val;
        } break;

        case JSONObject::ValueType::JSON_OBJECT:
        {
            return *(lhs.json_val) == *(rhs.json_val);
        } break;
        
        case JSONObject::ValueType::ARR:
        {
            if (lhs.json_arr != rhs.json_arr)
            {
                return 0;
            }
        } break;
        
        default:
        {
             // TODO(16.8.24): SyntaxError() type not supported
             return 0;
        } break;
    }

    return 1;
}

bool operator!=(const JSONObject::JSONValue& lhs, const JSONObject::JSONValue& rhs)
{
    return !(lhs == rhs);
}

std::ostream& operator<<(std::ostream& out, const JSONObject& obj)
{
    out << "{\n";
    obj.RecPrint(1, out);
    out << "}\n";

    return out;
}

std::ostream& operator<<(std::ostream& out, const JSONObject::JSONValue& value)
{
    value.PrintValueByType(0, out);
    return out;
}

void JSONObject::RecPrint(u8 indent, std::ostream& out) const
{
    for (std::string key : insertion_order)
    {
        const JSONObject::JSONValue* value = json.at(key);

        out << std::string(indent, '\t') << "\"" + key + "\": ";
        value->PrintValueByType(indent, out);
    }
}

} // namespace JSORON
