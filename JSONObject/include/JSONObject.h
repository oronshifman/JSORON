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
                using difference_type = std::ptrdiff_t;
                using value_type = JSONValue;
                using pointer = JSONValue*;
                using reference = JSONValue&;

                // Constructor
                Iterator(ValueArray::iterator iter) : m_iter(iter) {}

                // Dereference operator
                reference operator*() const {return **m_iter;};

                // Pre-increment operator
                Iterator& operator++() {++m_iter; return *this;};

                // Post-increment operator
                Iterator operator++(int) {Iterator t = *this; ++m_iter; return t;};

                // Comparison operators
                bool operator==(const Iterator& other) const {return m_iter == other.m_iter;};
                bool operator!=(const Iterator& other) const {return m_iter != other.m_iter;};

            private:
                friend class JSONArray;
                ValueArray::iterator m_iter;
            };

            class ConstIterator {
            public:
                // Iterator traits
                using iterator_category = std::forward_iterator_tag;
                using difference_type = std::ptrdiff_t;
                using value_type = const JSONValue;
                using pointer = const JSONValue*;
                using reference = const JSONValue&;

                // Constructor
                ConstIterator(ValueArray::const_iterator iter) : m_iter(iter) {}

                // Dereference operator
                reference operator*() const {return **m_iter;};

                // Pre-increment operator
                ConstIterator& operator++() {++m_iter; return *this;};

                // Post-increment operator
                ConstIterator operator++(int) {ConstIterator t = *this; ++m_iter; return t;};

                // Comparison operators
                bool operator==(const ConstIterator& other) const {return m_iter == other.m_iter;};
                bool operator!=(const ConstIterator& other) const {return m_iter != other.m_iter;};

            private:
                friend class JSONArray;
                ValueArray::const_iterator m_iter;
            };

            u64 Size() const;
            
            template<typename T>
            void PushBack(const T& value);

            void PushBack(JSONValue *value);
            
            Iterator Erase(u64 index);
            Iterator Erase(Iterator pos);
            // TODO: fix that At for a const JSONArray return a const reference
            // TODO: add an api for At for non const JSONArray
            JSONValue& At(u64 index) const;

            ConstIterator begin() const;
            ConstIterator end() const;
            Iterator begin();
            Iterator end();

            friend bool operator==(const JSONArray& lhs, const JSONArray& rhs);
            friend bool operator!=(const JSONArray& lhs, const JSONArray& rhs);

        private:
            void DeepCopyFrom(const JSONArray& other);
            void DeleteAll(void);

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
            operator int&();
            
            /**
             * @brief overloading cast to double.
             * @throw bad_cast
             */
            operator double&();
           
            /**
             * @brief overloading cast to string.
             * @throw bad_cast
             */
            operator std::string&();
          
            /**
             * @brief overloading cast to JSONObject.
             * @throw bad_cast
             */
            operator JSONObject&();
         
            /**
             * @brief overloading cast to JSONArray.
             * @throw bad_cast
             */
            operator JSONArray&();

            /**
             * @brief overloading cast to int.
             * @throw bad_cast
             */
            operator const int&() const;
            
            /**
             * @brief overloading cast to double.
             * @throw bad_cast
             */
            operator const double&() const;
           
            /**
             * @brief overloading cast to string.
             * @throw bad_cast
             */
            operator const std::string&() const;
          
            /**
             * @brief overloading cast to JSONObject.
             * @throw bad_cast
             */
            operator const JSONObject&() const;
         
            /**
             * @brief overloading cast to JSONArray.
             * @throw bad_cast
             */
            operator const JSONArray&() const;
        
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
            
            void PrintValueByType(u8 indent, std::ostream& out) const;
            void AssignValueByType(const JSONValue& src);
            void DestroyCurrentValue();
    
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
    
        /**
         * @brief retrieves the value associated with the given key
         * @param key the key to look up
         * @return a reference to the associated JSONValue
         * @throws std::out_of_range if the key is not found
         */
        JSONValue& At(const std::string& key);
        const JSONValue& At(const std::string& key) const;
    
        void Remove(std::string key);
       
        /**
         * @brief access or insert a value in the json object
         * @param key the key to look up or create
         * @return a reference to the existing value, or a newly inserted null-type JSONValue
         */
        JSONValue& operator[](std::string key);

        friend class JSONParser;

        friend bool operator==(const JSONObject& lhs, const JSONObject& rhs);
        friend bool operator!=(const JSONObject& lhs, const JSONObject& rhs);
        friend std::ostream& operator<<(std::ostream& out, const JSONObject& obj);
        friend std::ostream& operator<<(std::ostream& out, const JSONValue& value);

#ifdef NDEBUG 
    private:
#endif /* NDEBUG */
        typedef std::unordered_map<std::string, JSONValue*>::iterator JSONIter;
    
        std::unordered_map<std::string, JSONValue*> json;
        std::list<std::string> insertion_order;
    
        void DeepCopyFrom(const JSONObject& other);
        void DeleteAllJson(void);

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
        DestroyCurrentValue();
        new (this) JSONValue(src);
        return *this;
    }
}

#endif /* JSON_OBJECT_H */
