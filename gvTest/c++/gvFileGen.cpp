#include <iostream>
#include <fstream>
#include <string>
using namespace std;

void WriteDotFile( const string& inFile, bool needEdgeNum )
{
    ifstream infile( inFile.c_str() );
    if( !infile )
    {
        // 打开文件失败
        infile.close();
        return ;
    }

    ofstream outfile( "gd.gv" );
    // 写入图的类型声明以及图的名称G
    outfile << "digraph G {\n";
    // 通过ratio和size参数调整图形的比例
    // 不过在测试中发现size不太好确定，容易导致dot挂掉
    // 暂时只用ratio控制ratio=height/widht
    //outfile<<"graph[rankdir=BT, ratio=1.333];\n";
    outfile << "\tgraph[rankdir=BT];\n";

    while( !infile.eof() )
    {
        int edge, start_node, end_node;
        infile >> edge >> start_node >> end_node;
        outfile << "\t"
                << "v" << start_node
                << "->"
                << "v" << end_node;

        if( needEdgeNum )
        {
            outfile << " [label=\"e" << edge << "\"]";
        }

        outfile << ";\n";
    }

    outfile << "}";
    outfile.close();
    infile.close();
}

int main( int argc, char* argv[] )
{
    if( argc != 3 )
    {
        cout << "使用说明: gvFileGen.exe graphFile [-e]\n"
             << "    (1) graphFile -- 拓扑数据txt文件\n"
             << "    (2) e         -- 需要显示分支编号\n"
             << "        如果用户不输入该参数，那么默认情况下不显示分支编号";
        return 0;
    }
    //将全局区域设为操作系统默认区域，以支持中文路径
    locale::global( std::locale( "" ) );

    // 生成dot文件
    WriteDotFile( argv[1], strcmp( argv[2], "-e" ) == 0 );

    //恢复全局locale，如果不恢复，可能导致cout无法输出中文
    setlocale( LC_ALL, "C" );

    return 0;
}
