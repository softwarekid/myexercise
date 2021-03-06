#include "StdAfx.h"
#include "AFCTest.h"

#include "../AFC/AFC.h"
#include "../DaoHelper/TableCreator.h"
#include "../DaoHelper/DataWriter.h"
using namespace PJ;

#include "FilePathHelper.h"

void AFCTest::ws_afc_test()
{
    CDaoDatabase* pDB = CreateDB( BuildPath( GetAppPathDir(), _T( "afc.mdb" ) ), false );

    TableCreator* pTableCreator = new TableCreator( pDB );
    DataWriter* pDataWriter = new DataWriter( pDB );

    WS_AFC ws_afc;
    ws_afc.initMC_AFCStrategy( 6, 2, 3.2, 3.5, 1.1, 1.1, 1.1 );
    ws_afc.initMaxPeopleNum_AFC_CheckStrategy( 120 );
    ws_afc.initMaxWeight_AFC_CheckStrategy( 10 );
    ws_afc.initTve_AFCStrategy( 3.6, 17 );
    ws_afc.initVE_AFC_CheckStrategy( 17 );
    ws_afc.doAFC( pTableCreator, pDataWriter );

    // 再执行一次
    ws_afc.initGas_AFCStrategy( 13, 1.3 );
    ws_afc.initMaxPeopleNum_AFC_CheckStrategy( 125 );
    ws_afc.initMaxWeight_AFC_CheckStrategy( 12 );
    ws_afc.initTve_AFCStrategy( 3.6, 18 );
    ws_afc.initVE_AFC_CheckStrategy( 18 );
    ws_afc.doAFC( pTableCreator, pDataWriter );

    // 再执行一次
    ws_afc.initGas_AFCStrategy( 13, 1.3, 10 );
    ws_afc.initMaxPeopleNum_AFC_CheckStrategy( 125 );
    ws_afc.initMaxWeight_AFC_CheckStrategy( 12 );
    ws_afc.initTve_AFCStrategy( 3.6, 18 );
    ws_afc.initVE_AFC_CheckStrategy( 18, true );
    ws_afc.doAFC( pTableCreator, pDataWriter );

    delete pTableCreator;
    delete pDataWriter;
    pDB->Close();
    delete pDB;
}

void AFCTest::tws_afc_test()
{
    CDaoDatabase* pDB = CreateDB( BuildPath( GetAppPathDir(), _T( "afc.mdb" ) ), false );

    TableCreator* pTableCreator = new TableCreator( pDB );
    DataWriter* pDataWriter = new DataWriter( pDB );

    TWS_AFC tws_afc;
    tws_afc.initGas_AFCStrategy( 13, 1.3 );
    tws_afc.initLocalFan_AFCStrategy( 20, 2, 300 );
    tws_afc.initMaxPeopleNum_AFC_CheckStrategy( 125 );
    tws_afc.initMaxWeight_AFC_CheckStrategy( 12 );
    tws_afc.initVE_AFC_CheckStrategy( 20 );
    tws_afc.doAFC( pTableCreator, pDataWriter );

    // 再执行一次
    tws_afc.initGas_AFCStrategy( 13, 1.3 );
    tws_afc.initLocalFan_AFCStrategy( 20, 2, 300, false );
    tws_afc.initMaxPeopleNum_AFC_CheckStrategy( 125 );
    tws_afc.initMaxWeight_AFC_CheckStrategy( 12 );
    tws_afc.initVE_AFC_CheckStrategy( 20, false );
    tws_afc.doAFC( pTableCreator, pDataWriter );

    delete pTableCreator;
    delete pDataWriter;
    pDB->Close();
    delete pDB;
}
void AFCTest::dhc_afc_test()
{
    CDaoDatabase* pDB = CreateDB( BuildPath( GetAppPathDir(), _T( "afc.mdb" ) ), false );

    TableCreator* pTableCreator = new TableCreator( pDB );
    DataWriter* pDataWriter = new DataWriter( pDB );

    DHC_AFC dhc_afc;
    dhc_afc.initUEM_AFCStrategy( 100 );
    dhc_afc.initConstMinQ_AFC_CheckStrategy();
    dhc_afc.doAFC( pTableCreator, pDataWriter );

    // 再执行一次
    dhc_afc.initUEM_AFCStrategy( 300 );
    dhc_afc.initConstMinQ_AFC_CheckStrategy();
    dhc_afc.doAFC( pTableCreator, pDataWriter );

    dhc_afc.initPCR_AFCStrategy( 0.3 );
    dhc_afc.initConstMinQ_AFC_CheckStrategy();
    dhc_afc.doAFC( pTableCreator, pDataWriter );


    dhc_afc.initPCR_AFCStrategy( 0.7 );
    dhc_afc.initConstMinQ_AFC_CheckStrategy();
    dhc_afc.doAFC( pTableCreator, pDataWriter );

    dhc_afc.initLargeMEC_AFCStrategy( 1000, 0.03, 5 );
    dhc_afc.doAFC( pTableCreator, pDataWriter );

    dhc_afc.initSmallMEC_AFCStrategy( 500, 70 );
    dhc_afc.doAFC( pTableCreator, pDataWriter );

    delete pTableCreator;
    delete pDataWriter;
    pDB->Close();
    delete pDB;
}

void AFCTest::ot_afc_test()
{
    CDaoDatabase* pDB = CreateDB( BuildPath( GetAppPathDir(), _T( "afc.mdb" ) ), false );

    TableCreator* pTableCreator = new TableCreator( pDB );
    DataWriter* pDataWriter = new DataWriter( pDB );

    OT_AFC ot_afc;
    ot_afc.initOT_Gas_AFCStrategy( 3, LOW_GAS, ROCK );
    ot_afc.initVE_AFC_CheckStrategy( 20, true );
    ot_afc.initRTC_AFCStrategy( 2, 300 );
    ot_afc.doAFC( pTableCreator, pDataWriter );

    ot_afc.initOT_Gas_AFCStrategy( 3, LOW_GAS, ROCK );
    ot_afc.initEl_VE_AFC_CheckStrategy( 21 );
    ot_afc.doAFC( pTableCreator, pDataWriter );

    delete pTableCreator;
    delete pDataWriter;
    pDB->Close();
    delete pDB;
}

void AFCTest::all_afc_test()
{
    //AirFlowCaculate afc;
    //afc.setMineGasState(true); // 设置矿井为高瓦斯矿井
    //afc.test(BuildPath(GetAppPathDir(), _T("afc.mdb")));
}