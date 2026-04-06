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
    
            BOOL,
            INT,
            DOUBLE,
            STR,            // points to str_val
            MUT_STR,        // points to mut_str_val
            JSON_OBJECT,
    
            ARR,

            BAD_TYPE,
    
            NUM_JSON_TYPES
        };
        
        class JSONValue;
        class JSONArray
        {
        public:
            JSONArray() : array() {}
            JSONArray(const JSONArray& other);
            JSONArray(const JSONArray& other, const char *old_base, const char *new_base);
            JSONArray(JSONArray&& other) : array(std::move(other.array)) {}
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

            JSONValue& At(u64 index);
            const JSONValue& At(u64 index) const;

            JSONValue& operator[](u64 index);
            const JSONValue& operator[](u64 index) const;

            ConstIterator begin() const;
            ConstIterator end() const;
            Iterator begin();
            Iterator end();

            friend bool operator==(const JSONArray& lhs, const JSONArray& rhs);
            friend bool operator!=(const JSONArray& lhs, const JSONArray& rhs);

        private:
            template<typename T>
            void PushBack(const T& value, const char *old_base, const char *new_base);

            void DeepCopyFrom(const JSONArray& other, const char *old_base, const char *new_base);
            void DeleteAll(void);

            ValueArray array;
        };

        class JSONValue 
        {
        public:
            ValueType type;

            union
            {
                bool bool_val;
                s32 int_val;
                f64 double_val;
                std::string_view str_val;
                std::string mut_str_val;
                JSONObject *json_val;

                JSONArray json_arr;
            };

            JSONValue() : type(ValueType::NULL_TYPE) {}
            JSONValue(const JSONValue& value);
            JSONValue(const JSONValue& value, const char *old_base, const char *new_base);
            JSONValue& operator=(const JSONValue& other);

            template<typename T>
            JSONValue& operator=(const T& src);

            ~JSONValue();

            JSONValue(const ValueType& type) : type(type) {}

            JSONValue(const ValueType type, std::string_view key) : type(type), str_val(key) {}
            JSONValue(const bool value) : type(ValueType::BOOL), bool_val(value) {}
            JSONValue(const s32 value) : type(ValueType::INT), int_val(value) {}
            JSONValue(const f64 value) : type(ValueType::DOUBLE), double_val(value) {}
            JSONValue(std::string_view value) : type(ValueType::STR), str_val(value) {}
            JSONValue(const char *value) : type(ValueType::STR), str_val(value) {}
            JSONValue(JSONObject* value) : type(ValueType::JSON_OBJECT), json_val(value) {}
            JSONValue(const JSONObject& value);

            JSONValue(const JSONArray& arr) : type(ValueType::ARR), json_arr(arr) {}
            JSONValue(JSONArray&& arr) : type(ValueType::ARR), json_arr(std::move(arr)) {}

            /**
             * @brief overloading cast to bool.
             * @throw bad_cast
             */
            operator bool&();

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
             * @brief overloading cast to mutable std::string reference.
             * If the value is currently a string_view (zero-copy parsed state),
             * copies it into an owned std::string, switches to the mutable string
             * type, and returns a reference. Subsequent accesses return the
             * mutable copy directly.
             * @throw bad_cast if the value is not a string type.
             * @note NOT YET IMPLEMENTED — see TODO.md step 5.
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
             * @brief overloading cast to bool.
             * @throw bad_cast
             */
            operator const bool&() const;

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
             * @brief overloading cast to string_view.
             * @throw bad_cast
             */
            operator std::string_view() const;

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

            JSONValue& At(u64 index);
            const JSONValue& At(u64 index) const;
            JSONValue& At(const std::string_view key);
            const JSONValue& At(const std::string_view key) const;

            JSONValue& operator[](u64 key);

            /**
             * @brief for accessing keys from a nested json
             * @return JSONValue with the nested JSONObject
             */
            JSONValue& operator[](std::string_view key);
            JSONValue& operator[](const char *key);
            
            void PrintValueByType(u8 indent, std::ostream& out) const;
            void AssignValueByType(const JSONValue& src, const char *old_base, const char *new_base);
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
        JSONObject(const JSONObject& other, const char *old_base, const char *new_base);
        JSONObject& operator=(const JSONObject& obj);
        ~JSONObject();
    
        /**
         * @brief retrieves the value associated with the given key
         * @param key the key to look up
         * @return a reference to the associated JSONValue
         * @throws std::out_of_range if the key is not found
         */
        JSONValue& At(const std::string_view key);
        const JSONValue& At(const std::string_view key) const;
    
        /**
         * @brief removes the key-value pair associated with the given key
         * @param key the key to remove
         * @note no-op if the key does not exist
         */
        void Remove(std::string_view key);

        /**
         * @brief parses a JSON file into this object
         * Reads the entire file into source_buffer via seekg/tellg + resize + read,
         * then runs the single-pass parser. All string_views in the resulting tree
         * point into source_buffer.
         * @param json_file an open input file stream positioned anywhere (seeked to
         *        beginning internally). Must be open and in a good state.
         * @return 0 on success, 1 on error (stream not open, tellg failure,
         *         read failure, or parse error).
         */
        int Parse(std::ifstream& json_file);

        /**
         * @brief parses a JSON string into this object
         * Copies the input into source_buffer, then runs the single-pass parser.
         * All string_views in the resulting tree point into source_buffer.
         * @param json_str the JSON text to parse
         * @return 0 on success, 1 on parse error.
         */
        int Parse(const std::string& json_str);
       
        /**
         * @brief access or insert a value in the json object
         * @param key the key to look up or create
         * @return a reference to the existing value, or a newly inserted null-type JSONValue
         */
        JSONValue& operator[](std::string_view key);

        friend bool operator==(const JSONObject& lhs, const JSONObject& rhs);
        friend bool operator!=(const JSONObject& lhs, const JSONObject& rhs);
        friend std::ostream& operator<<(std::ostream& out, const JSONObject& obj);
        friend std::ostream& operator<<(std::ostream& out, const JSONValue& value);

#ifdef NDEBUG 
    private:
#endif /* NDEBUG */
        typedef std::unordered_map<std::string_view, JSONValue*>::iterator JSONIter;

        std::unordered_map<std::string_view, JSONValue*> json;
        std::list<std::string_view> insertion_order;
        std::list<std::string> inserted_keys; // holds keys that were inserted after parsing
        std::string source_buffer;

        void DeepCopyFrom(const JSONObject& other, const char *_old_base, const char *_new_base);
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
    void JSONObject::JSONArray::PushBack(const T& value, const char *old_base, const char *new_base)
    {
        JSONValue *new_val = new JSONValue(value, old_base, new_base);
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
