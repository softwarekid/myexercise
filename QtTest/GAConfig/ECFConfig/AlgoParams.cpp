#include "AlgoParams.h"

AlgoParams::AlgoParams( QWidget* parent ) : ConfigableWidget( parent )
{

}

SteadyStateTournamentParams::SteadyStateTournamentParams( QWidget* parent ) : AlgoParams( parent )
{
    tsizeSpinBox = new QSpinBox;
    tsizeSpinBox->setRange( 2, 100 );
    tsizeSpinBox->setSingleStep( 1 );
    tsizeSpinBox->setValue( 3 );

    QFormLayout* formLayout = new QFormLayout;
    formLayout->addRow( tr( "tsize" ), tsizeSpinBox );

    setLayout( formLayout );
}

RouletteWheelParams::RouletteWheelParams( QWidget* parent ) : AlgoParams( parent )
{
    crxprobSpinBox = new QDoubleSpinBox;
    crxprobSpinBox->setRange( 0.0, 1.0 );
    crxprobSpinBox->setSingleStep( 0.01 );
    crxprobSpinBox->setValue( 0.5 );

    selpressureSpinBox = new QDoubleSpinBox;
    selpressureSpinBox->setRange( 1, 100.0 );
    selpressureSpinBox->setSingleStep( 0.1 );
    selpressureSpinBox->setValue( 10.0 );
    // 选择压力：最佳个体选中的概率与平均个体选中概率的比值
    selpressureSpinBox->setToolTip( tr( "selection pressure always more than one" ) );

    QFormLayout* formLayout = new QFormLayout;
    formLayout->addRow( tr( "crxprob" ), crxprobSpinBox );
    formLayout->addRow( tr( "selpressure" ), selpressureSpinBox );

    setLayout( formLayout );
}

EliminationParams::EliminationParams( QWidget* parent ) : AlgoParams( parent )
{
    gengapSpinBox = new QDoubleSpinBox;
    gengapSpinBox->setRange( 0.0, 1.0 );
    gengapSpinBox->setSingleStep( 0.01 );
    gengapSpinBox->setValue( 0.6 );

    selpressureSpinBox = new QDoubleSpinBox;
    selpressureSpinBox->setRange( 0.0, 100.0 );
    selpressureSpinBox->setSingleStep( 0.1 );
    selpressureSpinBox->setValue( 10.0 );

    QFormLayout* formLayout = new QFormLayout;
    formLayout->addRow( tr( "gengap" ), gengapSpinBox );
    formLayout->addRow( tr( "selpressure" ), selpressureSpinBox );

    setLayout( formLayout );
}

TiXmlNode* SteadyStateTournamentParams::writeConfNode()
{
    TiXmlElement* node = new TiXmlElement( "SteadyStateTournament" );
    node->LinkEndChild( WriteIntEntry( "tsize", tsizeSpinBox->value() ) );
    return node;
}

TiXmlNode* RouletteWheelParams::writeConfNode()
{
    TiXmlElement* node = new TiXmlElement( "RouletteWheel" );
    node->LinkEndChild( WriteDoubleEntry( "crxprob", crxprobSpinBox->value() ) );
    node->LinkEndChild( WriteDoubleEntry( "selpressure", selpressureSpinBox->value() ) );
    return node;
}

TiXmlNode* EliminationParams::writeConfNode()
{
    TiXmlElement* node = new TiXmlElement( "Elimination" );
    node->LinkEndChild( WriteDoubleEntry( "gengap", gengapSpinBox->value() ) );
    node->LinkEndChild( WriteDoubleEntry( "selpressure", selpressureSpinBox->value() ) );
    return node;
}