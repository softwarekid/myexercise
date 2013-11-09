#include "StdAfx.h"
#include "CadQuitEditorReactor.h"

#include "Utils.h"

CadQuitEditorReactor::CadQuitEditorReactor ( const bool autoInitAndRelease ) : AcEditorReactor(), mbAutoInitAndRelease( autoInitAndRelease )
{
    if( autoInitAndRelease )
    {
        if( acedEditor )
        {
            //acutPrintf(_T("\nLinkedGE_EditorReactor : %ld"), (long)acedEditor);
            acedEditor->addReactor ( this ) ;
        }
        else
            mbAutoInitAndRelease = false ;
    }
}

CadQuitEditorReactor::~CadQuitEditorReactor ()
{
    Detach () ;
}

void CadQuitEditorReactor::Attach ()
{
    Detach () ;
    if ( !mbAutoInitAndRelease )
    {
        if ( acedEditor )
        {
            acedEditor->addReactor ( this ) ;
            mbAutoInitAndRelease = true ;
        }
    }
}

void CadQuitEditorReactor::Detach ()
{
    if ( mbAutoInitAndRelease )
    {
        if ( acedEditor )
        {
            acedEditor->removeReactor ( this ) ;
            mbAutoInitAndRelease = false ;
        }
    }
}

AcEditor* CadQuitEditorReactor::Subject () const
{
    return ( acedEditor ) ;
}

bool CadQuitEditorReactor::IsAttached () const
{
    return ( mbAutoInitAndRelease ) ;
}


// 参考：关闭autocad时成功卸载CUI（触发CAD退出事件）
// http://www.objectarx.net/forum.php?mod=viewthread&tid=2759
void CadQuitEditorReactor::quitWillStart( void )
{
    AcEditorReactor::quitWillStart();

    /*
    // 由于触发此事件前文档已关闭，所以需通过模板重建，以便命令通够执行
     if(acDocManager->isApplicationContext())
     {
    	 acDocManager->appContextNewDocument(_T("acadiso.dwt"));
    	 AfxMessageBox(_T("新建文档"));

    	 //CAcModuleResourceOverride myResources;


    	 // 卸载cui文件
    	 //UnLoadPartialMenuUseCom(CUI_FILE_GROUP_NAME);
    	 bool ret = UnLoadPartialMenuUseCmd(CUI_FILE_GROUP_NAME);

    	 if(ret)
    	 {
    		 AfxMessageBox(_T("命令方式卸载成功"));
    	 }
    	 else
    	 {
    		 AfxMessageBox(_T("命令方式卸载失败"));
    	 }
    	 AfxMessageBox(_T("卸载完毕"));
     }
     */
}
