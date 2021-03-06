//
// File name: Triangle4.cpp
// 用途: 追踪散点数据和断层数据并形成三角形集合
//		和其他三个文件triangle.cpp, triangle2.cpp, triangle3.cpp
//		一同隶属于CTriangle类。
// 包括的函数：
//
// 版权所有（2001年）， 由GriSoft保留所有版权
//

#include "stdafx.h"
#include "Triangle.h"
#include "somefuncs.h"

#include <cfloat>
#include <climits>

//追踪散点数据和断层数据并形成三角形集合
//任何三角形的三个点至多只有一个点在断层数据点上。
//以点theDot为起点追踪一个三角形集合， 访问过的点保存在usedDots中
void CTriangle::CreateTriSet( CDC* dc, SDot theDot, CArray<SDot, SDot>& usedDots )
{
    double dd, dm, alpha;
    double xc( 0 ), yc( 0 );
    int L, i, j, kc, k, km;	//Add by Robter
    CPoint Points[4];
    STriangle theTriangle;

    L = 1;

    //在所有点中查找和起始点(x[0],y[0])距离最近的点,并在ib2[1]中记载这个点在x[]中的下标号
    //...设定第一个三角形的第一个点为索引号位0的散点
    theTriangle.ib1 = theDot;

    //...开始查找第一个三角形的第二个点，从索引为0的散点开始查找...
    //	 但第二个点也可以为断层点，所以查找完散点之后，继续在断层点中查找...
    //	 直到找到与三角形的第一点距离最近的点为止。
    //......首先在散点数据点中查找...
    theDot.enumDotSource = DOTSRC_SCATTER;
    theDot.index = 0;
    UINT iOnFault;

    SDot tmpDot;
    tmpDot.enumDotSource = DOTSRC_SCATTER;
    bool bFirstLoop( true );
    for ( i = 0; i < kp; i++ )
    {
        if ( i == theTriangle.ib1.index ) continue;
        tmpDot.index = i;
        if ( IsLineCrossFaults( theTriangle.ib1, tmpDot, iOnFault ) == true )
            continue;
        dm = sqrt( ( x[i] - x[theTriangle.ib1.index] ) * ( x[i] - x[theTriangle.ib1.index] ) + ( y[i] - y[theTriangle.ib1.index] ) * ( y[i] - y[theTriangle.ib1.index] ) );
        if ( bFirstLoop == true )
        {
            bFirstLoop = false;
            theDot = tmpDot;
            dd = dm;
        }
        else if( dm < dd && dm > epsilon )
        {
            theDot = tmpDot;
            dd = dm;
        }
    }

    theTriangle.ib2 = theDot; //...OK， 找到了第一个三角形的第二个点
    //d12 = dd; //...d12保存了第一个三角形的第一个点和第二个点之间的距离

    //
    //查找第一个三角形的第三个点，该点可能是散点，也可能为断层点。
    //...首先在散点中查找；
    kc = -1;//kc为第一次进入循环的标志,如果在下面的for循环中找到了合适条件的第三点，
    //	则kc的值为1；
    dd = DBL_MAX;
    tmpDot.enumDotSource = DOTSRC_SCATTER;
    for ( i = 0; i < kp; i++ )
    {
        //判断点(x[i],[i])与点三角形的已知俩个点是否在同侧。
        //如果不再同一侧，继续下一个点的追踪。
        tmpDot.index = i;
        if ( IsValidNewTri( kc, alpha, dd, theTriangle.ib1, theTriangle.ib2, theTriangle.ib1, tmpDot, theDot, usedDots ) == true )
        {
            /*CString msg;
            msg.Format("%d", i);
            AfxMessageBox(msg);*/
            theDot = tmpDot;
        }
    }
    //dd = DBL_MAX;
    //...然后再从断层中查找
    CFault* tmpFault;
    tmpDot.enumDotSource = DOTSRC_FAULT;
    for( i = 0; i < m_faults.GetSize(); i++ )	//断层条数
    {
        tmpFault = ( CFault* )m_faults[i];
        tmpDot.iFaultID = i;
        for( j = 0 ; j < tmpFault->iDots; j++ )
        {
            tmpDot.index = j;
            if ( IsValidNewTri( kc, alpha, dd, theTriangle.ib1, theTriangle.ib2, theTriangle.ib1, tmpDot, theDot, usedDots ) == true )
            {
                theDot = tmpDot;
            }
        }
    }

    //对第一个三角形来说，已经找到了合适的第三点,所以在三角形集合中增加一个三角形。
    if( kc != -1 )
    {
        theTriangle.ib3 = theDot;
        m_trisets.Add( new CTriSet ); //增加第一个三角形集合
        //为这个新的三角形集合加入一个三角形；
        ( ( CTriSet* )m_trisets[m_trisets.GetUpperBound()] )->theTriSet.Add( theTriangle );
        usedDots.Add( theTriangle.ib1 ); //保存访问过的点
        usedDots.Add( theTriangle.ib2 );
        usedDots.Add( theTriangle.ib3 );
    }
    else
    {
        AfxMessageBox( "没有发现第一个三角形的第三个点！" );
        return;
    }

    // OK, 第一个三角形已经追踪完毕！！！
    ////////////////////////////////////////
    if ( kc != -1 ) Draw( 0, L - 1, dc );


    //=========主循环==========
    // 开始追踪其他三角形。

    SDot ib1k, ib2k, ib3k, searchedDot;
    k = 1;
    km = 500;
    int iPos;
    while( 1 )
    {
        if ( L < k && k != 1 )
        {
            CString msg;
            msg.Format( "iPos=%d, L=%d, k=%d", iPos, L, k );
            AfxMessageBox( msg );
            break; //L为三角形的总数，k为正在扩展处理的三角形的序号。
        }
        iPos = m_trisets.GetUpperBound();
        ib1k = ( ( ( CTriSet* )m_trisets[iPos] )->theTriSet ).GetAt( k - 1 ).ib1;
        ib2k = ( ( ( CTriSet* )m_trisets[iPos] )->theTriSet ).GetAt( k - 1 ).ib2;
        ib3k = ( ( ( CTriSet* )m_trisets[iPos] )->theTriSet ).GetAt( k - 1 ).ib3;

        //	first side for k_th triangle
//			ASSERT(k<25);
        // 除第1个三角形外，其它的三角形只处理2条边
        if ( k == 1 && SearchExtTri( ib1k, ib2k, ib3k, searchedDot, usedDots ) == true )
        {
            L++;
            theTriangle.ib1 = ib1k;
            theTriangle.ib2 = ib2k;
            theTriangle.ib3 = searchedDot;
            ( ( CTriSet* )m_trisets[iPos] )->theTriSet.Add( theTriangle );
            Draw( iPos, L - 1, dc );
        }

        // second side for k_th triangle
        if ( SearchExtTri( ib2k, ib3k, ib1k, searchedDot, usedDots ) == true )
        {
            L++;
            theTriangle.ib1 = ib2k;
            theTriangle.ib2 = ib3k;
            theTriangle.ib3 = searchedDot;
            ( ( CTriSet* )m_trisets[iPos] )->theTriSet.Add( theTriangle );
            Draw( iPos, L - 1, dc );
        }

        // four triangle or third side for the k-th angle
        if( SearchExtTri( ib3k, ib1k, ib2k, searchedDot, usedDots ) == true )
        {
            L++;
            theTriangle.ib1 = ib3k;
            theTriangle.ib2 = ib1k;
            theTriangle.ib3 = searchedDot;
            ( ( CTriSet* )m_trisets[iPos] )->theTriSet.Add( theTriangle );
            Draw( iPos, L - 1, dc );
        }

        // end first loop
        k = k + 1;
        //if (k>10) AfxMessageBox("HEllo");
    }
//	ibcount=L;//把三角形的个数L赋给类的成员变量
    ( ( CTriSet* )m_trisets[m_trisets.GetUpperBound()] )->ibcount = L; //把三角形的个数L赋给类的成员变量
}

//////////////////////////////////////////////////////////
//
// 功能：判断二个点是否在断层的同一侧， 或者说线段与断层的关系，
//		判断的原理是两点的连线的一部分不与任何断层线的一部分相交。
//		ib1和ib2为SDot结构体，描述了点在断层或散点中索引
//		如果相交返回True, 否则返回false;
// 输入: 接口参数：需要判断的二个点，
//		 成员变量：断层数据m_faults
//		iLinkFault的位返回了线段与线段交叉的具体情况；
// 输出：判断结果，false/true
// 调用：调用了成员函数CSomeFuncs.Relation_Of_Lines()
//
//////////////////////////////////////////////////////////
bool CTriangle::IsLineCrossFaults( const SDot ib1, const SDot ib2, UINT& iLinkFault )
{
    double x1, y1, x2, y2;
    SDot2XY( ib1, x1, y1 );
    SDot2XY( ib2, x2, y2 );
    return IsLineCrossFaults( x1, y1, x2, y2, iLinkFault );
}

bool CTriangle::IsLineCrossFaults( const double x1, const double y1, const double x2, const double y2, UINT& iLinkFault )
{
    int faultscount = m_faults.GetSize();
    int iDotsPerFault; //第i条断层的点数；
    int i, j;
    int status1( 0 );
    double x3, x4, xc( 0 ), y3, y4, yc( 0 );
    CSomeFuncs somefuncs;

    iLinkFault = 0;

    for ( i = 0; i < faultscount; i++ ) //依次处理每一条断层
    {
        //第i条断层
        iDotsPerFault = ( ( CFault* )m_faults[i] )->x.GetSize();
        for( j = 1; j <= iDotsPerFault; j++ )
        {
            x3 = ( ( CFault* )m_faults[i] )->x[j - 1];
            y3 = ( ( CFault* )m_faults[i] )->y[j - 1];
            if ( j == iDotsPerFault )
            {
                x4 = ( ( CFault* )m_faults[i] )->x[0];
                y4 = ( ( CFault* )m_faults[i] )->y[0];
            }
            else
            {
                x4 = ( ( CFault* )m_faults[i] )->x[j];
                y4 = ( ( CFault* )m_faults[i] )->y[j];
            }
            iLinkFault = somefuncs.Relation_Of_Lines( x1, y1, x2, y2, x3, y3, x4, y4, iLinkFault );
            if ( ( iLinkFault & ROL_NOPARA_INTERCROSS ) == ROL_NOPARA_INTERCROSS )
                return true;
        }
    }
    return false;

}

////////////////////////////////////////////////////
//	判断线段是否与已有的任何其他三角形相交。
//	如果相交，返回true, 否则，返回false
//Rem inter_cross ?
//Rem 1_2 for L-th triangle
bool CTriangle::Function01_Ex( const SDot dot1, const SDot dot2 )

{
    static double x1, y1, x2, y2, x3, y3, x4, y4, xc, yc;
    static SDot ib1i, ib2i, ib3i;
    int i, status( 0 );

    SDot2XY( dot1, x1, y1 );
    SDot2XY( dot2, x2, y2 );

    int num = m_trisets.GetUpperBound();
    if ( num == -1 ) return false;	//如果没有三角形集合，则可定不相交。

    CTriSet* tmpTriSet;
    STriangle theTri;
    tmpTriSet = ( CTriSet* )m_trisets[num];
    for ( i = 0; i <= tmpTriSet->theTriSet.GetUpperBound(); i++ )
    {
        theTri = ( tmpTriSet->theTriSet ).GetAt( i );
        ib1i = theTri.ib1;
        ib2i = theTri.ib2;
        ib3i = theTri.ib3;
        //Rem 1_2  for i_th triangle
        SDot2XY( ib1i, x3, y3 );
        SDot2XY( ib2i, x4, y4 );
        tie2( x1, y1, x2, y2, x3, y3, x4, y4, xc, yc, status );
        if ( status == 1 ) break;
        //Rem 2_3 for i_th triangle
        SDot2XY( ib3i, x3, y3 );
        tie2( x1, y1, x2, y2, x3, y3, x4, y4, xc, yc, status );
        if ( status == 1 ) break;
        //Rem 1_3 for i_th triangle
        SDot2XY( ib1i, x4, y4 );
        tie2( x1, y1, x2, y2, x3, y3, x4, y4, xc, yc, status );
        if ( status == 1 ) break;
    }
    if ( status == 1 ) //相交
        return true;
    else
        return false;
}

/////////////////////////////////////////////////
//	进一步判断所形成的三角形是否有效！
//	判断条件是这个三角形不与任何其他三角形重合，
//	重合，返回true, 否则，返回false
//Rem the L-th triangle is valid ? to check three sides
bool CTriangle::Function02_Ex( const SDot ib1L, const SDot ib2L, const SDot ib3L )
{
    int ks1( 0 ), ks2( 0 ), ks3( 0 );
    int i;
    SDot ib1i, ib2i, ib3i;

    int num = m_trisets.GetUpperBound();
    if ( num == -1 ) return false; //如果没有三角形集合，则可定不重合

    CTriSet* tmpTriSet;
    STriangle theTri;
    tmpTriSet = ( CTriSet* )m_trisets[m_trisets.GetUpperBound()];
    for ( i = 0; i <= tmpTriSet->theTriSet.GetUpperBound(); i++ )
    {
        theTri = tmpTriSet->theTriSet[i];
        ib1i = theTri.ib1;
        ib2i = theTri.ib2;
        ib3i = theTri.ib3;

        //Rem 1_2
        if ( ib1L == ib1i && ib2L == ib2i )	ks1 = ks1 + 1;
        if ( ib1L == ib2i && ib2L == ib1i )	ks1 = ks1 + 1;
        if ( ib1L == ib2i && ib2L == ib3i )	ks1 = ks1 + 1;
        if ( ib1L == ib3i && ib2L == ib2i )	ks1 = ks1 + 1;
        if ( ib1L == ib1i && ib2L == ib3i )	ks1 = ks1 + 1;
        if ( ib1L == ib3i && ib2L == ib1i )	ks1 = ks1 + 1;
        //Rem 2_3
        if ( ib2L == ib1i && ib3L == ib2i )    ks2 = ks2 + 1;
        if ( ib2L == ib2i && ib3L == ib1i )    ks2 = ks2 + 1;
        if ( ib2L == ib2i && ib3L == ib3i )    ks2 = ks2 + 1;
        if ( ib2L == ib3i && ib3L == ib2i )    ks2 = ks2 + 1;
        if ( ib2L == ib1i && ib3L == ib3i )    ks2 = ks2 + 1;
        if ( ib2L == ib3i && ib3L == ib1i )    ks2 = ks2 + 1;
        //Rem 1_3
        if ( ib1L == ib1i && ib3L == ib2i )    ks3 = ks3 + 1;
        if ( ib1L == ib2i && ib3L == ib1i )    ks3 = ks3 + 1;
        if ( ib1L == ib2i && ib3L == ib3i )    ks3 = ks3 + 1;
        if ( ib1L == ib3i && ib3L == ib2i )    ks3 = ks3 + 1;
        if ( ib1L == ib1i && ib3L == ib3i )    ks3 = ks3 + 1;
        if ( ib1L == ib3i && ib3L == ib1i )    ks3 = ks3 + 1;

        if ( ks1 > 1 || ks2 > 1 || ks3 > 1 )
            return true;
    }

    return false;
}

////////////////////////////////////////////////////
//寻找已知三角形的(dot1,dot2)的连线为边的外接三角形的点，并把这个点和...
//	三角形的两个起始点(dot1,dot2)所形成的三角形保存在三角形的集合数组m_trisets中。
//	找到的点必须满足 1：与已知三角形的三个点是否在断层的同一侧；
//					2：所形成的三角形的夹角必须满足需要的条件。
//	其中1，2两点由其他函数实现
bool CTriangle::SearchExtTri( SDot dot1, SDot dot2, SDot dot3,
                              SDot& searchedDot,
                              CArray<SDot, SDot>& usedDots )
{
    int i, j, kc, kc1;
    double alpha;
    double x1, y1, x2, y2, x3, y3, tmpX, tmpY;
    double m1, m2, d, e;
    SDot tmpDot;

    SDot2XY( dot1, x1, y1 );
    SDot2XY( dot2, x2, y2 );
    SDot2XY( dot3, x3, y3 );

    kc1 = 1;
    if ( fabs( x2 - x1 ) < epsilon )
        kc1 = -1;
    else
    {
        e = ( y1 * x2 - y2 * x1 ) / ( x2 - x1 );
        d = ( y2 - y1 ) / ( x2 - x1 );
    }
    if ( kc1 == 1 )
        m1 = y3 - d * x3 - e;
    else
        m1 = x3 - ( x1 + x2 ) / 2;

    kc = -1;

    double dd = DBL_MAX;

    //Rem new triange does not form(kc=-1)
    //首先在散点中寻找...
    tmpDot.enumDotSource = DOTSRC_SCATTER;
    for ( i = 0; i < kp; i++ )	//kp为散点总数.
    {
        //判断点(x[i],[i])与点三角形的已知俩个点是否在同侧
        //如果不再同一侧，继续下一个点的追踪。
        if ( kc1 == 1 )
            m2 = y[i] - d * x[i] - e;
        else
            m2 = x[i] - ( x1 + x2 ) / 2;
        if ( m1 * m2 < 0 )
        {
            tmpDot.index = i;
            if ( IsValidNewTri( kc, alpha, dd,  dot1, dot2, dot3, tmpDot, searchedDot, usedDots ) )
            {
                //ASSERT(kc==-1);
                searchedDot = tmpDot;
            }
        }
    }

    //dd = DBL_MAX;
    //然后在断层点中寻找
    CFault* tmpFault;
    tmpDot.enumDotSource = DOTSRC_FAULT;
    for( i = 0; i < m_faults.GetSize(); i++ )	//断层条数
    {
        tmpFault = ( CFault* )m_faults[i];
        tmpDot.iFaultID = i;
        for( j = 0 ; j < tmpFault->iDots; j++ )
        {
            tmpDot.index = j;
            SDot2XY( tmpDot, tmpX, tmpY );
            if ( kc1 == 1 )
                m2 = tmpY - d * tmpX - e;
            else
                m2 = tmpX - ( x1 + x2 ) / 2;
            if ( m1 * m2 < 0 )
            {
                if ( IsValidNewTri( kc, alpha, dd, dot1, dot2, dot3, tmpDot, searchedDot, usedDots ) )
                {
                    searchedDot = tmpDot;
                }
            }
        }
    }

    //增加一个三角形
    if ( kc == 1 )
        return true;
    else
        return false;
}

//////////////////////////////////////////
// 新三角形成立的条件判断:
//		1: 和断层的关系是否满足；
//		2: 和已有的三角形的关系是否满足；
//		3: 这个新三角形是否满足一些条件，如锐角等。
//
// 判断未知点desdot与三角形的三个点是否在断层的同一侧，并且判断三角形...
//     的开始两个点是否与未知点desdot满足需要的条件,
//     如果满足上述所有条件，则返回true,同时置kc为1，alpha为夹角值。
//////////////////////////////////////////
bool CTriangle::IsValidNewTri( int& kc, double& alpha, double& dd,
                               const SDot srcdot1, const SDot srcdot2, const SDot srcdot3,
                               const SDot desdot, const SDot lastdot,
                               CArray<SDot, SDot>& usedDots )
{
    //判断点desdot与三角形的已知俩个点是否在同侧。
    //如果不再同一侧，继续下一个点的追踪。
    //...这三个点的三条连线当然要在断层的同一侧啦！
    static double x1, y1, x2, y2, x3, y3, x0, y0;
    static double x01, y01;
    static int j;
    static UINT iOnFault;

    //...三个点的连线在同一侧
    //	计算这三个点的x,y坐标
    double d12, d23, d31;
    SDot2XY( srcdot1, x1, y1 );
    SDot2XY( srcdot2, x2, y2 );
    SDot2XY( desdot , x3, y3 );

    d12 = sqrt( ( x1 - x2 ) * ( x1 - x2 ) + ( y1 - y2 ) * ( y1 - y2 ) );
    d23 = sqrt( ( x2 - x3 ) * ( x2 - x3 ) + ( y2 - y3 ) * ( y2 - y3 ) );
    d31 = sqrt( ( x1 - x3 ) * ( x1 - x3 ) + ( y1 - y3 ) * ( y1 - y3 ) );

    if( d31 < epsilon || d23 < epsilon || ( d23 + d31 - dd ) >= 0 ) return false;

    //判断点与起始边的两端点所形成的两条线段必须不与任何断层相交。
    if ( IsLineCrossFaults( srcdot1, desdot, iOnFault ) == true ) return false;
    if ( IsLineCrossFaults( srcdot2, desdot, iOnFault ) == true ) return false;
    //是否有断层点在将要形成的三角形中间，如果有，则将要形成的三角形不再有效。
    static SDot tmpDot;
    tmpDot.enumDotSource = DOTSRC_FAULT;
    for( int i = 0; i < m_faults.GetSize(); i++ )	//断层条数
    {
        tmpDot.iFaultID = i;
        for( j = 0 ; j < ( ( CFault* )m_faults[i] )->iDots; j++ )
        {
            tmpDot.index = j;
            //判断断层上是否有点落在这三个三角形当中，如有，该点无效。
            if( DotRelationshipToTriangle( srcdot1, srcdot2, desdot, tmpDot ) == 1 ) return false;
            //判断断层上任意线段的中点是否落在三角形中，目的在于排除断层线段的两个端点正好搭在
            //	新三角形的两条边上。
            if ( j > 0 )
            {
                SDot2XY( srcdot1, x1, y1 );
                SDot2XY( srcdot2, x2, y2 );
                SDot2XY( srcdot3, x3, y3 );
                SDot2XY( tmpDot, x0, y0 );
                tmpDot.index = j - 1;
                SDot2XY( tmpDot, x01, y01 );
                if( DotRelationshipToTriangle( x1, y2, x2, y2, x3, y3, ( x0 + x01 ) / 2, ( y0 + y01 ) / 2 ) == 1 ) return false;
            }
        }
    }
    //如果起始边与断层重叠，则舍弃
    if ( IsLineCrossFaults( srcdot1, srcdot2, iOnFault ) == true ) return false;
    if ( ( iOnFault & ROL_PARA_OVERLAP ) == ROL_PARA_OVERLAP ) return false;

    ////////////////////////////////////////////////////////
    //判断点和已有的三角形是否相交？
    if( Function01_Ex( srcdot1, desdot ) == true ) return false;
    if( Function01_Ex( srcdot2, desdot ) == true ) return false;

    //判断点形成的三角形是否与其他三角形重复？
    if( Function02_Ex( srcdot1, srcdot2, desdot ) == true ) return false;


    ///////////////////////////////////////////////////////////
    //把该点添加到访问过的点集中
    //usedDots.Add(desdot);
    ///////////////////////////////////////////////////////////

    double alpham;
    if ( d31 > epsilon && d23 > epsilon )
    {
        alpham = -1 * d12 * d12 + d23 * d23 + d31 * d31; //alpham为在循环体内使用的临时变量
        alpham = alpham / ( 2 * d23 * d31 ); //cosC=(a^2+b^2-c^2)/(2*a*b)
        if ( kc == -1 )
        {
            kc = 1;
            alpha = alpham;
            dd = d31 + d23;
            usedDots.Add( desdot );
            return true;
        }
        else if ( alpham < alpha && alpham > -0.9848 ) //如果有更大角度的点
        {
            alpha = alpham;
            dd = d31 + d23;
            usedDots.Add( desdot );
            return true;
        }
    }
    return false;
}

//SDot的点结构转化为点(x0,y0);
void CTriangle::SDot2XY( const SDot srcdot, double& x0, double& y0 )
{
    if ( srcdot.enumDotSource == DOTSRC_SCATTER )
    {
        x0 = x[srcdot.index];
        y0 = y[srcdot.index];
    }
    else
    {
        x0 = ( ( CFault* )m_faults[srcdot.iFaultID] )->x[srcdot.index];
        y0 = ( ( CFault* )m_faults[srcdot.iFaultID] )->y[srcdot.index];
    }
}
void CTriangle::SDot2XY( const SDot srcdot, CPoint& point )
{
    double x0, y0;
    SDot2XY( srcdot, x0, y0 );
    point.x = ( int )x0;
    point.y = ( int )y0;
}

//判断点和三角形的关系
//包含或在线上，返回1， 不包含，返回－1，
//原理：三角形的两个点与待判断的点的交点的连线在它的对边内部。
int CTriangle::DotRelationshipToTriangle( SDot dot1, SDot dot2, SDot dot3, SDot destdot )
{
    double x1, y1, x2, y2, x3, y3, x0, y0;
    SDot2XY( dot1, x1, y1 );
    SDot2XY( dot2, x2, y2 );
    SDot2XY( dot3, x3, y3 );
    SDot2XY( destdot, x0, y0 );
    return DotRelationshipToTriangle( x1, y1, x2, y2, x3, y3, x0, y0 );
}

int CTriangle::DotRelationshipToTriangle( double x1, double y1, double x2, double y2, double x3, double y3, double x0, double y0 )
{
    double dmax, dmin, dymax, dymin;
    int status1;
    double xc, yc;

    tie2( x1, y1, x2, y2, x3, y3, x0, y0, xc, yc, status1 );
    if ( x1 >= x2 )
    {
        dmax = x1;
        dmin = x2;
    }
    else
    {
        dmax = x2;
        dmin = x1;
    }
    if ( y1 >= y2 )
    {
        dymax = y1;
        dymin = y2;
    }
    else
    {
        dymax = y2;
        dymin = y1;
    }
    if ( !( ( xc > dmin && xc < dmax ) && ( yc > dymin && yc < dymax ) ) )
        return -1;
    else if( xc == x0 && yc == y0 ) return 0;

    tie2( x2, y2, x3, y3, x1, y1, x0, y0, xc, yc, status1 );
    if ( x2 >= x3 )
    {
        dmax = x2;
        dmin = x3;
    }
    else
    {
        dmax = x3;
        dmin = x2;
    }
    if ( y2 >= y3 )
    {
        dymax = y2;
        dymin = y3;
    }
    else
    {
        dymax = y3;
        dymin = y2;
    }
    if ( !( ( xc > dmin && xc < dmax ) && ( yc > dymin && yc < dymax ) ) )
        return -1;
    else if( xc == x0 && yc == y0 ) return 0;

    tie2( x1, y1, x3, y3, x2, y2, x0, y0, xc, yc, status1 );
    if ( x1 >= x3 )
    {
        dmax = x1;
        dmin = x3;
    }
    else
    {
        dmax = x3;
        dmin = x1;
    }
    if ( y1 >= y3 )
    {
        dymax = y1;
        dymin = y3;
    }
    else
    {
        dymax = y3;
        dymin = y1;
    }
    if ( !( ( xc > dmin && xc < dmax ) && ( yc > dymin && yc < dymax ) ) )
        return -1;
    else if( xc == x0 && yc == y0 ) return 0;

    return 1;
}

//追踪多个三角形集合，
// 每一个三角形集合的形成是由Createtriset形成
void CTriangle::CreateTriSetEx( CDC* dc )
{
    SDot theDot;
    CArray<SDot, SDot> usedDots;

    theDot.enumDotSource = DOTSRC_SCATTER;
    for ( int i = 0; i < kp; i++ )
    {
        theDot.index = i;

        bool bExist( false ); //目的点在usedDots点集中存在吗？
        for ( int j = 0; j <= usedDots.GetUpperBound(); j++ )
        {
            if( theDot == usedDots[j] )
            {
                bExist = true;
                break;
            }
        }

        if( bExist == false )
        {
            CString msg;
            msg.Format( "第%d次", i );
            AfxMessageBox( msg );
            CreateTriSet( dc, theDot, usedDots );
        }
    }
}
