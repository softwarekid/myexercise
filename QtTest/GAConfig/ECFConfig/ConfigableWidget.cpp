#include "ConfigableWidget.h"
#include <QString>

ConfigableWidget::ConfigableWidget( QWidget* parent ) : QWidget( parent )
{

}

static TiXmlNode* WriteEntry( const char* name, const char* value )
{
    if( name == 0 ) return 0;
    TiXmlElement* entry_node = new TiXmlElement( "Entry" );
    entry_node->SetAttribute( "Key", name );
    entry_node->LinkEndChild( new TiXmlText( ( value == 0 ) ? "" : value ) );
    return entry_node;
}

TiXmlNode* WriteStringEntry( const char* name, const char* value )
{
    return WriteEntry( name, value );
}

TiXmlNode* WriteIntEntry( const char* name, int value )
{
    return WriteEntry( name, QString::number( value ).toAscii().data() );
}

TiXmlNode* WriteDoubleEntry( const char* name, double value )
{
    return WriteEntry( name, QString::number( value ).toAscii().data() );
}
