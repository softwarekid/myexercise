#include "stdafx.h"

/* ���Դ���(��������ɾ��) */
#include "ContourHelper.h"

#include <fstream>
// ��Ҫ����<fstream>����ʹ��
#include "AcFStream.h"

#include "../ArxHelper/ArxUtilHelper.h"

void ReadDataFromFile( const CString& fileName, AcGePoint3dArray& datas )
{
    AcIfstream inFile( fileName );
    if( !inFile ) return;

    while( !inFile.eof() )
    {
        double x, y, z;
        inFile >> x >> y >> z;
        datas.append( AcGePoint3d( x, y, z ) );
    }
    inFile.close();
}

void ContourTest()
{
    AcGePoint3dArray pts;
    //ReadDataFromFile(_T("D:\\CONTOUR(300).DAT"), pts);
    ReadDataFromFile( _T( "D:\\testsite.txt" ), pts );
    //ReadDataFromFile(_T("D:\\testdata.txt"), pts);
    //ReadDataFromFile(_T("D:\\testdata2.txt"), pts);

    ContourHelper::DrawCountour2( pts, 3 );
    //ContourHelper::DrawBoundary(pts);
}