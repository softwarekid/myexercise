//
// �ϲ����ݼ��غʹ���
// �˴�ʹ������������飬 ��ע��ʹ�÷���
// CopyRight 2000(R) by GriSoft, All right reserved

#include "stdafx.h"
#include "Triangle.h"

//���ó�Ա����m_faults
//�Ѷϲ������ļ��еĶϲ�����д���Ա����m_faults��
void CTriangle::Set_faults( CString filename )
{
    double x, y;
    double no;
    bool bExist( false );
    int theIndex;

    FILE* fp;

    if ( ( fp = fopen( filename, "r" ) ) == NULL )
    {
        AfxMessageBox( "���ļ�����" );
        return;
    }

    m_faults.RemoveAll();

    while( !feof( fp ) )
    {
        if ( fscanf( fp, "%lf %lf %lf", &x, &y, &no ) != 3 ) break;

        //�����Ƿ���ڱ��Ϊid�Ķϲ���
        bExist = false;
        for ( int i = 0; i < m_faults.GetSize(); i++ )
        {
            int meno = ( ( CFault* )m_faults[i] )->no;
            if( meno == no )
            {
                bExist = true;
                theIndex = i;
                break;
            }
        }
        if ( bExist == true ) //������ڣ����������ϲ���������һ����
        {
            ( ( CFault* )m_faults[theIndex] )->x.Add( x );
            ( ( CFault* )m_faults[theIndex] )->y.Add( y );
            ( ( CFault* )m_faults[theIndex] )->z.Add( 0 );
            ( ( ( CFault* )m_faults[theIndex] )->iDots )++;
        }
        else	//�����ڱ��Ϊid�Ķϲ��ߣ�������һ���µĶϲ���
        {
            m_faults.Add( new CFault() );
            ( ( CFault* )m_faults[m_faults.GetUpperBound()] )->no = ( int )no;
            ( ( CFault* )m_faults[m_faults.GetUpperBound()] )->x.Add( x );
            ( ( CFault* )m_faults[m_faults.GetUpperBound()] )->y.Add( y );
            ( ( CFault* )m_faults[m_faults.GetUpperBound()] )->z.Add( 0 );
            ( ( ( CFault* )m_faults[m_faults.GetUpperBound()] )->iDots ) = 1;
        }
    }
    fclose( fp );
    /*	int k = m_faults.GetSize();
    	for (int L=0; L<k; L++)
    	{
    		int N = ((CFault *)m_faults[L])->x.GetSize();
    		for (int M=0; M<N; M++)
    		{
    			double xx = ((CFault *)m_faults[L])->x[M];
    			double yy = ((CFault *)m_faults[L])->y[M];
    			double nono =((CFault *)m_faults[L])->no;
    		}
    	}
    */
}


// �����еĶϲ��ߡ�
// �������bSmoothΪtrue,��ͬʱƽ���ϲ��ߣ�����ֱ�ӰѶϲ�����������һ��
void CTriangle::DrawFaults( bool bSmooth, CDC* dc )
{
    int faultsCount, xyCount;
    int i, k;
    CPoint* ptPoint;
    faultsCount = m_faults.GetSize();
    const int DofCircle = 500;

    CPen* thepen = new CPen( 0, 1, RGB( 255, 0, 0 ) );
    CPen* oldpen = dc->SelectObject( thepen );
    dc->SelectStockObject( HOLLOW_BRUSH );

    for (	i = 0; i < faultsCount; i++ )
    {
        xyCount = ( ( CFault* )m_faults[i] )->x.GetSize();
        ptPoint = new CPoint[xyCount];
        for ( k = 0; k < xyCount; k++ )
        {
            ptPoint[k].x = ( ( CFault* )m_faults[i] )->x[k] - xmin;
            ptPoint[k].y = ( ( CFault* )m_faults[i] )->y[k] - ymin;
            dc->Ellipse( ptPoint[k].x - DofCircle / 2, ptPoint[k].y - DofCircle / 2, ptPoint[k].x + DofCircle / 2, ptPoint[k].y + DofCircle / 2 );
        }
        dc->Polygon( ptPoint, xyCount );
        delete []ptPoint;
    }

    dc->SelectObject( oldpen );
    delete thepen;
}