/***************************************************************************
 *   Copyright (C) 2008 by tyler shaub   *
 *   tyler.shaub@gmail.com   *
 *                                                                         *
 *   See COPYING	*
 ***************************************************************************/

#include "test3.hpp"

/*FLEX_REGISTER_TYPE(Flex_Test_::B2);
FLEX_REGISTER_TYPE(Flex_Test_::D2);
FLEX_REGISTER_TYPE(Flex_Test_::B1);*/

namespace Flex_Test_
{

    void test3()
    {
        B1* b1OfD2 = Flex_::Create<B1>( "Flex_Test_::D2" );

        Flex_::GetInfoFromObj( *b1OfD2 ).Print();

        b1OfD2->print();
        b1OfD2->vPrint();

        B1 const* cB1ofD2 = b1OfD2;
        //FIXME: Flex_::info_cast<B2 const>( cB1ofD2 )->print(); // cross-cast

        //FIXME: Flex_::info_cast<B1 const>( cB1ofD2 )->print(); // confusing-cast

        static_cast<D2*>( b1OfD2 )->print(); // static_cast works of course

        Flex_::Destroy( b1OfD2 );
    }

}
