#include "stdafx.h"

#include "ThreadData.h"

// 全局gambit线程数据
GambitThreadData global_gambit_thread_data;

// 全局fluent线程数据
FluentThreadData global_fluent_thread_data;

// 全局变量--用户是否要求线程结束
bool global_user_ask_for_thread_end;

// 全局变量--用于表示计算阶段
int ffc_step;

// 全局日志消息数据
LogMsg global_log_msg;

// 全局ffc线程数据
FFCThreadData global_ffc_thread_data;

bool IsUserAskForThreadEnd()
{
    return global_user_ask_for_thread_end;
}

void UserAskForThreadEnd( bool flag )
{
    global_user_ask_for_thread_end = flag;
}