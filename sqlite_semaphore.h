#ifndef __SQLITE_SEMAPHORE_H__
#define __SQLITE_SEMAPHORE_H__

#include <stdint.h>

int32_t SQL_semGet(int32_t semKey, int32_t index);
int SQL_semP(uint32_t semKey, int32_t index);
int SQL_semV(uint32_t semKey, int32_t index);

#endif
