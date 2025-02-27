mkdir build
cd build
cmake -G "Visual Studio 16 2019" -A Win32 ..
cmake --build . --config Release

mkdir include lib
copy ..\hlog.h .\include
xcopy ..\json\include .\include /s
xcopy ..\spdlog-1.x\include .\include /s
copy .\Release\hlog_st.lib .\lib