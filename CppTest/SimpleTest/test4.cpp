#include <iostream>
using namespace std;

enum ShaftFunctionProperty
{
    SFP_REPAIR                 = 0x0001,     // �Ƿ������Ӽ��������
    SFP_LIFT_STAFF             = 0x0002,     // �Ƿ�������Ա
    SFP_LIFT_MATERIAL          = 0x0004,     // �Ƿ���������
    SFP_SKIP_HOIST             = 0X0008,     // ��������
    SFP_BELT_CONVEY            = 0X0010,     // ��ʽ����
    SFP_CLOSE_MEASURES         = 0X0020,     // ���Ƶķ�մ�ʩ
    SFP_DUST_PROOF             = 0X0040,     // �ɿ�������ʩ
    SFP_METHANE_INTERRUPT      = 0X0080,     // װ�����ϵ���
    SFP_FIRE_ALARM             = 0X0100,     // װ���Զ��������װ��
    SFP_FIRE_FIGHTING_PIPELINE = 0X0200,     // װ�����������·
};

int main()
{
    int sfp = ( SFP_REPAIR | SFP_LIFT_STAFF );
    int ret = ( sfp & SFP_REPAIR );
    cout << ret << endl;

    sfp = 0;
    cout << sfp << endl;
    return 0;
}