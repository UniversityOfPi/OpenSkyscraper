#include "tools.h"

#include "../game.h"
#include "../../tower/tower.h"

using namespace OSS;
using namespace Classic;





//----------------------------------------------------------------------------------------------------
#pragma mark -
#pragma mark Construction
//----------------------------------------------------------------------------------------------------

ToolsUI::ToolsUI(GameUI * ui) : Responder(), ui(ui)
{
	//Initialize the construction tool
	constructionTool = new ConstructionTool(this);
	
	//Make sure that needs for updates get propagated to us
	constructionTool->updateIfNeeded.parent = &updateIfNeeded;
	
	//Use the construction tool by default
	setTool(constructionTool);
}

Tower * ToolsUI::getTower()
{
	return ui->getTower();
}

GameScene * ToolsUI::getScene()
{
	return ui->scene;
}





//----------------------------------------------------------------------------------------------------
#pragma mark -
#pragma mark Tools
//----------------------------------------------------------------------------------------------------

Tool * ToolsUI::getTool()
{
	return tool;
}

void ToolsUI::setTool(Tool * tool)
{
	if (this->tool != tool) {
		this->tool = tool;
	}
}





//----------------------------------------------------------------------------------------------------
#pragma mark -
#pragma mark Engine Object
//----------------------------------------------------------------------------------------------------

void ToolsUI::update()
{
	//Update the current tool if there is one
	if (tool)
		tool->updateIfNeeded();
}

void ToolsUI::draw(rectd dirtyRect)
{
	//Draw the current tool if there is one
	if (tool)
		tool->draw(dirtyRect);
}





//----------------------------------------------------------------------------------------------------
#pragma mark -
#pragma mark Event Sending
//----------------------------------------------------------------------------------------------------

bool ToolsUI::sendEventToNextResponders(Base::Event * event)
{
	if (tool && tool->sendEvent(event)) return true;
	return Responder::sendEventToNextResponders(event);
}
