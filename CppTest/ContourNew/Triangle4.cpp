//
// File name: Triangle4.cpp
// ��;: ׷��ɢ�����ݺͶϲ����ݲ��γ������μ���
//		�����������ļ�triangle.cpp, triangle2.cpp, triangle3.cpp
//		һͬ������CTriangle�ࡣ
// �����ĺ�����
//
// ��Ȩ���У�2001�꣩�� ��GriSoft�������а�Ȩ
//

#include "stdafx.h"
#include "Triangle.h"
#include "somefuncs.h"

#include <cfloat>
#include <climits>

//׷��ɢ�����ݺͶϲ����ݲ��γ������μ���
//�κ������ε�����������ֻ��һ�����ڶϲ����ݵ��ϡ�
//�Ե�theDotΪ���׷��һ�������μ��ϣ� ���ʹ��ĵ㱣����usedDots��
void CTriangle::CreateTriSet( CDC* dc, SDot theDot, CArray<SDot, SDot>& usedDots )
{
    double dd, dm, alpha;
    double xc( 0 ), yc( 0 );
    int L, i, j, kc, k, km;	//Add by Robter
    CPoint Points[4];
    STriangle theTriangle;

    L = 1;

    //�����е��в��Һ���ʼ��(x[0],y[0])��������ĵ�,����ib2[1]�м����������x[]�е��±��
    //...�趨��һ�������εĵ�һ����Ϊ������λ0��ɢ��
    theTriangle.ib1 = theDot;

    //...��ʼ���ҵ�һ�������εĵڶ����㣬������Ϊ0��ɢ�㿪ʼ����...
    //	 ���ڶ�����Ҳ����Ϊ�ϲ�㣬���Բ�����ɢ��֮�󣬼����ڶϲ���в���...
    //	 ֱ���ҵ��������εĵ�һ���������ĵ�Ϊֹ��
    //......������ɢ�����ݵ��в���...
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

    theTriangle.ib2 = theDot; //...OK�� �ҵ��˵�һ�������εĵڶ�����
    //d12 = dd; //...d12�����˵�һ�������εĵ�һ����͵ڶ�����֮��ľ���

    //
    //���ҵ�һ�������εĵ������㣬�õ������ɢ�㣬Ҳ����Ϊ�ϲ�㡣
    //...������ɢ���в��ң�
    kc = -1;//kcΪ��һ�ν���ѭ���ı�־,����������forѭ�����ҵ��˺��������ĵ����㣬
    //	��kc��ֵΪ1��
    dd = DBL_MAX;
    tmpDot.enumDotSource = DOTSRC_SCATTER;
    for ( i = 0; i < kp; i++ )
    {
        //�жϵ�(x[i],[i])��������ε���֪�������Ƿ���ͬ�ࡣ
        //�������ͬһ�࣬������һ�����׷�١�
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
    //...Ȼ���ٴӶϲ��в���
    CFault* tmpFault;
    tmpDot.enumDotSource = DOTSRC_FAULT;
    for( i = 0; i < m_faults.GetSize(); i++ )	//�ϲ�����
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

    //�Ե�һ����������˵���Ѿ��ҵ��˺��ʵĵ�����,�����������μ���������һ�������Ρ�
    if( kc != -1 )
    {
        theTriangle.ib3 = theDot;
        m_trisets.Add( new CTriSet ); //���ӵ�һ�������μ���
        //Ϊ����µ������μ��ϼ���һ�������Σ�
        ( ( CTriSet* )m_trisets[m_trisets.GetUpperBound()] )->theTriSet.Add( theTriangle );
        usedDots.Add( theTriangle.ib1 ); //������ʹ��ĵ�
        usedDots.Add( theTriangle.ib2 );
        usedDots.Add( theTriangle.ib3 );
    }
    else
    {
        AfxMessageBox( "û�з��ֵ�һ�������εĵ������㣡" );
        return;
    }

    // OK, ��һ���������Ѿ�׷����ϣ�����
    ////////////////////////////////////////
    if ( kc != -1 ) Draw( 0, L - 1, dc );


    //=========��ѭ��==========
    // ��ʼ׷�����������Ρ�

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
            break; //LΪ�����ε�������kΪ������չ�����������ε���š�
        }
        iPos = m_trisets.GetUpperBound();
        ib1k = ( ( ( CTriSet* )m_trisets[iPos] )->theTriSet ).GetAt( k - 1 ).ib1;
        ib2k = ( ( ( CTriSet* )m_trisets[iPos] )->theTriSet ).GetAt( k - 1 ).ib2;
        ib3k = ( ( ( CTriSet* )m_trisets[iPos] )->theTriSet ).GetAt( k - 1 ).ib3;

        //	first side for k_th triangle
//			ASSERT(k<25);
        // ����1���������⣬������������ֻ����2����
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
//	ibcount=L;//�������εĸ���L������ĳ�Ա����
    ( ( CTriSet* )m_trisets[m_trisets.GetUpperBound()] )->ibcount = L; //�������εĸ���L������ĳ�Ա����
}

//////////////////////////////////////////////////////////
//
// ���ܣ��ж϶������Ƿ��ڶϲ��ͬһ�࣬ ����˵�߶���ϲ�Ĺ�ϵ��
//		�жϵ�ԭ������������ߵ�һ���ֲ����κζϲ��ߵ�һ�����ཻ��
//		ib1��ib2ΪSDot�ṹ�壬�����˵��ڶϲ��ɢ��������
//		����ཻ����True, ���򷵻�false;
// ����: �ӿڲ�������Ҫ�жϵĶ����㣬
//		 ��Ա�������ϲ�����m_faults
//		iLinkFault��λ�������߶����߶ν���ľ��������
// ������жϽ����false/true
// ���ã������˳�Ա����CSomeFuncs.Relation_Of_Lines()
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
    int iDotsPerFault; //��i���ϲ�ĵ�����
    int i, j;
    int status1( 0 );
    double x3, x4, xc( 0 ), y3, y4, yc( 0 );
    CSomeFuncs somefuncs;

    iLinkFault = 0;

    for ( i = 0; i < faultscount; i++ ) //���δ���ÿһ���ϲ�
    {
        //��i���ϲ�
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
//	�ж��߶��Ƿ������е��κ������������ཻ��
//	����ཻ������true, ���򣬷���false
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
    if ( num == -1 ) return false;	//���û�������μ��ϣ���ɶ����ཻ��

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
    if ( status == 1 ) //�ཻ
        return true;
    else
        return false;
}

/////////////////////////////////////////////////
//	��һ���ж����γɵ��������Ƿ���Ч��
//	�ж���������������β����κ������������غϣ�
//	�غϣ�����true, ���򣬷���false
//Rem the L-th triangle is valid ? to check three sides
bool CTriangle::Function02_Ex( const SDot ib1L, const SDot ib2L, const SDot ib3L )
{
    int ks1( 0 ), ks2( 0 ), ks3( 0 );
    int i;
    SDot ib1i, ib2i, ib3i;

    int num = m_trisets.GetUpperBound();
    if ( num == -1 ) return false; //���û�������μ��ϣ���ɶ����غ�

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
//Ѱ����֪�����ε�(dot1,dot2)������Ϊ�ߵ���������εĵ㣬����������...
//	�����ε�������ʼ��(dot1,dot2)���γɵ������α����������εļ�������m_trisets�С�
//	�ҵ��ĵ�������� 1������֪�����ε��������Ƿ��ڶϲ��ͬһ�ࣻ
//					2�����γɵ������εļнǱ���������Ҫ��������
//	����1��2��������������ʵ��
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
    //������ɢ����Ѱ��...
    tmpDot.enumDotSource = DOTSRC_SCATTER;
    for ( i = 0; i < kp; i++ )	//kpΪɢ������.
    {
        //�жϵ�(x[i],[i])��������ε���֪�������Ƿ���ͬ��
        //�������ͬһ�࣬������һ�����׷�١�
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
    //Ȼ���ڶϲ����Ѱ��
    CFault* tmpFault;
    tmpDot.enumDotSource = DOTSRC_FAULT;
    for( i = 0; i < m_faults.GetSize(); i++ )	//�ϲ�����
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

    //����һ��������
    if ( kc == 1 )
        return true;
    else
        return false;
}

//////////////////////////////////////////
// �������γ����������ж�:
//		1: �Ͷϲ�Ĺ�ϵ�Ƿ����㣻
//		2: �����е������εĹ�ϵ�Ƿ����㣻
//		3: ������������Ƿ�����һЩ����������ǵȡ�
//
// �ж�δ֪��desdot�������ε��������Ƿ��ڶϲ��ͬһ�࣬�����ж�������...
//     �Ŀ�ʼ�������Ƿ���δ֪��desdot������Ҫ������,
//     ����������������������򷵻�true,ͬʱ��kcΪ1��alphaΪ�н�ֵ��
//////////////////////////////////////////
bool CTriangle::IsValidNewTri( int& kc, double& alpha, double& dd,
                               const SDot srcdot1, const SDot srcdot2, const SDot srcdot3,
                               const SDot desdot, const SDot lastdot,
                               CArray<SDot, SDot>& usedDots )
{
    //�жϵ�desdot�������ε���֪�������Ƿ���ͬ�ࡣ
    //�������ͬһ�࣬������һ�����׷�١�
    //...����������������ߵ�ȻҪ�ڶϲ��ͬһ������
    static double x1, y1, x2, y2, x3, y3, x0, y0;
    static double x01, y01;
    static int j;
    static UINT iOnFault;

    //...�������������ͬһ��
    //	�������������x,y����
    double d12, d23, d31;
    SDot2XY( srcdot1, x1, y1 );
    SDot2XY( srcdot2, x2, y2 );
    SDot2XY( desdot , x3, y3 );

    d12 = sqrt( ( x1 - x2 ) * ( x1 - x2 ) + ( y1 - y2 ) * ( y1 - y2 ) );
    d23 = sqrt( ( x2 - x3 ) * ( x2 - x3 ) + ( y2 - y3 ) * ( y2 - y3 ) );
    d31 = sqrt( ( x1 - x3 ) * ( x1 - x3 ) + ( y1 - y3 ) * ( y1 - y3 ) );

    if( d31 < epsilon || d23 < epsilon || ( d23 + d31 - dd ) >= 0 ) return false;

    //�жϵ�����ʼ�ߵ����˵����γɵ������߶α��벻���κζϲ��ཻ��
    if ( IsLineCrossFaults( srcdot1, desdot, iOnFault ) == true ) return false;
    if ( IsLineCrossFaults( srcdot2, desdot, iOnFault ) == true ) return false;
    //�Ƿ��жϲ���ڽ�Ҫ�γɵ��������м䣬����У���Ҫ�γɵ������β�����Ч��
    static SDot tmpDot;
    tmpDot.enumDotSource = DOTSRC_FAULT;
    for( int i = 0; i < m_faults.GetSize(); i++ )	//�ϲ�����
    {
        tmpDot.iFaultID = i;
        for( j = 0 ; j < ( ( CFault* )m_faults[i] )->iDots; j++ )
        {
            tmpDot.index = j;
            //�ж϶ϲ����Ƿ��е����������������ε��У����У��õ���Ч��
            if( DotRelationshipToTriangle( srcdot1, srcdot2, desdot, tmpDot ) == 1 ) return false;
            //�ж϶ϲ��������߶ε��е��Ƿ������������У�Ŀ�������ų��ϲ��߶ε������˵����ô���
            //	�������ε��������ϡ�
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
    //�����ʼ����ϲ��ص���������
    if ( IsLineCrossFaults( srcdot1, srcdot2, iOnFault ) == true ) return false;
    if ( ( iOnFault & ROL_PARA_OVERLAP ) == ROL_PARA_OVERLAP ) return false;

    ////////////////////////////////////////////////////////
    //�жϵ�����е��������Ƿ��ཻ��
    if( Function01_Ex( srcdot1, desdot ) == true ) return false;
    if( Function01_Ex( srcdot2, desdot ) == true ) return false;

    //�жϵ��γɵ��������Ƿ��������������ظ���
    if( Function02_Ex( srcdot1, srcdot2, desdot ) == true ) return false;


    ///////////////////////////////////////////////////////////
    //�Ѹõ����ӵ����ʹ��ĵ㼯��
    //usedDots.Add(desdot);
    ///////////////////////////////////////////////////////////

    double alpham;
    if ( d31 > epsilon && d23 > epsilon )
    {
        alpham = -1 * d12 * d12 + d23 * d23 + d31 * d31; //alphamΪ��ѭ������ʹ�õ���ʱ����
        alpham = alpham / ( 2 * d23 * d31 ); //cosC=(a^2+b^2-c^2)/(2*a*b)
        if ( kc == -1 )
        {
            kc = 1;
            alpha = alpham;
            dd = d31 + d23;
            usedDots.Add( desdot );
            return true;
        }
        else if ( alpham < alpha && alpham > -0.9848 ) //����и���Ƕȵĵ�
        {
            alpha = alpham;
            dd = d31 + d23;
            usedDots.Add( desdot );
            return true;
        }
    }
    return false;
}

//SDot�ĵ�ṹת��Ϊ��(x0,y0);
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

//�жϵ�������εĹ�ϵ
//�����������ϣ�����1�� �����������أ�1��
//ԭ���������ε�����������жϵĵ�Ľ�������������ĶԱ��ڲ���
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

//׷�ٶ�������μ��ϣ�
// ÿһ�������μ��ϵ��γ�����Createtriset�γ�
void CTriangle::CreateTriSetEx( CDC* dc )
{
    SDot theDot;
    CArray<SDot, SDot> usedDots;

    theDot.enumDotSource = DOTSRC_SCATTER;
    for ( int i = 0; i < kp; i++ )
    {
        theDot.index = i;

        bool bExist( false ); //Ŀ�ĵ���usedDots�㼯�д�����
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
            msg.Format( "��%d��", i );
            AfxMessageBox( msg );
            CreateTriSet( dc, theDot, usedDots );
        }
    }
}