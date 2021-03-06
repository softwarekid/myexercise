//由于断层上没有Z值，本文件的目的在给
//    断层插入Z值
#include "stdafx.h"
#include "triangle.h"
#include "somefuncs.h"

//在断层上插入z值
//方法：从三角形集合中查找含有所需点的所有三角形，记录这些三角形的点的Z值，然后再从
//      这些三角形的点往外寻找三角形并记录这些三角形的点的Z值，如此循环，直到找到6
//      个点为只，利用这6个点的Z值和X,Y值表征一个6次方程，计算Z值即为目的点的Z值。
//
//		介绍start和number,如下，
//          start=2,number=4,从start开始的number个数是↓指向的四个数
//          ∧为扩展四个↓得到的点
//
//          ↓↓↓↓ ∧∧∧∧∧∧∧∧
//		[][][][][][][][][][][][][][]  ←这为extList
//			↑		↑
//         start    start+number
//
double CTriangle::InsertZinDot( SDot destDot )
{
    int i, j, k, triscount, start, number;
    CTriSet* ptTriSet;
    SDot theDot, extDot;
    STriangle theTri;
    CList<SDot, SDot> retList, extList;


    //开始寻找所需的6个点
    for ( i = 0; i < m_trisets.GetSize(); i++ )
    {
        ptTriSet = ( CTriSet* )m_trisets[i];
        triscount = ptTriSet->theTriSet.GetSize();

        retList.RemoveAll();
        extList.RemoveAll();
        extList.AddTail( destDot ); //初始化extList
        start = 0;	//在extlist中从start开始的number个点进行循环，依次查找外接三角形
        number = 1;
        do
        {
            //扩展某一层的点集
            for( j = start; j < start + number; j++ )
            {
                extDot = extList.GetAt( extList.FindIndex( j ) ); //正在扩展的点
                for( k = 0; k < triscount; k++ )
                {
                    theTri = ptTriSet->theTriSet[k];
                    if( extDot == theTri.ib1 )
                    {
                        if( extList.Find( theTri.ib2 ) == NULL ) extList.AddTail( theTri.ib2 );
                        if( extList.Find( theTri.ib3 ) == NULL ) extList.AddTail( theTri.ib3 );
                    }
                    else if( extDot == theTri.ib2 )
                    {
                        if( extList.Find( theTri.ib1 ) == NULL ) extList.AddTail( theTri.ib1 );
                        if( extList.Find( theTri.ib3 ) == NULL ) extList.AddTail( theTri.ib3 );
                    }
                    else if( extDot == theTri.ib3 )
                    {
                        if( extList.Find( theTri.ib1 ) == NULL ) extList.AddTail( theTri.ib1 );
                        if( extList.Find( theTri.ib2 ) == NULL ) extList.AddTail( theTri.ib2 );
                    }
                }
            } //该层点已经全部扩展完毕

            //建立结果集合
            for( k = start + number; k < extList.GetCount(); k++ ) //如果扩展三角形数量为0，则此for部循环。
            {
                theDot = extList.GetAt( extList.FindIndex( k ) );
                if( theDot.enumDotSource == DOTSRC_SCATTER )
                    if( retList.Find( theDot ) == NULL )
                        retList.AddTail( theDot );
            }

            //是否继续进行下一级扩展
            if( extList.GetCount() == start + number || retList.GetCount() >= 6 )
                break;
            else
            {
                start += number;
                number = extList.GetCount() - number;
            }
        }
        while( 1 );

        if( retList.GetCount() != 0 ) break;
    }
    //已经完成了关联点(6个或更少)的查找,保存在retList集合中


    //下面依据retList中的6个点建立一个6次方程，求出相应的Z值。
    // 当找到的点小于6个是，使用距离加权方法，等于6个时采用二次曲面方程
    CSomeFuncs somefuncs;
    double dZvalue;
    int count;
    SPoint srcPnt[6];
    SPoint desPnt;

    count = retList.GetCount();
    if ( count == 0 ) return -1;	//没有点
    if ( count > 6 )	count = 6;	//点的总数不超过6

    for( i = 0; i < count; i++ )
    {
        theDot = retList.GetAt( retList.FindIndex( i ) );
        srcPnt[i].x = x[theDot.index];  //6个点全为散点，不能为断层点
        srcPnt[i].y = y[theDot.index];
        srcPnt[i].z = z[theDot.index];
    }
    desPnt.x = ( ( CFault* )m_faults[destDot.iFaultID] )->x[destDot.index]; //目的点可能为断层点
    desPnt.y = ( ( CFault* )m_faults[destDot.iFaultID] )->y[destDot.index]; //    如果为插入的散点，则x,y,z全为未知
    //    因此，目前只考虑断层点

#if defined(XLD_READ) //测试
    CBrush newBrush( RGB( 255, 0, 0 ) );
    CPen newPen( PS_SOLID, 1, RGB( 255, 0, 0 ) );

    CPen* oldPen = ( CPen* )mydc->SelectObject( &newPen );
    CBrush* oldbrush = ( CBrush* )mydc->SelectObject( &newBrush );
    mydc->Ellipse( desPnt.x - xmin - 200, desPnt.y - ymin - 200, desPnt.x - xmin + 200, desPnt.y - ymin + 200 );

    mydc->SelectStockObject( BLACK_BRUSH );
    mydc->SelectStockObject( BLACK_PEN );
    for( int n = 0; n < count; n++ )
        mydc->Ellipse( srcPnt[n].x - xmin - 200, srcPnt[n].y - ymin - 200, srcPnt[n].x - xmin + 200, srcPnt[n].y - ymin + 200 );

    mydc->SelectObject( oldPen );
    mydc->SelectObject( oldbrush );
    //AfxMessageBox("Press any key continue...");
#endif


    if( count == 6 )
    {
//		ASSERT(desPnt.x != 7199575 && desPnt.y != 6192169);
        dZvalue = somefuncs.tend( srcPnt, desPnt ); //求解z值,结果保存在desPnt.z中
    }
    else
    {
        AfxMessageBox( "Insert Z value---Less 6 dots!" );
        ;//距离加权????
    }


    /////////////////修改目的点的z值
    ( ( CFault* )m_faults[destDot.iFaultID] )->z[destDot.index] = desPnt.z; //给目的点修改z值

    return dZvalue;
}



//SDOT向SPoint结构转换
SPoint CTriangle::SDot2SPoint( SDot srcDot )
{
    SPoint retPnt;

    if ( srcDot.enumDotSource == DOTSRC_SCATTER )
    {
        retPnt.x = x[srcDot.index];
        retPnt.y = y[srcDot.index];
        retPnt.z = z[srcDot.index];
    }
    else //DOTSRC_FAULT
    {
        retPnt.x = ( ( CFault* )m_faults[srcDot.iFaultID] )->x[srcDot.index];
        retPnt.y = ( ( CFault* )m_faults[srcDot.iFaultID] )->y[srcDot.index];
        retPnt.z = ( ( CFault* )m_faults[srcDot.iFaultID] )->z[srcDot.index];
    }
    return retPnt;
}
