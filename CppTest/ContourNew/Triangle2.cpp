#include "stdafx.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "Triangle.h"
#include "Smooth.h"

//从m_trisets中查找查找所有的三角形集合并把符合zvalue值的三角形并绘制等值线。。。
void CTriangle::CreateContourEx( CDC* dc, const double zvalue )
{
    for ( int i = 0; i < m_trisets.GetSize(); i++ )
    {
        CreateContour( dc, zvalue, ( CTriSet* )m_trisets[i] );
    }

}

//从指定的三角形集合中画等直线
//  查找符合zvalue值的三角形并绘制等值线。。。
void CTriangle::CreateContour( CDC* dc, const double zvalue, CTriSet* triset )
{
    CSmooth smooth;
    SDot dot1, dot2, dot3;		//三角形的三个顶点
    SPoint pnt1, pnt2, pnt3;	//同上，但形式不一样，三角形的三个顶点
    CPoint points[2000];//记载等值线的一系列点
    int iUsedTri[2000], iUsedTriCount( 0 ); //在追踪过程中已经画过的三角形
    int index;	//记载已经追踪的三角形的总数
    int FirstTri, SecondTri, InTri; //从序号为FirstTri的三角形的哪一条边开始追踪...
    // 追踪到的第二个三角形序号，正在追踪的三角形序号
    SDot dot1_index_bak, dot2_index_bak;//第二个三角形的两个角点的索引
    SDot dot1_index, dot2_index;	//三角形的三 个 点
    double zbak;//zvalue是常量,不能加上一个epsilon,故用此变量

    bool HadSearched; //是否找到了合适的相邻三角形
    int i;

    //？？？
    double* dArrayZ = new double[kp];	//以前采用临时调整zvalue的方法,现在采用临时调整
    for( i = 0; i < kp; i++ )				//数组z的方法.但仍然保留老的方法,因为,同时采用两个方法
    {
        //并不冲突,后者会自然屏蔽前者方法.声明内部变量
        dArrayZ[i] = z[i];			//对成员变量数组z进行备份,然后在结束时恢复数组z
        if ( z[i] == zvalue ) z[i] += epsilon; //只在这个地方对Z进行了修改
    }

    HadSearched = false;
    for( i = 1; i < triset->ibcount; i++ )
    {
        if ( SearchTriInUsedTris( i, iUsedTri, iUsedTriCount ) == true ) continue;
        zbak = zvalue;
        dot1 = triset->theTriSet[i].ib1;
        pnt1 = SDot2SPoint( dot1 );
        dot2 = triset->theTriSet[i].ib2;
        pnt2 = SDot2SPoint( dot2 );
        dot3 = triset->theTriSet[i].ib3;
        pnt3 = SDot2SPoint( dot3 );

        if ( !( ( ( pnt1.z - zbak ) * ( pnt2.z - zbak ) <= 0 ) || ( ( pnt3.z - zbak ) * ( pnt2.z - zbak ) <= 0 ) ) )	continue;

        //找到了合适的三角形!!!
        //从第一号三角形开始查找满足条件的等值点
        HadSearched = true;
        index = 0;
        if ( ( ( pnt1.z - zbak ) * ( pnt2.z - zbak ) == 0 ) || ( ( pnt3.z - zbak ) * ( pnt2.z - zbak ) == 0 ) )
        {
            zbak += epsilon;
            if ( !( ( ( pnt1.z - zbak ) * ( pnt2.z - zbak ) < 0 ) || ( ( pnt3.z - zbak ) * ( pnt2.z - zbak ) < 0 ) ) )
                zbak = zbak - 2 * epsilon;
        }

        if ( ( pnt1.z - zbak ) * ( pnt2.z - zbak ) < 0 )
        {
            dot1_index = dot1;
            dot2_index = dot2;
        }
        else
        {
            dot1_index = dot3;
            dot2_index = dot2;
        }
        points[index].x = ( SDot2SPoint( dot1_index ).x * ( zbak - SDot2SPoint( dot2_index ).z ) + SDot2SPoint( dot2_index ).x * ( SDot2SPoint( dot1_index ).z - zbak ) ) / ( SDot2SPoint( dot1_index ).z - SDot2SPoint( dot2_index ).z );
        points[index].y = ( SDot2SPoint( dot1_index ).y * ( zbak - SDot2SPoint( dot2_index ).z ) + SDot2SPoint( dot2_index ).y * ( SDot2SPoint( dot1_index ).z - zbak ) ) / ( SDot2SPoint( dot1_index ).z - SDot2SPoint( dot2_index ).z );
        index++;
        FirstTri = i;
        InTri = i;
        dot1_index_bak = dot1_index;
        dot2_index_bak = dot2_index;

        //然后从临接三角形中再次查找满足条件的等值点
        for ( int j = 0; j < 2; j++ ) //只循环两次,一次为正向追踪,另外一次为反向追踪
        {
            while( 1 )
            {
                if ( GetNextTriangle( dot1_index, dot2_index, InTri, zvalue, zbak, iUsedTri, iUsedTriCount, triset ) == true )
                {
                    if ( index == 1 && j == 0 ) SecondTri = InTri;
                    SaveUsedTris( iUsedTri, iUsedTriCount, InTri );
                    points[index].x = ( SDot2SPoint( dot1_index ).x * ( zbak - SDot2SPoint( dot2_index ).z ) + SDot2SPoint( dot2_index ).x * ( SDot2SPoint( dot1_index ).z - zbak ) ) / ( SDot2SPoint( dot1_index ).z - SDot2SPoint( dot2_index ).z );
                    points[index].y = ( SDot2SPoint( dot1_index ).y * ( zbak - SDot2SPoint( dot2_index ).z ) + SDot2SPoint( dot2_index ).y * ( SDot2SPoint( dot1_index ).z - zbak ) ) / ( SDot2SPoint( dot1_index ).z - SDot2SPoint( dot2_index ).z );
                    index++;
                }
                else
                    break;
            }
            if( j == 0 )	//如果正向追踪完毕，则设置反向追踪条件，继续追踪
            {
                double kk = index / 2;
                for( int	k = 0; k < ( int )floor( kk ); k++ ) //倒序排列正向追踪过程中已经追踪到的点
                {
                    CPoint tmpPoint = points[k];
                    points[k] = points[index - 1 - k];
                    points[index - 1 - k] = tmpPoint;
                }
                //反向追踪,下面三条语句是根据两点及一个三角形，必然能找到另一个三角形
                dot1_index = dot1_index_bak;
                dot2_index = dot2_index_bak;
                InTri = SecondTri;
            }
        } // End for j

        // 如果追踪完一条等值线，则绘制该条等值线
        for ( int k = 0; k < index; k++ )
        {
            points[k].x -= xmin;
            points[k].y -= ymin;
        }
        dc->Polyline( points, index ); //画折线等值线

        smooth.SmoothPolyLine( points, index );	//把找到的等值点进行平滑处理
        smooth.DoSmooth( dc );			//把平滑处理结果画在屏幕上
        smooth.DoLable( dc, zvalue );		//处理结果的基础上标注
    } // End for i

    if ( HadSearched == false )
    {
        CString msg;
        msg.Format( _T( "z=%.3f\n不能找到第一个点!" ), zvalue );
        AfxMessageBox( msg );
    }

    for( i = 0; i < kp; i++ )		//恢复数组z原来的值
        z[i] = dArrayZ[i];
    delete []dArrayZ;
}//end function

//如果成功的找到了下一个三角形,则返回True,
//一条边只能被两个三角形共享，所以，根据两点（一条边）及一个三角形的编号，必然能找到另一个三角形。
bool CTriangle::GetNextTriangle( SDot& dot1_index, SDot& dot2_index, int& InTri, const double zvalue, double& zbak, const int* iUsedTri, const int iUsedTriCount, CTriSet* triset )
{
    bool bFound( false );
    SDot dot3_index;

    zbak = zvalue;
    for ( int i = 1; i < triset->ibcount; i++ )
    {
        if ( SearchTriInUsedTris( i, iUsedTri, iUsedTriCount ) == true ) continue;
        if( ( dot1_index == triset->theTriSet[i].ib1
                || dot1_index == triset->theTriSet[i].ib2
                || dot1_index == triset->theTriSet[i].ib3 )
                && ( dot2_index == triset->theTriSet[i].ib1
                     || dot2_index == triset->theTriSet[i].ib2
                     || dot2_index == triset->theTriSet[i].ib3 ) && ( i != InTri ) )
        {
            bFound = true;
            if( dot1_index == triset->theTriSet[i].ib1 )
            {
                if( dot2_index == triset->theTriSet[i].ib2 )
                    dot3_index = triset->theTriSet[i].ib3;
                else
                    dot3_index = triset->theTriSet[i].ib2;
            }
            else if( dot1_index == triset->theTriSet[i].ib2 )
            {
                if( dot2_index == triset->theTriSet[i].ib1 )
                    dot3_index = triset->theTriSet[i].ib3;
                else
                    dot3_index = triset->theTriSet[i].ib1;
            }
            else
            {
                if( dot2_index == triset->theTriSet[i].ib1 )
                    dot3_index = triset->theTriSet[i].ib2;
                else
                    dot3_index = triset->theTriSet[i].ib1;
            } // 求解dot3_index

            if ( zbak == SDot2SPoint( dot1_index ).z )
            {
                SDot tmp_index = dot2_index;
                dot2_index = dot1_index;
                dot1_index = tmp_index;
            }
            if ( ( SDot2SPoint( dot3_index ).z - zbak ) * ( SDot2SPoint( dot1_index ).z - zbak ) <= 0 )
            {
                if ( ( SDot2SPoint( dot3_index ).z - zbak ) * ( SDot2SPoint( dot1_index ).z - zbak ) == 0 )
                {
                    zbak += epsilon;
                    if ( !( ( SDot2SPoint( dot3_index ).z - zbak ) * ( SDot2SPoint( dot1_index ).z - zbak ) == 0 ) )
                        zbak = zbak - 2 * epsilon;
                }
                dot1_index = dot1_index;
                dot2_index = dot3_index;
            }
            else
            {
                if ( ( SDot2SPoint( dot3_index ).z - zbak ) * ( SDot2SPoint( dot2_index ).z - zbak ) == 0 )
                {
                    zbak += epsilon;
                    if ( !( ( SDot2SPoint( dot3_index ).z - zbak ) * ( SDot2SPoint( dot2_index ).z - zbak ) == 0 ) )
                        zbak = zbak - 2 * epsilon;
                }
                dot1_index = dot2_index;
                dot2_index = dot3_index;
            }
            InTri = i;
            break;
        }
    }//end for
    return bFound;
}

//保存已经在其中画过等值线的三角形的号,成功 保存,返回true.
void CTriangle::SaveUsedTris( int* iUsedTri, int& iUsedTriCount, const int iTriNum )
{
    iUsedTri[iUsedTriCount] = iTriNum;
    iUsedTriCount++;
}

//如果 在用过的三角形列表中存在由SRCTRI指定的三角形号,则返回TRUE
//srcTri为三角形的序号, iUsedTri为用过的三角形的数组, iUsedTriCount为该数组的大小
bool CTriangle::SearchTriInUsedTris( const int srcTri, const int* iUsedTri, const int iUsedTriCount )
{
    bool theRect( false );

    for( int i = 0; i < iUsedTriCount; i++ )
        if( iUsedTri[i] == srcTri )
        {
            theRect = true;
            break;
        }
    return theRect;
}



//判断已知鼠标点是否落在控制点上,如果是,则返回TRUE, 并返回控制点的Z值, 否则返回FALSE
bool CTriangle::IsInControlDots( const POINT point, double& zval )
{
    int r( 200 );
    bool theRet( false );

    //散点
    for ( int i = 0; i < kp; i++ )
    {
        if ( ( x[i] - ( point.x + xmin ) ) * ( x[i] - ( point.x + xmin ) ) + ( y[i] - ( point.y + ymin ) ) * ( y[i] - ( point.y + ymin ) ) < r * r )
        {
            zval = z[i];
            return true;
        }
    }

    //断层点
    for ( int i = 0; i < m_faults.GetSize(); i++ )
    {
        for( int k = 0; k < m_faults[i]->iDots; k++ )
        {
            double x, y;
            x = m_faults[i]->x[k];
            y = m_faults[i]->y[k];
            if ( ( x - ( point.x + xmin ) ) * ( x - ( point.x + xmin ) ) + ( y - ( point.y + ymin ) ) * ( y - ( point.y + ymin ) ) < r * r )
            {
                zval = ( double )m_faults[i]->z[k];
                return true;
            }
        }
    }
    return theRet;
}


