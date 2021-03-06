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
    // 计算方向系数
    initDirection();

    // 判断回路能否进行迭代???
    bCanDoIterate = canDoIterate();
}

void IndependentCycle::initDirection()
{
    if( path.empty() ) return;

    Digraph::Node u = dg.target( baseEdge );
    short d = 1;  // 以基准分支方向为正方向

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
    // 固定风量大于0
    return ed[getBaseEdge()]->fq > 0;
}

bool IndependentCycle::hasExtraF()
{
    // 检查回路中是否存在附加阻力(风机)
    bool hasFan = false;

    int len = path.size();
    for( int i = 0; i < len; i++ )
    {
        Digraph::Arc e = path[i];

        // 分支上有附加阻力(风机)
        if( fd[e] != 0 )
        {
            hasFan = true;
        }
        //else
        //{
        //	// 其它的情况，例如自然风压，火风压等等
        //	// ???可能不太正确
        //	// ...
        //}
        if( hasFan ) break;
    }

    return hasFan;
}

bool IndependentCycle::canDoIterate()
{
    // 1) 基准分支是固定风量分支，不参与迭代计算
    if( isFixQCycle() ) return false;

    // 2) 如果基准分支是大气分支, 且不包含附加阻力, 则不参与迭代计算
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
    // 一定要记住：基准分支也要参与计算
    // f1()方法的返回值始终>=0
    double s = f1( baseEdge );

    // 回路中的其它分支计算阻力1阶导数的绝对值之和
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
    // 风机动力方向与方向系数c有关
    // sq - c*hf
    // sq表示回路分支阻力代数和，hf表示风机动力(根据特性曲线计算出来的)
    // 而风机分支始终是余枝，也即c=1
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

    // 回路不参与迭代计算
    if( !bCanDoIterate )
    {
        return;
    }

    // 返回==>回路的风量修正量
    dq = deltaQ();

    // 修正基准分支风量
    increaseQ( getBaseEdge(), dq );

    // 回路中的其它分支修正风量
    int n = path.size();
    for( int i = 0; i < n; i++ )
    {
        increaseQ( path[i], c[i]*dq ); // 修正回路分支风量
    }

    // 返回==>回路阻力代数和
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
