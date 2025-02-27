# 原仓库https://gitee.com/for_ten_years/log
# 在原来的基础上做了一些定制化的修改，
# log

#### 介绍
基于C++ spdlog日志库的二次封装<br>


 **支持以下功能** ：
1. 颜色控制台输出
2. 旋转日志文件输出  （存储多个文件，循环覆盖） 
3. 日期日志文件输出  （采用日期文件记录，在每天的指定时间生成一个日志文件）
4. 同步/异步输出模式
5. 配置文件加载日志库    
6. 多logger输出     （ps: 可以形成多个日志文件，指定logger输出，方便多模块功能的日志记录）
<br><br>


#### 目录介绍
 **hlog** ：日志库源码+二次封装的源码<br>
 **test** ：测试代码（注：需要拷贝静态库的include、lib目录过来，这两个目录由hlog目录生成）<br>

注意：在hlog目录中，里面有用到一个json库，其作用是为了支持使用配置文件方式加载日志。如果不需要使用该方式，可以将其库去除，以及封装源码中的配置文件加载接口去除即可
<br><br>

#### windows下如何生成静态库
```
1、打开cmd.exe界面，进入hlog目录，执行下面命令
winbuild_vs2019.bat

2、命令执行完以后会在编译目录生成 include、lib目录

```


<br><br>

#### linux下如何生成静态库
```
//进入hlog目录，执行下面的命令
mkdir build
cd build
cmake ..
make
make install    //该命令会在编译目录生成 include、lib 目录

```

<br><br>

#### 如何在项目中使用
1、在hlog目录下使用cmake生成静态库<br>
3、拷贝静态库的目录到你的项目中，并链接它<br>
<br><br>

#### 如何使用测试目录进行测试
1、进入hlog生成静态库<br>
2、拷贝静态库目录 include、lib 目录到test目录下<br>
3、执行cmake编译出可执行文件<br>
4、运行可执行文件<br>
<br><br>


#### **类使用实例**

```c++
#pragma once
#include "hlog.h"
 
int main()
{
        //控制台（软件初始化）
	Hlog log;
	log.AddColorConsole("logger1");
	log.Init();
	LOG_DEBUG("logger1", "test data:{}");
	LOG_INFO("logger1", "test data:{}");
	LOG_WARN("logger1", "test data:{}");

	////控制台（配置文件初始化）
	//Hlog log;
	//log.InitConfig(".\\log_rotating_file.json");
	//LOG_DEBUG("logger1", "test data:{}");
	//LOG_INFO("logger1", "test data:{}");
	//LOG_WARN("logger1", "test data:{}");



	////控制台+旋转文件（软件初始化）
	//Hlog log;
	//log.AddColorConsole("logger1");
	//log.AddRotatingFile("logger1", "log/test.log");
	//log.Init();
	//LOG_DEBUG("logger1", "test data:{}");
	//LOG_INFO("logger1", "test data:{}");
	//LOG_WARN("logger1", "test data:{}");

	////控制台+旋转文件（配置文件初始化）
	//Hlog log;
	//log.InitConfig(".\\log_rotating_file.json");
	//LOG_DEBUG("logger1", "test data:{}");
	//LOG_INFO("logger1", "test data:{}");
	//LOG_WARN("logger1", "test data:{}");





	////控制台+日期文件（软件初始化）
	//Hlog log;
	//log.AddColorConsole("logger1");
	//log.AddDailyFile("logger1", "log/test.log", 5, 0);
	//log.Init();
	//LOG_DEBUG("logger1", "test data:{}");
	//LOG_INFO("logger1", "test data:{}");
	//LOG_WARN("logger1", "test data:{}");

	////控制台+日期文件（配置文件初始化）
	//log.InitConfig(".\\log_daily_file.json");
	//LOG_DEBUG("logger1", "test data:{}");
	//LOG_INFO("logger1", "test data:{}");
	//LOG_WARN("logger1", "test data:{}");





	////多个记录器，记录器里：控制台+旋转文件（软件方式初始化）
	//Hlog log;
	//log.AddColorConsole("log1", Hlog::LEVEL_DEBUG);
	//log.AddRotatingFile("log1", "log/test.log", 10240000, 10, Hlog::LEVEL_DEBUG);
	//log.AddColorConsole("log2", Hlog::LEVEL_DEBUG);
	//log.AddRotatingFile("log2", "log/test.log", 10240000, 10, Hlog::LEVEL_DEBUG);
	//log.Init(Hlog::SYNC);
	//LOG_DEBUG("log1", "test data:{}");
	//LOG_INFO("log1", "test data:{}");
	//LOG_WARN("log1", "test data:{}");
	//LOG_DEBUG("log2", "test data:{}");
	//LOG_INFO("log2", "test data:{}");
	//LOG_WARN("log2", "test data:{}");
	
	////多个记录器，记录器里：控制台+旋转文件（配置文件初始化）
	//Hlog log;
	//log.InitConfig(".\\multi_log.json");
	//LOG_DEBUG("log1", "test data:{}");
	//LOG_INFO("log1", "test data:{}");
	//LOG_WARN("log1", "test data:{}");
	//LOG_DEBUG("log2", "test data:{}");
	//LOG_INFO("log2", "test data:{}");
	//LOG_WARN("log2", "test data:{}");
}
```
</br></br>

#### 配置文件内容

```
# 文件描述：
# outputMode: 同步/异步的日志输出模式， 值： SYNC、ASYNC
# outputFormat: 日志输出格式，具体格式参照官方文档
# level：输出等级,值： TRACE、DEBUG、INFO、WARN、ERROR、CRITI
# type: sink类型， 值：color_console、rotating_file
#
# loggers： 存放所有记录器，里面每个对象都是一个记录器，记录器里面存在多个sink
# logger-name: 记录器的名称
# logger-level:记录器的输出等级
#
# sinks： 存放多个sink输出位置
# sink-type: sink的类型，控制台或者文件等


{
	"outputMode" : "SYNC",
        "outputFormat" : "%^[ %Y-%m-%d %H:%M:%S.%e ] <thread %t> [%n] [%l]\n%@,%!\n%v%$\n",
	"loggers" : [
		{
			"name" : "log1",
			"level" : "DEBUG",
			"sinks" : [
				{
					"type" : "color_console",        //控制台
					"level" : "INFO"
				},
				{
					"type" : "rotating_file",        //旋转文件
					"level" : "DEBUG",
					"fileName": "log/test1.log",
					"maxFileSize" : 10240000,
					"maxFile" : 10
				},
				{
					"type" : "daily_file",           //日期文件
					"level" : "DEBUG",
					"fileName": "log/test.log",
					"hour" : 5,
					"minute" : 0
				}
			]
		},
		{
			... 同上相同，这里是另外一个logger记录器
		}
	]
}
```


