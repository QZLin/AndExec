cmake_minimum_required(VERSION 3.10)

project(andexec LANGUAGES C)

function(add_andexec_variant target)
    add_executable(${target} WIN32 and_exec.c app.rc)
    # 全部使用宽字符 WinAPI,与 wWinMain / CreateProcessW 一致
    target_compile_definitions(${target} PRIVATE UNICODE _UNICODE ${ARGN})
    if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
        target_link_options(${target} PRIVATE -municode)  # MinGW 使用 wWinMain 入口
    endif()
    set_target_properties(${target} PROPERTIES OUTPUT_NAME ${target})
endfunction()

add_andexec_variant(andexec)                                         # 无define
add_andexec_variant(andexec_d FLAG_ENABLE_WORKDIR)                   # 指定工作目录
add_andexec_variant(andexec_w FLAG_ENABLE_WAIT)                      # 等待子进程退出
add_andexec_variant(andexec_wd FLAG_ENABLE_WORKDIR FLAG_ENABLE_WAIT) # 指定工作目录+等待子进程退出