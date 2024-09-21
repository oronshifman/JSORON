# JSORON

**JSORON** is a study project as part of the **"Performance-Aware Programming"** course by Casey Muratori. The repository is structured into two directories: one for `JSONObject` and another for `JSONParser`.

- The **JSONObject** directory contains code for handling JSON objects and their key-value pairs.
- The **JSONParser** directory includes the parser logic that converts JSON strings into `JSONObject` instances.

## Features

- Parse JSON strings into structured `JSONObject` objects.
- Being a study project, this parser is not fully compatible with the JSON spec.
- Minimal dependencies, written entirely in C++.
  
## Table of Contents

- [Installation](#installation)
- [Directory Structure](#directory-structure)
- [Usage](#usage)
  - [JSONObject](#jsonobject)
  - [JSONParser](#jsonparser)

## Installation

To install and use the JSORON library, first clone the repository:

```bash
git clone https://github.com/yourusername/JSORON.git
```

Navigate to the project directory:

```bash
cd JSORON
```

## Directory Structure

- **JSONObject/**: Contains the code for representing JSON objects and manipulating key-value pairs.
- **JSONParser/**: Contains the code that parses raw JSON strings into structured `JSONObject` objects.
- **tests/**: Unit tests for both `JSONObject` and `JSONParser`.

## Usage

### JSONObject

`JSONObject` represents a JSON object with key-value pairs. It allows access and modification of the data.

Example usage:

```cpp
using nammespace JSORON;
JSONObject obj;
obj.Put("name", "JSORON");
obj.Put("version", 1.0);
std::string name = obj["name"];
```

### JSONParser

`JSONParser` is responsible for parsing a JSON string and returning a `JSONObject`.

Example usage:

```cpp
using nammespace JSORON;
JSONParser parser;
JSONObject obj = parser.Parse("{\"name\": \"JSORON\", \"version\": 1.0}");
std::string name = obj["name"];
```

---
