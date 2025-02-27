#include "spdlog.h"
#include <unistd.h>

int main()
{
    // print log test, you can transfer any param to do format
    int param = 1;
    unix_ai::Logger::getInstance("custom_logger", "./custom_log_dir", 3);
    uint16_t data[] = {3, 0x1122};
    while (1)
    {
        // hex print
        LOG_INFO("Hex data: 0x{:04x} 0x{:04x}", data[0], data[1]);
        // int print
        LOG_INFO("Int data: {:d} {:d}", data[0], data[1]);

        LOG_TRACE("this is trace log record, param: {}", ++param); // int type param is ok
        LOG_DEBUG("this is debug log record, param: {}", ++param);
        LOG_INFO("this is info log record, param: {}", ++param);
        LOG_WARN("this is warn log record, param: {}", double(++param));           // double type param is ok
        LOG_ERROR("this is error log record, param: {}", std::to_string(++param)); // string type param is ok
        usleep(100000);                                                            // 0.1 seconds
    }

    return 0;
}
