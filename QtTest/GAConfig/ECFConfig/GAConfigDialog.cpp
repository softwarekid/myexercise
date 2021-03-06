#include "GAConfigDialog.h"

GAConfigDialog::GAConfigDialog()
{
    algoSection = new AlgoSection;
    regSection = new RegistrySection;
    genoSection = new GenotypeSection;

    QPushButton* confButton = new QPushButton( tr( "Generate Config File" ) );
    confButton->setFixedHeight( 50 );

    QGridLayout* gridLayout = new QGridLayout;
    gridLayout->addWidget( genoSection, 0, 0, 2, 1 );
    gridLayout->addWidget( algoSection, 0, 1 );
    gridLayout->addWidget( regSection, 1, 1 );
    gridLayout->addWidget( confButton, 2, 0, 1, 2 );
    setLayout( gridLayout );

    setWindowTitle( tr( "GA Config Dialog" ) );

    connect( confButton, SIGNAL( clicked() ), this, SLOT( writeConfigFile() ) );
}

void GAConfigDialog::writeConfigFile()
{
    TiXmlDocument doc;

    TiXmlElement* root = new TiXmlElement( "ECF" );
    doc.LinkEndChild( root );

    root->LinkEndChild( algoSection->writeConfNode() );
    root->LinkEndChild( regSection->writeConfNode() );
    root->LinkEndChild( genoSection->writeConfNode() );

    bool ret = doc.SaveFile( "parameters.txt" );

    QMessageBox msgBox;
    msgBox.setText( ret ? tr( "write config file successfully" ) : tr( "fail to write config file" ) );
    msgBox.exec();
}