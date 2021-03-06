#include "EventFilter.h"
#include "../common/CGraphicsDevice.h"
#include "EditorDrawer.h"
#include "EditorLevelIO.h"
#include "StateHandler.h"
#include "EditorState.h"
#include "editorapp.h"
#include "EditorSelection.h"

extern volatile bool Refresh;

CEventFilter::CEventFilter(CGraphicsDevice* aGD, CEditorDrawer* aDrawer, CEditorLevelIO* aLevelIO, CStateHandler* aStateHandler, CEditorSelection* aLevelSelection)
{
	iGD = aGD;
	iDrawer = aDrawer;
	iLevelIO = aLevelIO;
	iStateHandler = aStateHandler;
	iLevelSelection = aLevelSelection;
}

bool CEventFilter::HandleEvent(SDL_Event	 event)
{
	if ((SDL_EVENTMASK( event.type ) & SDL_MOUSEEVENTMASK))
		Refresh = true;
	if (event.type == SDL_KEYDOWN)
	{
		Refresh = true;
		switch (event.key.keysym.sym)
		{
			case SDLK_F3:
				if (iLevelIO->LoadLevel())
				{
					iDrawer->SetZoom(iDrawer->Zoom());
					iLevelSelection->Clear();
				}
				return true;
			break;
			case SDLK_F9:
				if (iLevelIO->LoadINI())
				{
					iDrawer->SetZoom(iDrawer->Zoom());
				}
				return true;
			break;
			case SDLK_F2: iLevelIO->SaveLevelAs();
				return true;
		}

		if (event.key.keysym.mod & KMOD_CTRL)
			if (event.key.keysym.sym == SDLK_s)
				{
					iLevelIO->SaveLevel();
					return true;
				}
	}
	if (event.type == SDL_VIDEORESIZE)
	{
		iGD->SetMode(event.resize.w,event.resize.h,KBitDepth,0,SDL_RESIZABLE);
		iDrawer->Resize(iGD->Width(),iGD->Height());
		Refresh = true;
		return true;
	}

	return false;
}
