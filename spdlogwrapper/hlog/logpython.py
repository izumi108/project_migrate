import sys
# 生成的动态库路径
sys.path.append('hlog/build')
import log_wrapper
import inspect

logger = log_wrapper.LogWrapper()


logger.AddColorConsole()
logger.AddRotatingFile("hardBridge", "log/hardBridge")

logger.Init()
cnt=1
logger.log_debug("This is a debug message")
logger.log_info("This is a info message")
logger.log_debug("test: {} {} {}", inspect.currentframe().f_code.co_filename,inspect.currentframe().f_lineno,cnt) 