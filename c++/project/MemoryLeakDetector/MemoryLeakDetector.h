#ifndef MEMORY_LEAK_DETECTOR_H
#define MEMORY_LEAK_DETECTOR_H

#include <cstdlib>
#include <iostream>
#include <windows.h>
#include <unordered_map>
#include <sstream>
#include <algorithm>
#include <fstream>
#include <list>


#define CONTROL_SWITCH 0

struct autoMap{
    const char* file;
    int line;
    size_t size;
};

extern std::list<void*> memoryList;
extern std::unordered_map<void*, autoMap> newAddress;
extern std::unordered_map<int, void*>leakAddress;

bool createDirectory(const std::string& path);
bool createTimeFile(char *buff, size_t buffSize, const std::string& str);

void* operator new(size_t size, const char* file, int line);
void operator delete(void* ptr, size_t size) noexcept;


class MemoryLeakDetector{
    private:

        MemoryLeakDetector() {}
    public:

        MemoryLeakDetector(const MemoryLeakDetector&) = delete;
        MemoryLeakDetector& operator=(const MemoryLeakDetector&) = delete;

        static MemoryLeakDetector& getInstance() {
            static MemoryLeakDetector instance;
            return instance;
        }

    void fileStart(void *ptr);
    void fileEnd(void *ptr);
    void reportStart(void *ptr);
    void reportEnd(void *ptr);
    int reportLeak();
};



#endif