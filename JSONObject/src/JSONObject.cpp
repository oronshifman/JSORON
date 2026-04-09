/* ------------------------------------------*/ 
/* Filename: JSONObject.cpp                  */
/* Date:     23.07.2024                      */
/* Author:   Oron                            */ 
/* ------------------------------------------*/

#include <ostream>
#include <string>
#include <iostream>
#include <algorithm>
#include <assert.h>

#include "JSONObject.h"
#include "profiler.h"

namespace JSORON
{

/**************************************************************************************************
 * 
 *  JSONArray
 * 
 **************************************************************************************************/

JSONObject::JSONArray::JSONArray(const JSONObject::JSONArray& other)
{
    DeepCopyFrom(other);
}

JSONObject::JSONArray& JSONObject::JSONArray::operator=(const JSONObject::JSONArray& other)
{
    if (this == &other)
    {
        return *this;
    }

    DeleteAll();
    return *this;
}

JSONObject::JSONArray::~JSONArray()
{
    DeleteAll();
}

void JSONObject::JSONArray::PushBack(JSONValue *value)
{
    array.push_back(value);
}

JSONObject::JSONArray::Iterator JSONObject::JSONArray::Erase(u64 index)
{
    assert(index < array.size());

    auto iter = Erase(Iterator(std::next(array.begin(), index)));
    return iter;
}

JSONObject::JSONArray::Iterator JSONObject::JSONArray::Erase(Iterator pos)
{
    assert(pos.m_iter < array.end());

    delete *pos.m_iter;

    auto iter = array.erase(pos.m_iter);
    return Iterator(iter);
}

JSONObject::JSONValue& JSONObject::JSONArray::At(u64 index) const
{
    assert(index < array.size());
    
    return *array[index];
}

u64 JSONObject::JSONArray::Size() const
{
    return array.size();
}

JSONObject::JSONArray::ConstIterator JSONObject::JSONArray::begin() const
{
    return ConstIterator(array.begin());
}

JSONObject::JSONArray::ConstIterator JSONObject::JSONArray::end() const
{
    return ConstIterator(array.end());
}

JSONObject::JSONArray::Iterator JSONObject::JSONArray::begin()
{
    return Iterator(array.begin());
}

JSONObject::JSONArray::Iterator JSONObject::JSONArray::end()
{
    return Iterator(array.end());
}

void JSONObject::JSONArray::DeleteAll(void)
{
    for (auto *val : array)
    {
        delete val;
    }
    array.clear();
}

void JSONObject::JSONArray::DeepCopyFrom(const JSONArray& other)
{
    array.reserve(other.Size());
    for (auto& val : other)
    {
        PushBack(val);
    }
}

/**************************************************************************************************
 * 
 *  JSONValue
 * 
 **************************************************************************************************/
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
    
    DestroyCurrentValue();
    new (this) JSONValue(other);
    return *this;
}

JSONObject::JSONValue::operator int&()
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

JSONObject::JSONValue::operator double&()
{
    if (type == JSONObject::ValueType::DOUBLE)
    {
        return double_val;
    } else
    {
        throw std::bad_cast();
    }
}

JSONObject::JSONValue::operator std::string&()
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

JSONObject::JSONValue::operator JSONObject&()
{
    if (type == JSONObject::ValueType::JSON_OBJECT)
    {
        return *json_val;
    }
    else
    {
        throw std::bad_cast();
    }
}

JSONObject::JSONValue::operator JSONArray&()
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

JSONObject::JSONValue::operator const int&() const
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

JSONObject::JSONValue::operator const double&() const
{
    if (type == JSONObject::ValueType::DOUBLE)
    {
        return double_val;
    } else
    {
        throw std::bad_cast();
    }
}

JSONObject::JSONValue::operator const std::string&() const
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

JSONObject::JSONValue::operator const JSONObject&() const
{
    if (type == JSONObject::ValueType::JSON_OBJECT)
    {
        return *json_val;
    }
    else
    {
        throw std::bad_cast();
    }
}

JSONObject::JSONValue::operator const JSONArray&() const
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
        case JSONObject::ValueType::ARR:
            return json_arr.At(index);

        default:
            return *this;
    }
}

JSONObject::JSONValue& JSONObject::JSONValue::operator[](std::string key)
{
    assert(type == JSONObject::ValueType::JSON_OBJECT);
    return json_val->operator[](key);
}

JSONObject::JSONValue& JSONObject::JSONValue::operator[](const char* key)
{
    return (*this)[std::string(key)];
}

void JSONObject::JSONValue::PrintValueByType(u8 indent, std::ostream& out) const
{
        switch (type)
        {
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

            case JSONObject::ValueType::KEY:
            case JSONObject::ValueType::NULL_TYPE:
            case JSONObject::ValueType::NUM_JSON_TYPES:
                break;
        }
}

void JSONObject::JSONValue::DestroyCurrentValue(void)
{
    switch (this->type)
    {   
        case JSONObject::ValueType::STR:
        case JSONObject::ValueType::KEY:
        {
            str_val.~basic_string();
        } break;

        case JSONObject::ValueType::JSON_OBJECT:
        {
            delete this->json_val;
        } break;

        case JSONObject::ValueType::ARR:
        {
            json_arr.~JSONArray();
        } break;
        
        case JSONObject::ValueType::NULL_TYPE:
        case JSONObject::ValueType::INT:
        case JSONObject::ValueType::DOUBLE:
        default:
            break;
    }

    this->type = JSONObject::ValueType::NULL_TYPE;
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

        case JSONObject::ValueType::KEY:
        {
            type = ValueType::KEY;
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
    }
}


JSONObject::JSONValue::~JSONValue()
{
    DestroyCurrentValue();
}

/**************************************************************************************************
 * 
 *  JSONObject
 * 
 **************************************************************************************************/

JSONObject::JSONObject(const JSONObject& other) : insertion_order(other.insertion_order)
{
    DeepCopyFrom(other);
}

JSONObject& JSONObject::operator=(const JSONObject& other)
{
    if (this == &other)
    {
        return *this;
    }
    
    DeleteAllJson();
    DeepCopyFrom(other);
   
    insertion_order = other.insertion_order;

    return *this;
}

JSONObject::~JSONObject()
{
    DeleteAllJson();
    insertion_order.clear();
}

void JSONObject::DeepCopyFrom(const JSONObject& other)
{
    for (auto& pair : other.json)
    {
        JSONValue *copy_value = new JSONValue(*(pair.second));
        json.insert({pair.first, copy_value});
    }
}

void JSONObject::DeleteAllJson(void)
{
    for (auto& pair : json)
    {
        delete pair.second;
    }
    json.clear();
}

JSONObject::JSONValue& JSONObject::At(const std::string& key)
{
    return const_cast<JSONValue&>(static_cast<const JSONObject&>(*this).At(key));
}

const JSONObject::JSONValue& JSONObject::At(const std::string& key) const
{
    auto value = json.find(key);
    if (value == json.end())
    {
        throw std::out_of_range("Key not found: " + key);
    }
    return *(value->second);
}

void JSONObject::Remove(std::string key)
{
    auto iter = json.find(key);
    if (iter == json.end()) return;

    delete iter->second;
    json.erase(iter);
    insertion_order.remove(key);
}

JSONObject::JSONValue& JSONObject::operator[](std::string key)
{
    try {
        return this->At(key);
    }
    catch (const std::out_of_range& e) {
        JSONValue *new_val = new JSONValue();
        json.insert({key, new_val});
        insertion_order.push_back(key);
        return *new_val;
    }
}

bool operator==(const JSONObject::JSONArray& lhs, const JSONObject::JSONArray& rhs)
{
    if (&lhs == &rhs)
    {
        return 1;
    }

    if (lhs.Size() != rhs.Size())
    {
        return 0;
    }
   
    for (u64 i = 0; i < lhs.Size(); ++i)
    {
        if (lhs.At(i) != rhs.At(i))
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

bool operator==(const JSONObject& lhs, const JSONObject& rhs)
{
    if (&lhs == &rhs)
    {
        return 1;
    }
    
    if (lhs.json.size() != rhs.json.size() ||
        lhs.insertion_order.size() != rhs.insertion_order.size())
    {
        return 0;
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
        case JSONObject::ValueType::NULL_TYPE:
            return 1;

        case JSONObject::ValueType::INT:
            return lhs.int_val == rhs.int_val;
        
        case JSONObject::ValueType::DOUBLE:
            return lhs.double_val == rhs.double_val;
        
        case JSONObject::ValueType::STR:
            return lhs.str_val == rhs.str_val;

        case JSONObject::ValueType::JSON_OBJECT:
            return *(lhs.json_val) == *(rhs.json_val);
        
        case JSONObject::ValueType::ARR:
        {
            if (lhs.json_arr != rhs.json_arr)
            {
                return 0;
            }
        } break;
        
        default:
             // TODO(16.8.24): SyntaxError() type not supported
             return 0;
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
