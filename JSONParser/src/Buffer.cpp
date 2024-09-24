/* ------------------------------------------*/ 
/* Filename: Buffer.cpp                      */
/* Date:     19.09.2024                      */
/* Author:   Oron                            */ 
/* ------------------------------------------*/

#include <string.h>
#include <stdlib.h>

#include "Buffer.h"

Buffer::Buffer(u64 size)
{
    mem = (char *)malloc(size);

    if (!mem)
    {
        mem = nullptr;
        size = 0;

        return;
    }

    this->size = size;
}

Buffer::Buffer(const Buffer& other)
{
    if (this == &other)
    {
        return;
    }

    this->~Buffer();
    mem = other.mem;
    size = other.size;
}

Buffer& Buffer::operator=(const Buffer& other)
{
    if (this == &other)
    {
        return *this;
    }

    this->~Buffer();
    mem = other.mem;
    size = other.size;

    return *this;
}

Buffer::~Buffer()
{
    free(mem);
    size = 0;
}

bool operator==(const Buffer& lhs, const Buffer& rhs)
{
    if (&lhs == &rhs)
    {
        return 1;
    }

    if (lhs.size != rhs.size)
    {
        return 0;
    }

    return strncmp(lhs.mem, rhs.mem, lhs.size) == 0 ? 1 : 0;
}