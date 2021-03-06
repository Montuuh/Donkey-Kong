#include "Application.h"

#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModulePlayer.h"
#include "Level4.h"
#include "ModuleCollisions.h"
#include "ModuleRender.h"
#include "ModuleHammer.h"
#include "ModuleEnemies.h"
#include "ModuleFadeToBlack.h"
#include "SceneIntro.h"
#include "ModuleSceneWin.h"
#include "ModuleParticles.h"
#include "ModuleFonts.h"
#include "ModuleHud.h"
#include "ModuleDebugInfo.h"
#include "Level2.h"
#include "Level3.h"
#include "Level2Win.h"
#include "Level3Win.h"
Application::Application()
{
	// The order in which the modules are added is very important.
	// It will define the order in which Pre/Update/Post will be called
	// Render should always be last, as our last action should be updating the screen

	modules[0] = window = new ModuleWindow(true);
	modules[1] = input = new ModuleInput(true);
	modules[2] = textures = new ModuleTextures(true);
	modules[3] = audio = new ModuleAudio(true);

	modules[4] = intro = new SceneIntro(true);
	modules[5] = level4win = new Level4Win(false);
	modules[6] = level2win = new Level2Win(false);
	modules[7] = level3win = new Level3Win(false);


	modules[8] = level4 = new ModuleScene(false);
	modules[9] = level2 = new ModuleScene2(false);
	modules[10] = level3 = new ModuleScene3(false);

	modules[11] = player = new ModulePlayer(false);	//Player starts disabled
	modules[12] = particles = new ModuleParticles(true);//Gameplay scene starts disabled
	modules[13] = hammer = new ModuleHammer(false);
	modules[14] = enemies = new ModuleEnemies(false);	//Enemies start disabled

	modules[15] = collisions = new ModuleCollisions(true);
	modules[16] = fade = new ModuleFadeToBlack(true);
	modules[17] = fonts = new ModuleFonts(true);
	modules[18] = hud = new ModuleHud(true);
	modules[19] = debugInfo = new ModuleDebugInfo(true);
	modules[20] = render = new ModuleRender(true);
}

Application::~Application()
{
	for (int i = 0; i < NUM_MODULES; ++i)
	{
		//Important: when deleting a pointer, set it to nullptr afterwards
		//It allows us for null check in other parts of the code
		delete modules[i];
		modules[i] = nullptr;
	}
}

bool Application::Init()
{
	bool ret = true;

	// All modules (active and disabled) will be initialized
	for (int i = 0; i < NUM_MODULES && ret; ++i)
		ret = modules[i]->Init();

	// Only active modules will be 'started'
	for (int i = 0; i < NUM_MODULES && ret; ++i)
		ret = modules[i]->IsEnabled() ? modules[i]->Start() : true;

	return ret;
}

update_status Application::Update()
{
	update_status ret = update_status::UPDATE_CONTINUE;

	for (int i = 0; i < NUM_MODULES && ret == update_status::UPDATE_CONTINUE; ++i)
		ret = modules[i]->IsEnabled() ? modules[i]->PreUpdate() : update_status::UPDATE_CONTINUE;

	for (int i = 0; i < NUM_MODULES && ret == update_status::UPDATE_CONTINUE; ++i)
		ret = modules[i]->IsEnabled() ? modules[i]->Update() : update_status::UPDATE_CONTINUE;

	for (int i = 0; i < NUM_MODULES && ret == update_status::UPDATE_CONTINUE; ++i)
		ret = modules[i]->IsEnabled() ? modules[i]->PostUpdate() : update_status::UPDATE_CONTINUE;

	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;

	for (int i = NUM_MODULES - 1; i >= 0 && ret; --i)
		ret = modules[i]->IsEnabled() ? modules[i]->CleanUp() : true;

	return ret;
}
