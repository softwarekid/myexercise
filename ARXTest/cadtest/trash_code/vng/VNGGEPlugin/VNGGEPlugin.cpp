#include "StdAfx.h"
#include "VNGGEPlugin.h"

#include "VNG_NodeSimpleDraw.h"
#include "VNG_EdgeSimpleDraw.h"

void VNGGEPlugin::initialize( MineGEDrawManager* pGEDrawManager )
{
    BEGIN_REGISTER_MINEGE_DRAW( MineGEDrawManager, pGEDrawManager )

    REGISTER_MINEGE_DRAW( VNG_Node, VNG_NodeSimpleDraw )
    REGISTER_MINEGE_DRAW( VNG_Edge, VNG_EdgeSimpleDraw )

    END_REGISTER_MINEGE_DRAW
}

void VNGGEPlugin::unInitialize( MineGEDrawManager* pGEDrawManager )
{
    BEGIN_UNREGISTER_MINEGE_DRAW( MineGEDrawManager, pGEDrawManager )

    UNREGISTER_MINEGE_DRAW( VNG_Node, VNG_NodeSimpleDraw )
    UNREGISTER_MINEGE_DRAW( VNG_Edge, VNG_EdgeSimpleDraw )

    END_UNREGISTER_MINEGE_DRAW
}

// 插件创建函数定义实现并导出
MINEGE_PLUGIN_CREATE_FUNC_IMPL( VNGGEPlugin )
