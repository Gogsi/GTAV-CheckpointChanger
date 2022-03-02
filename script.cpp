/*
	THIS FILE IS A PART OF GTA V SCRIPT HOOK SDK
				http://dev-c.com
			(C) Alexander Blade 2015
*/

#include "script.h"
#include "utils.h"
#include "keyboard.h"
#include <vector>
#include <chrono>

using namespace std::chrono;

UINT64* currentCheckpoint;
UINT64 oldCheckpoint = 0;
UINT64 desiredCheckpoint = 0;

struct TextDrawInfo {
	std::string text;
	int red;
	int green;
	int blue;
	milliseconds endTime;
};

typedef std::vector<TextDrawInfo> TextDrawInfoList;
TextDrawInfoList g_TextDrawInfos;

void AddTextDraw(const std::string text, UINT64 displayTimeInMilliSeconds, int red = 255, int green = 255, int blue = 255) {
	TextDrawInfo info; 
	info.text = text;  
	info.red = red;
	info.green = green;
	info.blue = blue;
	info.endTime = duration_cast<milliseconds>(
		system_clock::now().time_since_epoch()
		) + milliseconds(displayTimeInMilliSeconds);
	g_TextDrawInfos.push_back(info); 
}

void update()
{
	Player player = PLAYER::PLAYER_ID();
	Ped playerPed = PLAYER::PLAYER_PED_ID();


	if (*currentCheckpoint != oldCheckpoint) {
		// Checkpoint has been changed, reset
		desiredCheckpoint = *currentCheckpoint;
	}

	if (IsKeyJustUp(VK_OEM_MINUS) && desiredCheckpoint > 0) {
		desiredCheckpoint--;
		AddTextDraw("Selected checkpoint " + std::to_string(desiredCheckpoint) + ". Press \"0\" to apply.", 1200);
	}
	if (IsKeyJustUp(VK_OEM_PLUS)) {
		desiredCheckpoint++;
		AddTextDraw("Selected checkpoint " + std::to_string(desiredCheckpoint) + ". Press \"0\" to apply.", 1200);
	}

	if (IsKeyJustUp(0x30)) { // '0' key
		*currentCheckpoint = desiredCheckpoint;
		AddTextDraw("Going to checkpoint " + std::to_string(desiredCheckpoint), 3000);
		ENTITY::SET_ENTITY_HEALTH(playerPed, 0, 0);
	}

	oldCheckpoint = *currentCheckpoint;


	// Draw text
	TextDrawInfoList::iterator it;// Draw it
	float x = 0.5;
	float y = 0.5;
	for(it = g_TextDrawInfos.begin(); it != g_TextDrawInfos.end(); it++){  
		TextDrawInfo info = *it;
		HUD::SET_TEXT_FONT(0);
		HUD::SET_TEXT_SCALE(0.4f, 0.4f);
		HUD::SET_TEXT_COLOUR(info.red, info.blue, info.green, 255);
		HUD::SET_TEXT_CENTRE(true);
		HUD::SET_TEXT_DROPSHADOW(3, 0, 0, 0, 255);
		HUD::BEGIN_TEXT_COMMAND_DISPLAY_TEXT("STRING");
		HUD::ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(info.text.c_str());
		HUD::END_TEXT_COMMAND_DISPLAY_TEXT(x, y, 0);
		y += 0.03f;
	}

	// Remove expired text
	it = g_TextDrawInfos.begin(); 
	while (it != g_TextDrawInfos.end()) { 
		TextDrawInfo info = *it;

		auto timeNowMs = duration_cast<milliseconds>(
			system_clock::now().time_since_epoch()
		);

		if (timeNowMs > info.endTime) {
			g_TextDrawInfos.erase(it);     
			it = g_TextDrawInfos.begin(); 
		} 
		else { 
			it++; 
		} 
	}
}

void main()
{
	//Wait for ingame.
	while (CAM::IS_SCREEN_FADED_OUT()) {
		WAIT(100);
	}

	AddTextDraw("Checkpoint Changer Mod v1.0 loaded!", 5000, 255, 10, 10);
	AddTextDraw("Press \"+\" or \"-\" while on a mission to adjust checkpoint and \"0\" to apply.", 5000);
	currentCheckpoint = getGlobalPtr(89999);
	while (true)
	{
		update();
		WAIT(0);
	}
}

void ScriptMain()
{
	main();
}
