#include "log_wrapper.h"

int main() {
  //     //控制台（软件初始化）
  //   LogWrapper log;
  //   log.AddColorConsole("logger1");
  //   log.Init();
  //   LOG_DEBUG("logger1", "test data:{}");
  //   LOG_INFO("logger1", "test data:{}");
  // LOG_WARN("logger1", "test data:{}");

  ////控制台（配置文件初始化）
  //   LogWrapper log;
  //   log.InitConfig("/data/.health_ctrl_ws/hlog/test/log_config.json");
  //   LOG_DEBUG("test data:{}");
  //   LOG_INFO("test data:{}");
  //   LOG_WARN("test data:{}");

  // 控制台+旋转文件（软件初始化）
  LogWrapper log;
  //   log.AddColorConsole("logger1");
  log.AddRotatingFile("healthCtrl", "log");
  long long time = 0;
  log.Init();
  while (1) {
    time++;
    LOG_DEBUG("test data:{}", time);
    LOG_INFO("test data:{}", time);
    LOG_WARN("test data:{}", time);
    usleep(100);
  }
  //   LOG_DEBUG("logger1", "test data:{}");
  //   LOG_INFO("logger1", "test data:{}");
  //   LOG_WARN("logger1", "test data:{}");

  // cmake如何自动生成编译库的头文件

  return 0;
}