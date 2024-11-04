/* ------------------------------------------*/ 
/* Filename: JSONObject.h                    */
/* Date:     23.07.2024                      */
/* Author:   Oron                            */ 
/* ------------------------------------------*/

#ifndef __JSON_OBJECT_H__
#define __JSON_OBJECT_H__

#include <ostream>
#include <unordered_map>
#include <list>
#include <string>
#include <vector>

#include "my_int.h"

namespace JSORON
{

    class JSONObject 
    {
#ifndef NDEBUG
    public: 
#endif /* NDEBUG */
        enum class ValueType
        {
            BAD_TYPE,
            NULL_TYPE,
    
            KEY,
            
            INT,
            DOUBLE,
            STR,
            JSON_OBJECT,
    
            ARR,
    
            NUM_JSON_TYPES
        };
        
        class JSONValue;
        class JSONArray
        {
        public:
            JSONArray() : array() {}
            JSONArray(const JSONArray& other);
            JSONArray& operator=(const JSONArray& other);

            ~JSONArray();

            typedef std::vector<JSONValue*> ValueArray;

            class Iterator {
            public:
                // Iterator traits
                using iterator_category = std::forward_iterator_tag;
                using value_type = JSONValue;
                using difference_type = std::ptrdiff_t;
                using pointer = JSONValue*;
                using reference = JSONValue&;

                // Constructor
                Iterator(ValueArray::iterator iter) : is_const(0), m_iter(iter) {}
                Iterator(ValueArray::const_iterator iter) : is_const(1), m_const_iter(iter) {}

                // Dereference operator
                reference operator*() const;

                // Pre-increment operator
                Iterator& operator++();

                // Post-increment operator
                Iterator operator++(int);

                // Comparison operators
                bool operator==(const Iterator& other) const;
                bool operator!=(const Iterator& other) const;

            private:
                b8 is_const;
                union
                {
                    ValueArray::iterator m_iter;  // Internal pointer for traversal
                    ValueArray::const_iterator m_const_iter;  // Internal pointer for traversal
                };
                
            };

            u64 Size() const;
            
            template<typename T>
            void PushBack(const T& value);

            void PushBack(JSONValue *value);
            
            JSONValue Erase(u64 index);
            JSONValue& At(u64 index) const;

            Iterator begin() const;
            Iterator end() const;
            Iterator begin();
            Iterator end();

            friend bool operator==(const JSONArray& lhs, const JSONArray& rhs);
            friend bool operator!=(const JSONArray& lhs, const JSONArray& rhs);
        private:
            ValueArray array;
        };
        
        class JSONValue 
        {
        public:
            ValueType type;
    
            union
            {
                s32 int_val;
                f64 double_val;
                std::string str_val;
                JSONObject *json_val;
    
                JSONArray json_arr;
            };
    
            JSONValue() : type(ValueType::NULL_TYPE) {}
            JSONValue(const JSONValue& value);
            JSONValue& operator=(const JSONValue& other);
            
            template<typename T>
            JSONValue& operator=(const T& src);

            ~JSONValue();
    
            JSONValue(const ValueType& type) : type(type) {}
    
            JSONValue(const ValueType type, const std::string& key) : type(type), str_val(key) {}
            JSONValue(const s32 value) : type(ValueType::INT), int_val(value) {}
            JSONValue(const f64 value) : type(ValueType::DOUBLE), double_val(value) {}
            JSONValue(const std::string& value) : type(ValueType::STR), str_val(value) {}
            JSONValue(const JSONObject* value);
            JSONValue(const JSONObject& value);
    
            JSONValue(const JSONArray& arr) : type(ValueType::ARR), json_arr(arr) {}
            
            /**
             * @brief overloading cast to int.
             * @throw bad_cast
             */
            operator int&() const;
            
            /**
             * @brief overloading cast to double.
             * @throw bad_cast
             */
            operator double&() const;
           
            /**
             * @brief overloading cast to string.
             * @throw bad_cast
             */
            operator std::string&() const;
          
            /**
             * @brief overloading cast to JSONObject.
             * @throw bad_cast
             */
            operator JSONObject&() const;
         
            /**
             * @brief overloading cast to JSONArray.
             * @throw bad_cast
             */
            operator JSONArray&() const;
        
            JSONValue operator[](u64 key);

            /**
             * @brief for accessing keys from a nested json
             * @return JSONValue with the nested JSONObject
             */
            JSONValue& operator[](std::string key);
            
            /**
             * @brief for accessing keys from a nested json
             * @return JSONValue with the nested JSONObject
             */
            JSONValue& operator[](const char* key);
            
            /**
             * @brief for accessing keys from a nested json
             * @return JSONValue with the nested JSONObject
             */
            const JSONValue& operator[](std::string key) const;
            
            /**
             * @brief for accessing keys from a nested json
             * @return JSONValue with the nested JSONObject
             */
            const JSONValue& operator[](const char* key) const;

            void PrintValueByType(u8 indent, std::ostream& out) const;
            void AssignValueByType(const JSONValue& src);
    
            friend bool operator==(const JSONObject& lhs, const JSONObject& rhs);
            friend bool operator==(const JSONValue& lhs, const JSONValue& rhs);
            friend bool operator!=(const JSONObject& lhs, const JSONObject& rhs);
            friend bool operator!=(const JSONValue& lhs, const JSONValue& rhs);
            friend std::ostream& operator<<(std::ostream& out, const JSONValue& value);
        };
    
    public:
        JSONObject() : json(), insertion_order() {}
        JSONObject(const JSONObject& other);
        JSONObject& operator=(const JSONObject& obj);
        ~JSONObject();
    
        template<typename T>
        void Put(const std::string key, const T& value);
        
        void Put(const std::string key, JSONValue *value);

        /**
         * @brief adds a new json object to this json
         * @param key the key for the new json object
         * @return a reference to the new object that was inserted
         */
        JSONObject& AddObj(const std::string& key);
        
        /**
         * @brief adds a new array of type T to this json
         * @param key the key for the new array
         * @return a reference to the new array that was inserted
         */
        template<typename T>
        std::vector<T>& AddArr(const std::string& key);
    
        void Remove(std::string key);
       
        /**
         * @brief access values in json object
         * @param key - the key associated with the value to be pulled from the json object
         * @return if key exists in json object, returns a reference the value associated with 
         *         key else return a reference to a JSONValue of type ValueType::NULL_TYPE
         */
        JSONValue& operator[](std::string key);

        /**
         * @brief access values in json object
         * @param key - the key associated with the value to be pulled from the json object
         * @return if key exists in json object, returns a reference the value associated with 
         *         key else return a reference to a JSONValue of type ValueType::NULL_TYPE
         */
        const JSONValue& operator[](std::string key) const;
        
        friend class JSONParser;

        friend bool operator==(const JSONObject& lhs, const JSONObject& rhs);
        friend bool operator!=(const JSONObject& lhs, const JSONObject& rhs);
        friend std::ostream& operator<<(std::ostream& out, const JSONObject& obj);
        friend std::ostream& operator<<(std::ostream& out, const JSONValue& value);

#ifdef NDEBUG 
    private:
#endif /* NDEBUG */
        typedef std::unordered_map<std::string, JSONValue*>::iterator JSONIter;
    
        static const JSONValue bad_value;
        std::unordered_map<std::string, JSONValue*> json;
        std::list<std::string> insertion_order;
    
        void RecPrint(u8 indent, std::ostream& out) const;
    };
    
    typedef JSONObject::JSONArray JSONArray;    
    typedef JSONObject::JSONValue JSONValue;    

    template<typename T>
    void JSONObject::JSONArray::PushBack(const T& value)
    {
        JSONValue *new_val = new JSONValue(value);
        array.push_back(new_val);
    }

    template<typename T>
    JSONObject::JSONValue& JSONObject::JSONValue::operator=(const T& src)
    {
        this->~JSONValue();
    
        JSONValue *new_value = new JSONValue(src);
        AssignValueByType(*new_value);
        delete new_value;
    
        return *this;
    }
    
    template<typename T>
    void JSONObject::Put(const std::string key, const T& value)
    {
        JSONValue* new_value = new JSONValue(value);
        auto res = json.insert({key, new_value});
        if (res.second)
        {
            insertion_order.push_back(key);
        }
    }
    
    template<typename T>
    std::vector<T>& JSONObject::AddArr(const std::string& key)
    {
        std::vector<T> new_arr;
        JSONValue* new_value = new JSONValue(new_arr);
        auto res = json.insert({key, new_value});
        if (res.second)
        {
            insertion_order.push_back(key);
        }
        return res.first->second;
    }
    
}

#endif /* JSON_OBJECT_H */

