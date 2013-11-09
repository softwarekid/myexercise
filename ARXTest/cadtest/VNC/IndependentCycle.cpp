#include "IndependentCycle.h"
#include <algorithm>
#include <iterator>

IndependentCycle::IndependentCycle( Digraph& _dg, VNC_EdgeDataMap& _eds, VNC_FanDataMap& _fds, Digraph::Arc _airEdge, Digraph::Arc _baseEdge )
    : dg( _dg ), ed( _eds ), fd( _fds ),
      airEdge( _airEdge ), baseEdge( _baseEdge )
{
}

Digraph::Arc IndependentCycle::getBaseEdge() const
{
    return baseEdge;
}

void IndependentCycle::addEdge( Digraph::Arc e )
{
    if( e == INVALID || e == baseEdge ) return;
    if( std::find( path.begin(), path.end(), e ) != path.end() ) return;

    path.push_back( e );
}

void IndependentCycle::getPath( EdgeArray& p )
{
    std::copy( path.begin(), path.end(), std::back_inserter( p ) );
}

void IndependentCycle::init()
{
    // ���㷽��ϵ��
    initDirection();

    // �жϻ�·�ܷ���е���???
    bCanDoIterate = canDoIterate();
}

void IndependentCycle::initDirection()
{
    if( path.empty() ) return;

    Digraph::Node u = dg.target( baseEdge );
    short d = 1;  // �Ի�׼��֧����Ϊ������

    int len = path.size();
    for( int i = 0; i < len; i++ )
    {
        Digraph::Arc e = path[i];
        Digraph::Node s = dg.source( e );
        Digraph::Node t = dg.target( e );
        if( d > 0 )
        {
            if( s != u )
            {
                d = -1 * d;
                u = s;
            }
            else
            {
                u = t;
            }
        }
        else
        {
            if( t != u )
            {
                d = -1 * d;
                u = t;
            }
            else
            {
                u = s;
            }
        }
        c.push_back( d );
    }
}

bool IndependentCycle::isFixQCycle()
{
    // �̶���������0
    return ed[getBaseEdge()]->fq > 0;
}

bool IndependentCycle::hasExtraF()
{
    // ����·���Ƿ���ڸ�������(���)
    bool hasFan = false;

    int len = path.size();
    for( int i = 0; i < len; i++ )
    {
        Digraph::Arc e = path[i];

        // ��֧���и�������(���)
        if( fd[e] != 0 )
        {
            hasFan = true;
        }
        //else
        //{
        //	// �����������������Ȼ��ѹ�����ѹ�ȵ�
        //	// ???���ܲ�̫��ȷ
        //	// ...
        //}
        if( hasFan ) break;
    }

    return hasFan;
}

bool IndependentCycle::canDoIterate()
{
    // 1) ��׼��֧�ǹ̶�������֧���������������
    if( isFixQCycle() ) return false;

    // 2) �����׼��֧�Ǵ�����֧, �Ҳ�������������, �򲻲����������
    if( ( airEdge == getBaseEdge() ) && !hasExtraF() ) return false;

    return true;
}

double IndependentCycle::h0() const
{
    double s = h0( baseEdge );

    int len = path.size();
    for( int i = 0; i < len; i++ )
    {
        Digraph::Arc e = path[i];
        s += h0( e ) * c[i];
    }

    return s;
}

double IndependentCycle::h1() const
{
    double s = h1( baseEdge );;

    int len = path.size();
    for( int i = 0; i < len; i++ )
    {
        Digraph::Arc e = path[i];
        s += h1( e ) * c[i];
    }

    return s;
}

double IndependentCycle::f0() const
{
    double s = f0( baseEdge );

    int len = path.size();
    for( int i = 0; i < len; i++ )
    {
        s += c[i] * f0( path[i] );
    }
    return s;
}

double IndependentCycle::f1() const
{
    // һ��Ҫ��ס����׼��֧ҲҪ�������
    // f1()�����ķ���ֵʼ��>=0
    double s = f1( baseEdge );

    // ��·�е�������֧��������1�׵����ľ���ֵ֮��
    int len = path.size();
    for( int i = 0; i < len; i++ )
    {
        s += abs( c[i] ) * f1( path[i] );
    }
    return s;
}

double IndependentCycle::f0( Digraph::Arc e ) const
{
    return ed[e]->f0();
}

double IndependentCycle::f1( Digraph::Arc e ) const
{
    return ed[e]->f1();
}

double IndependentCycle::h0( Digraph::Arc e ) const
{
    if( fd[e] == 0 ) return 0;
    return fd[e]->h0( ed[e]->q );
}

double IndependentCycle::h1( Digraph::Arc e ) const
{
    if( fd[e] == 0 ) return 0;
    return fd[e]->h1( ed[e]->q );
}

double IndependentCycle::deltaQ() const
{
    // ������������뷽��ϵ��c�й�
    // sq - c*hf
    // sq��ʾ��·��֧���������ͣ�hf��ʾ�������(�����������߼��������)
    // �������֧ʼ������֦��Ҳ��c=1
    double F0 = f0();
    double H0 = h0();
    double F1 = f1();
    double H1 = h1();

    return -1.0 * ( F0 - H0 ) / ( F1 - H1 );
}

void IndependentCycle::increaseQ( Digraph::Arc e, double dq )
{
    ed[e]->q += dq;
}

void IndependentCycle::iterate( double& dq, double& df )
{
    dq = 0;
    df = 0;

    // ��·�������������
    if( !bCanDoIterate )
    {
        return;
    }

    // ����==>��·�ķ���������
    dq = deltaQ();

    // ������׼��֧����
    increaseQ( getBaseEdge(), dq );

    // ��·�е�������֧��������
    int n = path.size();
    for( int i = 0; i < n; i++ )
    {
        increaseQ( path[i], c[i]*dq ); // ������·��֧����
    }

    // ����==>��·����������
    df = f0() - h0();
}

short IndependentCycle::getCycleCoeff( Digraph::Arc e )
{
    if( e == INVALID ) return 0;
    if( e == baseEdge ) return 1;

    EdgeArray::iterator itr = std::find( path.begin(), path.end(), e );
    if( itr == path.end() ) return 0;

    return c[itr - path.begin()];
}