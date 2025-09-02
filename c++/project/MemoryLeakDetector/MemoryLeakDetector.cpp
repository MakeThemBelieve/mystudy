#include "MemoryLeakDetector.h"
#include <direct.h> 
#include <chrono>
#include <format>

std::list<void*> memoryList;
std::unordered_map<void*, autoMap> newAddress;
std::unordered_map<int, void*>leakAddress;

//创建目录
bool createDirectory(const std::string& path) {

    #ifdef _WIN32
        int result = _mkdir(path.c_str());
    
    // 如果目录已存在或其他错误，检查错误类型
    if (result != 0 && errno != EEXIST) {
        std::cerr << "创建目录失败: " << path << std::endl;
        return false;
    }
    #else
        std::string command = "mkdir -p " + path + " 2>/dev/null";
    #endif
    
    return true;
}

bool createTimeFile(char *buff, size_t buffSize, const std::string& str) {
    
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm local_tm;

    #ifdef _WIN32
        localtime_s(&local_tm, &now_c);
    #else
        local_tm = *std::localtime(&now_c);
    #endif

    //使用时间作为文件名
    if (std::strftime(buff, buffSize, str.c_str(), &local_tm) == 0) {
        std::cerr << "时间格式转换失败" << std::endl;
        return false;
    }
    return true;
}

//
void *operator new(size_t size, const char* file, int line){
    
    void* ptr = malloc(size);
    newAddress[ptr] = {file, line, size};
    memoryList.push_back(ptr);

    #if CONTROL_SWITCH
    //输出文件
    MemoryLeakDetector::getInstance().fileStart(ptr);
    #else 
    //仅输出日志
    MemoryLeakDetector::getInstance().reportStart(ptr);
    #endif

    return ptr;
}

//
void operator delete(void* ptr, size_t size) noexcept {

    if (ptr) {

        // 从内存跟踪结构中移除
        auto it = newAddress.find(ptr);
        if (it != newAddress.end()) {
            newAddress.erase(it);
        }
        
        auto listIt = std::find(memoryList.begin(), memoryList.end(), ptr);
        if (listIt != memoryList.end()) {
            memoryList.erase(listIt);
        }

    #if CONTROL_SWITCH
    //输出文件
        MemoryLeakDetector::getInstance().fileEnd(ptr);
    #else 
    //仅输出日志
        MemoryLeakDetector::getInstance().reportEnd(ptr);
    #endif

        std::free(ptr);
    } else {

        printf(" double delete");
    }
}

//new文件输出
void MemoryLeakDetector::fileStart(void* ptr) {

    if (!createDirectory("./mem")) {

        std::cout << "将尝试直接写入当前目录" << std::endl;
    }

    char buff[128] = {0};

    //使用指针值作为文件名
    #ifdef _WIN32

        std::stringstream ss;
        ss << std::hex << ptr;
        std::string ptr_str = ss.str();
        std::replace(ptr_str.begin(), ptr_str.end(), ':', '_');
        snprintf(buff, sizeof(buff), "./mem/%s.mem", ptr_str.c_str());
    #else
    #endif

    //打开文件写入
    std::ofstream file(buff);
    if (file.is_open()) {
        auto it = newAddress.find(ptr);
        if (it != newAddress.end()) {
            file << "[操作:new  " << ptr << " ]" << std::endl
                << " 位置:" << it->second.file << std::endl
                << " 行号:" << it->second.line << std::endl
                << " 大小:" << it->second.size << std::endl << std::endl;
        }
        file.close();
    }
}

//del文件删除
void MemoryLeakDetector::fileEnd(void* ptr) {

    char buff[128] = {0};

    //删除文件
    #ifdef _WIN32

        // 生成文件名
        std::stringstream ss;
        ss << std::hex << ptr;
        std::string ptr_str = ss.str();
        std::replace(ptr_str.begin(), ptr_str.end(), ':', '_');
        snprintf(buff, sizeof(buff), "./mem/%s.mem", ptr_str.c_str());
        
        // 删除Windows平台的文件
        if (remove(buff) != 0) {
        // 如果mem目录中找不到文件，尝试当前目录
        snprintf(buff, sizeof(buff), "%s.mem", ptr_str.c_str());
        remove(buff);
    }
    #else
    #endif
}

//new报告写入
void MemoryLeakDetector::reportStart(void* ptr) {

    char buff[128] = {0};
    if (!createTimeFile(buff, "./memleakHitstory_%Y%m%d_%H%M%S.rep")) {

        std::cout << "将尝试直接写入当前文件" << std::endl;
    }
    //打开文件写入
    std::ofstream file(buff);
    if (file.is_open()) {
        auto it = newAddress.find(ptr);
        if (it != newAddress.end()) {
            file << "[操作:new  " << ptr << " ]" << std::endl
                << " 位置:" << it->second.file << std::endl
                << " 行号:" << it->second.line << std::endl
                << " 大小:" << it->second.size << std::endl << std::endl;
        }
        file.close();
    }
}

//del报告写入
void MemoryLeakDetector::reportEnd(void *ptr) {

    char buff[128] = {0};
    if (!createTimeFile(buff, "./memleakHitstory_%Y%m%d_%H%M%S.rep")) {

        std::cout << "将尝试直接写入当前文件" << std::endl;
    }

    //打开文件写入
    std::ofstream file(buff);
    if (file.is_open()) {
        auto it = newAddress.find(ptr);
        if (it != newAddress.end()) {
            file << "[操作:delete  " << ptr << " ]" << std::endl
                << " new位置:" << it->second.file << std::endl
                << " new行号:" << it->second.line << std::endl
                << " mem大小:" << it->second.size << std::endl << std::endl;
        }
        file.close();
    }
}

//泄漏报告输出
int MemoryLeakDetector::reportLeak() {

    char buff[128] = {0};

    createTimeFile(buff, "./memleakReport_%Y%m%d_%H%M%S.rep");

    //打开文件写入
    std::ofstream file(buff);
    int leakCount = 0;
    if (file.is_open()) {
        
        for (auto i = memoryList.begin(); i != memoryList.end(); ++i) {

            auto it = newAddress.find(*i);
            if (it != newAddress.end()) {
                file << "[操作:new  " << *i << " ]" << std::endl
                    << " 位置:" << it->second.file << std::endl
                    << " 行号:" << it->second.line << std::endl
                    << " 大小:" << it->second.size << std::endl << std::endl;
                leakCount++;
            }
        }
        if (!leakCount) std::cout << "已生成内存泄漏报告" << std::endl;
        file.close();
    }
    return leakCount;
}
