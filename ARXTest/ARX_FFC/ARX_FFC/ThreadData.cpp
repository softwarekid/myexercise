#include "stdafx.h"

#include "ThreadData.h"

// ȫ��gambit�߳�����
GambitThreadData global_gambit_thread_data;

// ȫ��fluent�߳�����
FluentThreadData global_fluent_thread_data;

// ȫ�ֱ���--�û��Ƿ�Ҫ���߳̽���
bool global_user_ask_for_thread_end;

// ȫ�ֱ���--���ڱ�ʾ����׶�
int ffc_step;

// ȫ����־��Ϣ����
LogMsg global_log_msg;

// ȫ��ffc�߳�����
FFCThreadData global_ffc_thread_data;

bool IsUserAskForThreadEnd()
{
    return global_user_ask_for_thread_end;
}

void UserAskForThreadEnd( bool flag )
{
    global_user_ask_for_thread_end = flag;
}