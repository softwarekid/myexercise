#include "AlgoSection.h"
#include "AlgoParams.h"

AlgoSection::AlgoSection( QWidget* parent ) : ConfigableWidget( parent )
{
    algosCombox = new QComboBox;
    algosCombox->addItem( tr( "SteadyStateTournament" ) );
    algosCombox->addItem( tr( "RouletteWheel" ) );
    algosCombox->addItem( tr( "Elimination" ) );
    algosCombox->setCurrentIndex( 0 );

    pagesWidget = new QStackedWidget;
    pagesWidget->addWidget( new SteadyStateTournamentParams );
    pagesWidget->addWidget( new RouletteWheelParams );
    pagesWidget->addWidget( new EliminationParams );

    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addWidget( algosCombox );
    mainLayout->addWidget( pagesWidget );
    setLayout( mainLayout );

    connect( algosCombox, SIGNAL( activated( int ) ),
             pagesWidget, SLOT( setCurrentIndex( int ) ) );
}

TiXmlNode* AlgoSection::writeConfNode()
{
    ConfigableWidget* cw = ( ConfigableWidget* )pagesWidget->currentWidget();
    TiXmlElement* node = new TiXmlElement( "Algorithm" );
    node->LinkEndChild( cw->writeConfNode() );
    return node;
}