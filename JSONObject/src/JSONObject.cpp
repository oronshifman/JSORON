/* ------------------------------------------*/ 
/* Filename: JSONObject.cpp                  */
/* Date:     23.07.2024                      */
/* Author:   Oron                            */ 
/* ------------------------------------------*/

#include <ostream>
#include <string>
#include <cstring>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <charconv>
#include <assert.h>

#include "JSONObject.h"
#include "profiler.h"

#define CHECK_BUF(buf, pos, cond) ((pos) < (buf).size()) && (cond)

namespace JSORON
{

/**************************************************************************************************
 * 
 *  JSONArray
 * 
 **************************************************************************************************/

JSONObject::JSONArray::JSONArray(const JSONObject::JSONArray& other)
{
    DeepCopyFrom(other, nullptr, nullptr);
}

JSONObject::JSONArray::JSONArray(const JSONObject::JSONArray& other, const char *old_base, const char *new_base)
{
    DeepCopyFrom(other, old_base, new_base);
}

JSONObject::JSONArray& JSONObject::JSONArray::operator=(const JSONObject::JSONArray& other)
{
    if (this == &other)
    {
        return *this;
    }

    DeleteAll();
    DeepCopyFrom(other, nullptr, nullptr);
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

JSONObject::JSONValue& JSONObject::JSONArray::At(u64 index)
{
    return const_cast<JSONValue&>(static_cast<const JSONArray&>(*this).At(index));
}

const JSONObject::JSONValue& JSONObject::JSONArray::At(u64 index) const
{
    assert(index < array.size());
    
    return *array[index];
}

JSONObject::JSONValue& JSONObject::JSONArray::operator[](u64 index)
{
    return (*this).At(index);
}

const JSONObject::JSONValue& JSONObject::JSONArray::operator[](u64 index) const
{
    return (*this).At(index);
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

void JSONObject::JSONArray::DeepCopyFrom(const JSONArray& other, const char *old_base, const char *new_base)
{
    array.reserve(other.Size());
    if (old_base && new_base)
    {
        for (auto& val : other)
        {
            PushBack(val, old_base, new_base);
        }
    }
    else
    {
        for (auto& val : other)
        {
            PushBack(val);
        }
    }
}

/**************************************************************************************************
 * 
 *  JSONValue
 * 
 **************************************************************************************************/
JSONObject::JSONValue::JSONValue(const JSONValue& value)
{
    AssignValueByType(value, nullptr, nullptr);
}

JSONObject::JSONValue::JSONValue(const JSONValue& value, const char *old_base, const char *new_base)
{
    AssignValueByType(value, old_base, new_base);
}

JSONObject::JSONValue::JSONValue(const JSONObject& value) : type(ValueType::JSON_OBJECT)
{
    json_val = new JSONObject(value);
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

JSONObject::JSONValue::operator bool&()
{
    return const_cast<bool&>(static_cast<const JSONValue&>(*this).operator const bool&());
}

JSONObject::JSONValue::operator int&()
{
    return const_cast<int&>(static_cast<const JSONValue&>(*this).operator const int&());
}

JSONObject::JSONValue::operator double&()
{
    return const_cast<double&>(static_cast<const JSONValue&>(*this).operator const double&());
}

JSONObject::JSONValue::operator std::string&()
{
    // TODO: implement
}

JSONObject::JSONValue::operator JSONObject&()
{
    return const_cast<JSONObject&>(static_cast<const JSONValue&>(*this).operator const JSONObject&());
}

JSONObject::JSONValue::operator JSONArray&()
{
    return const_cast<JSONArray&>(static_cast<const JSONValue&>(*this).operator const JSONArray&());
}

JSONObject::JSONValue::operator const bool&() const
{
    if (type == JSONObject::ValueType::BOOL)
    {
        return bool_val;
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

JSONObject::JSONValue::operator std::string_view() const
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

JSONObject::JSONValue& JSONObject::JSONValue::At(u64 index)
{
    return const_cast<JSONValue&>(static_cast<const JSONValue&>(*this).At(index));
}

const JSONObject::JSONValue& JSONObject::JSONValue::At(u64 index) const
{
    assert(type == JSONObject::ValueType::ARR);
    return json_arr.At(index);
}

JSONObject::JSONValue& JSONObject::JSONValue::At(const std::string_view key)
{
    return const_cast<JSONValue&>(static_cast<const JSONValue&>(*this).At(key));
}

const JSONObject::JSONValue& JSONObject::JSONValue::At(const std::string_view key) const
{
    assert(type == JSONObject::ValueType::JSON_OBJECT);
    return json_val->At(key);
}

JSONObject::JSONValue& JSONObject::JSONValue::operator[](u64 index)
{
    return (*this).At(index);
}

JSONObject::JSONValue& JSONObject::JSONValue::operator[](std::string_view key)
{
    assert(type == JSONObject::ValueType::JSON_OBJECT);
    return json_val->operator[](key);
}

JSONObject::JSONValue& JSONObject::JSONValue::operator[](const char *key)
{
    return (*this)[std::string_view(key)];
}

void JSONObject::JSONValue::PrintValueByType(u8 indent, std::ostream& out) const
{
    switch (type)
    {
        case JSONObject::ValueType::NULL_TYPE:
        {
            out << "null" << "\n";
        } break;

        case JSONObject::ValueType::BOOL:
        {
            out << (bool_val ? "true" : "false") << "\n";
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
            char tabs[256];
            memset(tabs, '\t', indent);
            out.write(tabs, indent);
            out << "}\n";
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

        default:
            break;
    }
}

void JSONObject::JSONValue::DestroyCurrentValue(void)
{
    switch (this->type)
    {   
        case JSONObject::ValueType::JSON_OBJECT:
        {
            delete this->json_val;
        } break;

        case JSONObject::ValueType::ARR:
        {
            json_arr.~JSONArray();
        } break;
        
        case JSONObject::ValueType::STR:
        case JSONObject::ValueType::NULL_TYPE:
        case JSONObject::ValueType::INT:
        case JSONObject::ValueType::DOUBLE:
        default:
            break;
    }

    this->type = JSONObject::ValueType::NULL_TYPE;
}

void JSONObject::JSONValue::AssignValueByType(const JSONValue& src, const char *old_base, const char *new_base)
{
    switch (src.type)
    {   
        case JSONObject::ValueType::BOOL:
        {
            type = ValueType::BOOL;
            bool_val = src.bool_val;
        } break;

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
            if (old_base && new_base)
            {
                str_val= std::string_view(
                    new_base + (src.str_val.data() - old_base),
                    src.str_val.size());
            }
            else
            {
                str_val = src.str_val;
            }
        } break;

        case JSONObject::ValueType::JSON_OBJECT:
        {
            type = ValueType::JSON_OBJECT;
            if (old_base && new_base)
            {
                json_val = new JSONObject(*src.json_val, old_base, new_base);
            }
            else
            {
                json_val = new JSONObject(*src.json_val);
            }
        } break;

        case JSONObject::ValueType::ARR:
        {
            type = ValueType::ARR;
            if (old_base && new_base)
            {
                new (&json_arr) JSONArray(src.json_arr, old_base, new_base);
            }
            else
            {
                new (&json_arr) JSONArray(src.json_arr);
            }
        } break;

        case JSONObject::ValueType::NULL_TYPE:
        case JSONObject::ValueType::NUM_JSON_TYPES:
        {
            type = ValueType::NULL_TYPE;
        } break;

        default:
            type = ValueType::BAD_TYPE;
            break;
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
JSONObject::JSONObject(const JSONObject& other)
{
    DeepCopyFrom(other, nullptr, nullptr);
}

JSONObject::JSONObject(const JSONObject& other, const char *old_base, const char *new_base)
{
    DeepCopyFrom(other, old_base, new_base);
}

JSONObject& JSONObject::operator=(const JSONObject& other)
{
    if (this == &other)
    {
        return *this;
    }
    
    DeleteAllJson();
    insertion_order.clear();
    DeepCopyFrom(other, nullptr, nullptr);
   
    return *this;
}

JSONObject::~JSONObject()
{
    DeleteAllJson();
    insertion_order.clear();
}

void JSONObject::DeepCopyFrom(const JSONObject& other, const char *_old_base, const char *_new_base)
{
    if (other.source_buffer.size() != 0)
    {
        source_buffer = other.source_buffer;
    }

    const char *old_base;
    const char *new_base;
    if (_old_base && _new_base)
    {
        old_base = _old_base;
        new_base = _new_base;
    }
    else
    {
        old_base = other.source_buffer.data();
        new_base = source_buffer.data();
    }

    for (auto& [key, val] : other.json)
    {
        std::string_view new_key(new_base + (key.data() - old_base), key.size());
        JSONValue *copy = new JSONValue(*(val), old_base, new_base);
        json.insert({new_key, copy});
    }

    for (auto& key : other.insertion_order)
    {
        std::string_view new_key(new_base + (key.data() - old_base), key.size());
        insertion_order.push_back(new_key);
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

JSONObject::JSONValue& JSONObject::At(const std::string_view key)
{
    return const_cast<JSONValue&>(static_cast<const JSONObject&>(*this).At(key));
}

const JSONObject::JSONValue& JSONObject::At(const std::string_view key) const
{
    auto value = json.find(key);
    if (value == json.end())
    {
        throw std::out_of_range("Key not found: " + std::string(key));
    }
    return *(value->second);
}

void JSONObject::Remove(std::string_view key)
{
    auto iter = json.find(key);
    if (iter == json.end()) return;

    delete iter->second;
    json.erase(iter);
    insertion_order.remove(key);
}

static void SkipWhitespace(const std::string& buf, u32& pos)
{
    while (CHECK_BUF(buf, pos, (std::isspace(buf[pos])))) 
    {
        ++pos;
    }
}

/**
 * @brief skips optional comma and surrounding whitespace between JSON elements
 * If buf[pos] is a comma (after whitespace), advances past it and trailing whitespace.
 * No-op if there is no comma (normal for the last element before ] or }).
 */
static void SkipComma(const std::string& buf, u32& pos)
{
    SkipWhitespace(buf, pos);
    if (CHECK_BUF(buf, pos, (buf[pos] == ','))) 
    {
        ++pos;
        SkipWhitespace(buf, pos);
    }
}

/**
 * @brief parses a JSON string value from buf starting at pos (past the opening ")
 * Scans until the closing " and returns a string_view into buf.
 * Advances pos past the closing ".
 */
static std::string_view ParseStr(const std::string& buf, u32& pos)
{
    u32 start = pos;
    while (CHECK_BUF(buf, pos, (buf[pos] != '"'))) ++pos;

    std::string_view key(&buf[start], pos - start);

    if (CHECK_BUF(buf, pos, (buf[pos] == '"')))
    {
        ++pos; // skip closing "
        return key;
    }
    else
    {
        return std::string_view();
    }
}

static JSONValue *ParseVal(const std::string& buf, u32& pos);

/**
 * @brief parses the body of a JSON object (after the opening {)
 * Reads key-value pairs and inserts them into obj.
 * Advances pos past the closing }.
 * @return nullptr on success, or a BAD_TYPE JSONValue* on parse error.
 */
static JSONValue *ParseObj(const std::string& buf, u32& pos, JSONObject& obj)
{
    while (CHECK_BUF(buf, pos, (buf[pos] != '}')))
    {
        SkipWhitespace(buf, pos);
        
        ++pos; // skip opening "
        std::string_view key = ParseStr(buf, pos);
        if (key.data() == nullptr)
        {
            return new JSONValue(JSONObject::ValueType::BAD_TYPE);
        }

        SkipWhitespace(buf, pos);
        ++pos; // skip :
        SkipWhitespace(buf, pos);

        JSONValue *val = ParseVal(buf, pos);
        if (val->type == JSONObject::ValueType::BAD_TYPE)
        {
            return val;
        }

        obj.json.insert({key, val});
        obj.insertion_order.push_back(key);

        SkipComma(buf, pos);
    }

    if (CHECK_BUF(buf, pos, (buf[pos] == '}')))
    {
        ++pos; // skip }
        return nullptr;
    }
    else
    {
        return new JSONValue(JSONObject::ValueType::BAD_TYPE);
    }
}

/**
 * @brief parses the body of a JSON array (after the opening [)
 * Reads values and pushes them into arr.
 * Advances pos past the closing ].
 * @return nullptr on success, or a BAD_TYPE JSONValue* on parse error.
 */
static JSONValue *ParseArr(const std::string& buf, u32& pos, JSONArray& arr)
{
    while (CHECK_BUF(buf, pos, (buf[pos] != ']')))
    {
        SkipWhitespace(buf, pos);
        JSONValue *val = ParseVal(buf, pos);
        if (val->type != JSONObject::ValueType::BAD_TYPE)
        {
            arr.PushBack(val);
            SkipComma(buf, pos);
        }
        else
        {
            return val;
        }
    }

    if (CHECK_BUF(buf, pos, (buf[pos] == ']')))
    {
        ++pos; // skip ]
        return nullptr;
    }
    else
    {
        return new JSONValue(JSONObject::ValueType::BAD_TYPE);
    }
}

/**
 * @brief parses a JSON number (integer or floating-point) from buf starting at pos
 * Uses std::from_chars for zero-allocation conversion.
 * Returns a JSONValue with INT or DOUBLE type. Advances pos past the number.
 */
static JSONValue *ParseNum(const std::string& buf, u32& pos)
{
    u32 start = pos;
    
    if (CHECK_BUF(buf, pos, (buf[pos] == '-'))) ++pos;
    while (CHECK_BUF(buf, pos, (std::isdigit(buf[pos])))) ++pos;

    b8 is_float = 0;
    if (CHECK_BUF(buf, pos, (buf[pos] == '.')))
    {
        is_float = 1;
        ++pos;
        while (CHECK_BUF(buf, pos, (std::isdigit(buf[pos])))) ++pos;
    }

    if (is_float)
    {
        f64 new_float;
        std::from_chars(&buf[start], &buf[pos], new_float);
        return new JSONValue(new_float);
    }
    else 
    {
        s32 new_int;
        std::from_chars(&buf[start], &buf[pos], new_int);
        return new JSONValue(new_int);
    }
}

/**
 * @brief parses a JSON literal: true, false, or null
 * Returns a JSONValue with BOOL type (true/false) or NULL_TYPE (null).
 * Returns BAD_TYPE if the literal is unrecognized. Advances pos past the literal.
 */
static JSONValue *ParseTFN(const std::string& buf, u32& pos)
{
    u32 start = pos;
    while (CHECK_BUF(buf, pos, (std::isalpha(buf[pos])))) ++pos;

    std::string_view value(&buf[start], pos - start);

    if (value == "null")
    {
        return new JSONValue();
    }
    else if (value == "false")
    {
        return new JSONValue((bool)false);
    }
    else if (value == "true")
    {
        return new JSONValue((bool)true);
    }

    return new JSONValue(JSONObject::ValueType::BAD_TYPE);
}

/**
 * @brief parses a single JSON value starting at buf[pos]
 * Dispatches to ParseObj, ParseArr, ParseStr, ParseNum, or ParseTFN
 * based on the current character. Advances pos past the value.
 * @return the parsed JSONValue*, or a BAD_TYPE JSONValue* on parse error.
 */
static JSONValue *ParseVal(const std::string& buf, u32& pos)
{
    if (pos >= buf.size())
    {
        return new JSONValue(JSONObject::ValueType::BAD_TYPE);
    }

    switch (buf[pos])
    {
        case '{':
        {
            JSONObject *obj = new JSONObject();
            ++pos; // skip {
            JSONValue *val = ParseObj(buf, pos, *obj);
            if (!val)
            {
                return new JSONValue(obj);
            }
            else if (val->type == JSONObject::ValueType::BAD_TYPE)
            {
                delete obj;
                return val;
            }
        } break;

        case '[':
        {
            JSONArray arr;
            ++pos; // skip [
            JSONValue *val = ParseArr(buf, pos, arr);
            if (!val)
            {
                return new JSONValue(std::move(arr));
            }
            else if (val->type == JSONObject::ValueType::BAD_TYPE)
            {
                return val;
            }
        } break;

        case '"':
        {
            ++pos; // skip "
            std::string_view val = ParseStr(buf, pos);
            if (val.data() == nullptr)
            {
                return new JSONValue(JSONObject::ValueType::BAD_TYPE);
            }
            else
            {
                return new JSONValue(val);
            }
        } break;

        default:
        {
            if (std::isdigit(buf[pos]) || buf[pos] == '-')
            {
                return ParseNum(buf, pos);
            } 
            else if (std::isalpha(buf[pos]))
            {
                return ParseTFN(buf, pos);
            }
            else
            {
                return new JSONValue(JSONObject::ValueType::BAD_TYPE);
            }
        } break;
    }

    return new JSONValue(JSONObject::ValueType::BAD_TYPE);
}

/**
 * @brief entry point for the single-pass parser
 * Skips leading whitespace, expects an opening {, then delegates to ParseObj
 * to populate obj with the parsed key-value pairs.
 * @param buf the source buffer containing JSON text (owned by the root JSONObject)
 * @param obj the JSONObject to populate with parsed data
 * @return 0 on success, 1 if the input is not a valid JSON object or a parse
 *         error occurred.
 */
static int ParseSourceBuf(std::string& buf, JSONObject& obj)
{
    u32 pos = 0;
    SkipWhitespace(buf, pos);
    if (CHECK_BUF(buf, pos, (buf[pos] == '{')))
    {
        ++pos; // skip opening {
        JSONValue *val = ParseObj(buf, pos, obj);
        if (!val)
        {
            return 0;
        }
        else if (val->type == JSONObject::ValueType::BAD_TYPE)
        {
            delete val;
            return 1;
        }
    }
    return 1;
}

int JSONObject::Parse(std::ifstream& json_file)
{
    if (!json_file.is_open())
    {
        return 1;
    }

    json_file.seekg(0, std::ios_base::end);
    std::streampos file_end = json_file.tellg();
    if (file_end == std::streampos(-1))
    {
        return 1;
    }

    json_file.seekg(0);
    u64 file_size = static_cast<u64>(file_end);
    source_buffer.resize(file_size);

    json_file.read(&source_buffer[0], file_size);
    if (json_file.fail())
    {
        return 1;
    }

    return ParseSourceBuf(source_buffer, *this);
}

int JSONObject::Parse(const std::string& json_str)
{
    source_buffer = json_str;

    return ParseSourceBuf(source_buffer, *this);
}

JSONObject::JSONValue& JSONObject::operator[](std::string_view key)
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
        case JSONObject::ValueType::BOOL:
            return lhs.bool_val == rhs.bool_val;

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
            if ((lhs.type == JSONObject::ValueType::NULL_TYPE) &&
                (rhs.type == JSONObject::ValueType::NULL_TYPE))
            {
                return 1;
            }
            else
            {
                return 0;
            }
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
    char tabs[256];
    for (std::string_view key : insertion_order)
    {
        const JSONObject::JSONValue* value = json.at(key);

        memset(tabs, '\t', indent);
        out.write(tabs, indent);
        out << "\"" << key << "\": ";
        value->PrintValueByType(indent, out);
    }
}

} // namespace JSORON
