/* ------------------------------------------*/ 
/* Filename: Buffer.h                        */
/* Date:     19.09.2024                      */
/* Author:   Oron                            */ 
/* ------------------------------------------*/

#ifndef __BUFFER_H__
#define __BUFFER_H__

#include "my_int.h"

class Buffer
{
public:
    char *mem;
    u64 size;

    Buffer() : mem(nullptr), size(0) {}
    Buffer(u64 size);
    Buffer(const Buffer& other);
    Buffer& operator=(const Buffer& other);

    friend bool operator==(const Buffer& lhs, const Buffer& rhs);

    ~Buffer();
};

#endif /* BUFFER_H */