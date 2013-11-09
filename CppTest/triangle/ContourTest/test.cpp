#include <iostream>
#include <string>
#include <fstream>
#include <iterator>
#include <algorithm>
using namespace std;

#include "../Contour/Contour.h"
#pragma comment(lib, "Contour")

// ��2���ո��ʾ�Ʊ���
#define TAB "  "

void ReadDataFromFile( const string& fileName, PointArray& pts )
{
    ifstream inFile( fileName.c_str() );
    if( !inFile ) return;

    double x, y, z;

    while( inFile >> x >> y >> z )
    {
        DT_Point p = {x, y, z};
        pts.push_back( p );
    }
    inFile.close();
}

static void GetMinMaxZValue( const PointArray& datas, double& minZ, double& maxZ )
{
    using namespace std;
    int len = datas.size();
    vector<double> zs;
    vector<double>::iterator itr;
    for( int i = 0; i < len; i++ )
    {
        zs.push_back( datas[i].z );
    }
    itr = min_element( zs.begin(), zs.end() );
    minZ = *itr;

    itr = max_element( zs.begin(), zs.end() );
    maxZ = *itr;
}

ostream& operator<<( ostream& os, const DT_Point& pt )
{
    return os << "(" << pt.x << ", " << pt.y << ", " << pt.z << ")" << endl;
}

ostream& operator<<( ostream& os, const DT_Contour& cnt )
{
    return os << "(zi=" << cnt.zi
           << ", cnpts_num=" << cnt.cnpts_num
           << ")" << endl;
}

//ostream& operator<<(ostream& os, const DT_Label& label)
//{
//	return os<<"��עλ��:("<<label.pt.x<<", "<<label.pt.y
//		     <<"),   ����Ƕ�:"<<label.angle<<endl;
//}

static void PrintContour( const DoubleArray& zValues, const PointArray& cnpts, const ContourArray& cna )
{
    // ���Ƶ�ֵ��
    for( int i = 0; i < ( int )cna.size(); i++ )
    {
        DT_Contour z_contour = cna[i];
        int zi = z_contour.zi;
        int cnpts_num = z_contour.cnpts_num;
        cout << "��" << i << "����ֵ��, z=" << zValues[z_contour.zi] << endl;

        // ��λ����i����ֵ��λ��
        int s = 0;
        for( int j = 0; j < i; j++ )
        {
            s += cna[j].cnpts_num;
        }
        int t = s + cna[i].cnpts_num;

        PointArray inPts, outPts;
        for( int j = s; j < t; j++ )
        {
            inPts.push_back( cnpts[j] );
            cout << cnpts[j] << "  ";
        }
        cout << endl;
        cout << "ʹ��������ֵ" << endl;
        SmoothPoints( inPts, outPts, 2 );
        //copy(outPts.begin(), outPts.end(), ostream_iterator<DT_Point>(cout));
        cout << endl;
    }
}

void test_label()
{
    double a[6][2] =
    {
        16473.7856,  -715.6180,
        16521.9323,  -689.5390,
        16597.6757,  -634.5149,
        17090.1478,  -853.9335,
        16448.0142,  -853.9335,
        16473.7856,  -715.618
    };

    PointArray cnpts;
    for( int i = 0; i < 6; i++ )
    {
        DT_Point p = {a[i][0], a[i][1], 0};
        cnpts.push_back( p );
    }

    PointArray lp;
    SearchLabelPostions( cnpts, lp );
}

//static void PrintLabel(const DoubleArray& zValues, const PointArray& cnpts, const ContourArray& cna, double tolerance)
//{
//	// ���Ƶ�ֵ��
//	for(int i=0;i<(int)cna.size();i++)
//	{
//		DT_Contour z_contour = cna[i];
//		int zi = z_contour.zi;
//		int cnpts_num = z_contour.cnpts_num;
//		cout<<"��"<<i<<"����ֵ��, z="<<zValues[z_contour.zi]<<endl;
//
//		// ��λ����i����ֵ��λ��
//		int s = 0;
//		for(int j=0;j<i;j++)
//		{
//			s += cna[j].cnpts_num;
//		}
//		int t = s + cna[i].cnpts_num;
//
//		PointArray inPts;
//		for(int j=s;j<t;j++)
//		{
//			inPts.push_back(cnpts[j]);
//		}
//		LabelArray la;
//		SearchLabelPostion(inPts, tolerance, 38.527, la);
//
//		copy(la.begin(), la.end(), ostream_iterator<DT_Label>(cout));
//		cout<<endl;
//	}
//}

static void PrintLabel( const DoubleArray& zValues, const PointArray& cnpts, const ContourArray& cna )
{
    // ���Ƶ�ֵ��
    for( int i = 0; i < ( int )cna.size(); i++ )
    {
        DT_Contour z_contour = cna[i];
        int zi = z_contour.zi;
        int cnpts_num = z_contour.cnpts_num;
        cout << "��" << i << "����ֵ��, z=" << zValues[z_contour.zi] << endl;

        // ��λ����i����ֵ��λ��
        int s = 0;
        for( int j = 0; j < i; j++ )
        {
            s += cna[j].cnpts_num;
        }
        int t = s + cna[i].cnpts_num;

        PointArray inPts;
        for( int j = s; j < t; j++ )
        {
            inPts.push_back( cnpts[j] );
        }
        if( inPts.empty() ) continue;

        PointArray tpts;
        SearchLabelPostions( inPts, tpts );
        for( int k = 0; k < tpts.size(); k++ )
        {
            cout << "\t" << tpts[k];
        }
        cout << endl;
    }
}

void SplitRange( double minZ, double maxZ, double dz, DoubleArray& zValues )
{
    const double eplison = 0.001;
    //double dz = (maxZ-minZ)/n;
    for( double z = minZ; ( maxZ + eplison - z ) > 0; z += dz )
    {
        zValues.push_back( z );
    }
}

int main()
{
    string path = "testdata2.txt";
    //string path = "testdata.txt";
    //string path = "testsite.txt";
    PointArray pts;
    ReadDataFromFile( path, pts );

    PointArray bounds;
    string path2 = "bound.txt";
    ReadDataFromFile( path2, bounds );

    DoubleArray zValues;
    //zValues.push_back(1.24);
    //zValues.push_back(1.27);
    //zValues.push_back(1.30);

    //zValues.push_back(4617);
    //zValues.push_back(4649);
    //zValues.push_back(4681);

    double minZ, maxZ, dz;
    //GetMinMaxZValue(pts, minZ, maxZ);
    // dz = (maxZ - minZ)/5;
    minZ = 0;
    maxZ = 6;
    dz = 1;

    SplitRange( minZ, maxZ, dz, zValues );

    // ���������ʷ�
    EdgeArray segments;
    PointArray holes;
    EdgeArray ea;
    TriangleArray ta;
    CDT_LIB( pts, segments, holes, ea, ta );

    // ׷�ٵ�ֵ��
    PointArray cnpts;
    ContourArray cna;
    TraceContours( pts, ea, ta, zValues, cnpts, cna );

    cout << "zֵ����:" << zValues.size() << endl;
    copy( zValues.begin(), zValues.end(), ostream_iterator<double>( cout, TAB ) );
    cout << endl;

    cout << "��ֵ�߸���:" << cna.size() << endl;
    copy( cna.begin(), cna.end(), ostream_iterator<DT_Contour>( cout, TAB ) );
    cout << endl;

    //PrintContour(zValues, cnpts, cna);

    //cout<<"��ֵ���б�: "<<cnpts.size()<<endl;
    //cout<<fixed;
    //copy(cnpts.begin(), cnpts.end(), ostream_iterator<DT_Point>(cout));
    //cout<<endl;

    // ���������Χ�߽�
    PointArray bpts;
    SearchBoundary( pts, ea, ta, bpts );
    cout << "��ӡ�߽�:" << endl;
    std::copy( bpts.begin(), bpts.end(), std::ostream_iterator<DT_Point>( cout, TAB ) );
    cout << endl;

    // ������ֵ������
    PointArray clip_cnpts;
    ContourArray clip_cna;

    //Clip(cnpts, cna, bpts, all_cnpts, all_cna);
    Clip( cnpts, cna, bounds, clip_cnpts, clip_cna );

    PointArray all_cnpts;
    ContourArray all_cna;
    IntArray sortPos;
    //EncloseContour_BuildTopolgy(cnpts, cna, bounds, all_cnpts, all_cna, sortPos);
    EncloseContour_BuildTopolgy( clip_cnpts, clip_cna, bounds, all_cnpts, all_cna, sortPos );

    for( int i = 0; i < all_cnpts.size(); i++ )
    {
        printf( "\n%.3f, %.3f", all_cnpts[i].x, all_cnpts[i].y );
    }
    for( int i = 0; i < all_cna.size(); i++ )
    {
        printf( "\nzi=%d, cnpts_num=%d, bpts_num=%d", all_cna[i].zi, all_cna[i].cnpts_num, all_cna[i].bpts_num );
    }

    cout << "��ӡ����˳��" << endl;
    std::copy( sortPos.begin(), sortPos.end(), std::ostream_iterator<int>( cout, TAB ) );
    cout << endl;

    // ���ұ�עλ��
    //PrintLabel(zValues, cnpts, cna, 20);
    PrintLabel( zValues, cnpts, cna );
    return 0;
}