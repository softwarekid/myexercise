#include "stdafx.h"

#include "../MineGE/DataLink.h"
#include "../MineGE/HelperClass.h"

#include "../ArxHelper/HelperClass.h"

#include "../VentNetworkAlgo/VNAlgoHelper.h"

class OTDataLink : public DataLink
{
public:
    OTDataLink(): ft( 0 ), needAir( false ) {}

protected:
    virtual void regDatas()
    {
        linkIntData( _T( "功能类型" ), &ft );
        linkBoolData( _T( "是否单独配风" ), &needAir );
        ft = ft % 6; // 0-5
    }

public:
    bool isNeedAir()
    {
        return ( needAir || ft == FT_ELECTRIC || ft == FT_DIESEL );
    }
    bool isMain()
    {
        return ( ft == FT_MAIN );
    }
    bool isSubMain()
    {
        return ( ft == FT_SUBMAIN );
    }

public:
    int ft;              // 巷道功能类型
    bool needAir;        // 是否需要单独配风
};

static void FilterGE( const AcDbObjectIdArray& objIds, const CString& type, AcDbObjectIdArray& filterObjIds )
{
    int len = objIds.length();
    for( int i = 0; i < len; i++ )
    {
        if( ArxUtilHelper::IsEqualType( type, objIds[i] ) )
        {
            filterObjIds.append( objIds[i] );
        }
    }
}

// 查找进回风井，全部包含在数组shafts
// 返回值表示进风井与回风井之间的分界点
int FindShafts( AcDbObjectIdArray& shafts )
{
    // 查找所有的源点的出边分支
    // 以及所有汇点的入边分支
    AcDbObjectIdArray objIds1, objIds2;
    ConnectivityHelper::FIOE( objIds1, objIds2 );

    AcDbObjectIdArray inShafts, outShafts;
    // 排除非巷道的分支，例如工作面、硐室、掘进工作面等
    FilterGE( objIds1, _T( "Tunnel" ), inShafts );
    FilterGE( objIds2, _T( "Tunnel" ), outShafts );

    shafts.append( inShafts );
    shafts.append( outShafts );
    return inShafts.length();
}

void FindTunnelsWithoutShaft( AcDbObjectIdArray& objIds )
{
    AcDbObjectIdArray shafts;
    int pos = FindShafts( shafts );

    AcDbObjectIdArray allObjIds;
    DrawHelper::FindMineGEs( _T( "Tunnel" ), allObjIds );

    int len = allObjIds.length();
    for( int i = 0; i < len; i++ )
    {
        if( !shafts.contains( allObjIds[i] ) )
        {
            objIds.append( allObjIds[i] );
        }
    }
}

// 查找需风巷道
static void FindUseAirTunnels( AcDbObjectIdArray& objIds )
{
    // 查找所有巷道(不包括井筒)
    AcDbObjectIdArray allObjIds;
    FindTunnelsWithoutShaft( allObjIds );

    bool ret = true;
    AcDbObjectIdArray tempObjIds;
    int len = allObjIds.length();
    for( int i = 0; i < len; i++ )
    {
        // 提取属性数据
        OTDataLink ot;
        ot.setDataSource( allObjIds[i] );
        if( !ot.updateData( false ) ) // 巷道没有相应的字段，提取数据失败
        {
            ret = false;
            break;
        }
        if( ot.isNeedAir() )
        {
            tempObjIds.append( allObjIds[i] );
        }
        else if( ot.isMain() )
        {
            tempObjIds.append( allObjIds[i] );
        }
        else if( ot.isSubMain() )
        {
            tempObjIds.append( allObjIds[i] );
        }
    }
    if( ret )	objIds.append( tempObjIds );
}

// 查找需要测风的地点
void FindUseAirPlaces( AcDbObjectIdArray& objIds, int& p1, int& p2, int& p3 )
{
    // 应测风地点包括：
    //    用风地点、主要巷道、采区进回风巷道
    AcDbObjectIdArray tempObjIds;

    DrawHelper::FindMineGEs( _T( "WorkSurface" ), tempObjIds ); // 工作面
    objIds.append( tempObjIds );
    p1 = objIds.length();

    tempObjIds.removeAll();

    DrawHelper::FindMineGEs( _T( "TTunnel" ), tempObjIds );  // 掘进工作面
    objIds.append( tempObjIds );
    p2 = objIds.length();

    tempObjIds.removeAll();

    DrawHelper::FindMineGEs( _T( "StorageGE" ), tempObjIds ); // 硐室
    objIds.append( tempObjIds );
    p3 = objIds.length();

    tempObjIds.removeAll();

    // 查找需要测风的巷道(不包括井筒)：
    //    1) 用风巷道
    //    2) 主要巷道和采区进回风巷道
    FindUseAirTunnels( tempObjIds );
    objIds.append( tempObjIds );

    //tempObjIds.removeAll();
}