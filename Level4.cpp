#include "Level4.h"

#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleRender.h"
#include "ModuleAudio.h"
#include "ModuleCollisions.h"
#include "ModuleEnemies.h"
#include "ModulePlayer.h"
#include "ModuleHammer.h"
#include "ModuleFadeToBlack.h"
#include "ModuleInput.h"
#include "ModuleHud.h"
#include "ModuleParticles.h"
#include "SceneIntro.h"
#include "ModuleFonts.h"


#include "Game/SDL_mixer/include/SDL_mixer.h"
#include "Game/SDL/include/SDL_scancode.h"


#include "Game/SDL/include/SDL.h"
#include "Game/SDL_mixer/include/SDL_mixer.h"
#pragma comment( lib, "Game/SDL_mixer/libx86/SDL2_mixer.lib" )

ModuleScene::ModuleScene(bool startEnabled) : Module(startEnabled)
{
	// Level 4 scene sprite
	level_4.x = 472;
	level_4.y = 160;
	level_4.w = SCREEN_WIDTH;
	level_4.h = SCREEN_HEIGHT;


	dkAnimIdle.PushBack({ 256, 56, 40, 32 });
	dkRightHand.PushBack({ 357, 56, 46, 32 });
	dkLeftHand.PushBack({ 304, 56, 46, 32 });
	dkAnimIdle.speed = 0.1f;
	dkRightHand.speed = 0.1f;
	dkLeftHand.speed = 0.1f;

	donkeyPath.PushBack({ 0,0, }, 120, &dkAnimIdle);
	donkeyPath.PushBack({ 0,0, }, 30, &dkRightHand);
	donkeyPath.PushBack({ 0,0, }, 30, &dkLeftHand);
	donkeyPath.PushBack({ 0,0, }, 30, &dkRightHand);
	donkeyPath.loop = true;

	prAnimRight.PushBack({ 96,103,15,22 });
	prAnimRight.PushBack({ 121,103,15,22 });
	
	prAnimRight.speed = 0.1f;
	prAnimRight.loop = true;
	prAnimRightIdle.PushBack({ 143,103,16,22 });

	prAnimLeft.PushBack({ 96,131,15,22 });
	prAnimLeft.PushBack({ 121,131,15,22 });
	
	prAnimLeft.speed = 0.1f;
	prAnimLeft.loop = true;
	prAnimLeftIdle.PushBack({ 143,131,16,22 });

	princessPathRight.PushBack({ 0,0 }, 150, &prAnimRightIdle);
	princessPathRight.PushBack({ 0,0 }, 60, &prAnimRight);
	princessPathRight.loop = true;

	princessPathLeft.PushBack({ 0,0 }, 150, &prAnimLeftIdle);
	princessPathLeft.PushBack({ 0,0 }, 60, &prAnimLeft);
	princessPathRight.loop = true;

	helpRight = {280,108,24,8};
	helpLeft = {248,108,23,8};
}

ModuleScene::~ModuleScene()
{
	
}

// Load assets
bool ModuleScene::Start()
{
	App->hud->drawScore = true;

	LOG("Loading background assets");

	bool ret = true;
	// Scene sprites
	bgTexture = App->textures->Load("Assets/Background2.png");
	++activeTextures; ++totalTextures;

	level_4BGM = App->audio->PlayMusic("Assets/Music/Stage4BGM.ogg");
	
	FX_Win = App->audio->LoadFx("Assets/Music/Stage_Clear_2.wav");
	++activeFx; ++totalFx;

	FX_Lose = App->audio->LoadFx("Assets/Music/SFX_Death.wav");
	++activeFx; ++totalFx;

	FX_Stomp = App->audio->LoadFx("Assets/Music/SFX_Stomp.wav");
	++activeFx; ++totalFx;
	
	App->particles->Enable();
	App->collisions->Enable();
	App->player->Enable();
	App->hammer->Enable();
	if (App->hud->lives != 3) App->hammer->Start();
	App->enemies->Enable();

	//Starting position of the Mario
	App->player->position.x = 0;
	App->player->position.y = 232;

	Nuts = 8;
	frameCount = 0;
	resetCounter = 0;
	donkeyCounterFX = 0;
	helpCounter = 0;
	App->player->destroyed = false;

	// Level 4 colliders:
	App->collisions->AddCollider({ 0, 248, 224, 8 }, Collider::Type::GROUND); // Base

	App->collisions->AddCollider({ 7, 208, 1, 8 }, Collider::Type::GROUND); // Floor 1
	// LEFT LADDER
	App->collisions->AddCollider({ 17, 208, 39, 8 }, Collider::Type::GROUND); // Floor 1
	App->collisions->AddCollider({ 64, 208, 48-2-7, 8}, Collider::Type::GROUND); // Floor 1 
	// MID LADDER
	App->collisions->AddCollider({ 64+49, 208, 48-1, 8 }, Collider::Type::GROUND); // Floor 1
	App->collisions->AddCollider({ 168, 208, 39, 8 }, Collider::Type::GROUND); // Floor 1
	// RIGHT LADDER
	App->collisions->AddCollider({ 217, 208, 1, 8 }, Collider::Type::GROUND); // Floor 1

	App->collisions->AddCollider({ 15, 168, 1, 8 }, Collider::Type::GROUND); // Floor 2
	// LEFT LADDER
	App->collisions->AddCollider({ 25, 168, 31, 8 }, Collider::Type::GROUND); // Floor 2
	App->collisions->AddCollider({ 64, 168, 7, 8 }, Collider::Type::GROUND); // Floor 2
	// MID LADDER
	App->collisions->AddCollider({ 81, 168, 62, 8 }, Collider::Type::GROUND); // Floor 2
	// MID LADDER
	App->collisions->AddCollider({ 153, 168, 7, 8 }, Collider::Type::GROUND); // Floor 2
	App->collisions->AddCollider({ 168, 168, 31, 8 }, Collider::Type::GROUND); // Floor 2
	// RIGHT LADDER
	App->collisions->AddCollider({ 209, 168, 1, 8 }, Collider::Type::GROUND); // Floor 2

	App->collisions->AddCollider({ 23, 128, 1, 8 }, Collider::Type::GROUND); // Floor 3
	// LEFT LADDER
	App->collisions->AddCollider({ 33, 128, 23, 8  }, Collider::Type::GROUND); // Floor 3
	App->collisions->AddCollider({ 64, 128, 39, 8 }, Collider::Type::GROUND); // Floor 3
	// MID LADDER
	App->collisions->AddCollider({ 113, 128, 47, 8  }, Collider::Type::GROUND); // Floor 3
	App->collisions->AddCollider({ 168, 128, 23, 8 }, Collider::Type::GROUND); // Floor 3
	// RIGHT LADDER
	App->collisions->AddCollider({ 201, 128, 1, 8 }, Collider::Type::GROUND); // Floor 3

	App->collisions->AddCollider({ 31, 88, 1, 8 }, Collider::Type::GROUND); // Floor 4
	// LEFT LADDER
	App->collisions->AddCollider({ 41, 88, 15, 8  }, Collider::Type::GROUND); // Floor 4
	App->collisions->AddCollider({ 63, 88, 1, 8 }, Collider::Type::GROUND); // Floor 4
	//MID LADDER
	//App->collisions->AddCollider({ 73, 88, 78, 8  }, Collider::Type::GROUND); // Floor 4
	//MID LADDER
	App->collisions->AddCollider({ 160, 88, 1, 8 }, Collider::Type::GROUND); // Floor 4
	App->collisions->AddCollider({ 168, 88, 15, 8  }, Collider::Type::GROUND); // Floor 4
	// RIGHT LADDER
	App->collisions->AddCollider({ 193, 88, 1, 8 }, Collider::Type::GROUND); // Floor 4

	activeColliders += 27-1; totalColliders += 27-1;

	App->collisions->AddCollider({ 8+3, 216 - 8, 8-6, 32 + 8 }, Collider::Type::LADDER); // Base
	App->collisions->AddCollider({ 104+3, 216 - 8, 8-6, 32 + 8 }, Collider::Type::LADDER); // Base
	App->collisions->AddCollider({ 208+3, 216 - 8, 8-6, 32 + 8 }, Collider::Type::LADDER); // Base

	App->collisions->AddCollider({ 16 + 3, 176-8, 8 - 6, 32+8 }, Collider::Type::LADDER); // Floor 1

	App->collisions->AddCollider({ 72 + 3, 176-8, 8 - 6, 32+8 }, Collider::Type::LADDER); // Floor 1

	App->collisions->AddCollider({ 144 + 3, 176-8, 8 - 6, 32+8 }, Collider::Type::LADDER); // Floor 1
	App->collisions->AddCollider({ 200 + 3, 176-8, 8 - 6, 32+8 }, Collider::Type::LADDER); // Floor 1

	App->collisions->AddCollider({ 24 + 3, 136 - 8, 8 - 6, 32+8 }, Collider::Type::LADDER); // Floor 2
	App->collisions->AddCollider({ 104 + 3, 136 - 8, 8 - 6, 32+8 }, Collider::Type::LADDER); // Floor 2
	App->collisions->AddCollider({ 192 + 3, 136 - 8, 8 - 6, 32+8 }, Collider::Type::LADDER); // Floor 2

	App->collisions->AddCollider({ 32 + 3, 96 - 8, 8 - 6, 32+8 }, Collider::Type::LADDER); // Floor 3
	App->collisions->AddCollider({ 64 + 3, 96 - 8, 8 - 6, 32+8 }, Collider::Type::LADDER); // Floor 3
	App->collisions->AddCollider({ 152 + 3, 96-8, 8 - 6, 32+8 }, Collider::Type::LADDER); // Floor 3
	App->collisions->AddCollider({ 184 + 3, 96-8, 8 - 6, 32+8 }, Collider::Type::LADDER); // Floor 3
	
	activeColliders += 14+2; totalColliders += 14+2;
	// Wall collisions
	App->collisions->AddCollider({ -1, 216, 1, 32 }, Collider::Type::WALL); // Base Left
	App->collisions->AddCollider({ 224, 216, 1, 32 }, Collider::Type::WALL); // Base Right
	App->collisions->AddCollider({ 0, 177, 4, 31 }, Collider::Type::WALL); // Floor 1 Left
	App->collisions->AddCollider({ 220, 177, 4, 31 }, Collider::Type::WALL); // Floor 1 Right
	App->collisions->AddCollider({ 0, 137, 12, 31 }, Collider::Type::WALL); // Floor 2 Left
	App->collisions->AddCollider({ 212, 137, 12, 31 }, Collider::Type::WALL); // Floor 2 Right
	App->collisions->AddCollider({ 0, 97, 20, 32 }, Collider::Type::WALL); // Floor 3 Left
	App->collisions->AddCollider({ 204, 97, 20, 32 }, Collider::Type::WALL); // Floor 3 Right
	App->collisions->AddCollider({ 0, 60, 28, 28 }, Collider::Type::WALL); // Floor 4 Left
	App->collisions->AddCollider({ 196, 60, 28, 28 }, Collider::Type::WALL); // Floor 4 Right
	App->collisions->AddCollider({ 71+2+2, 56, 74, 32 }, Collider::Type::WALL); // TopBar Left

	activeColliders += 11; totalColliders += 11;

	//ENEMY Wall collisions
	App->collisions->AddCollider({ -1, 216, 1, 32 }, Collider::Type::ENEMYWALL); // Base Left
	App->collisions->AddCollider({ 224, 216, 1, 32 }, Collider::Type::ENEMYWALL); // Base Right
	App->collisions->AddCollider({ 0, 177, 4, 31 }, Collider::Type::ENEMYWALL); // Floor 1 Left
	App->collisions->AddCollider({ 220, 177, 4, 31 }, Collider::Type::ENEMYWALL); // Floor 1 Right
	App->collisions->AddCollider({ 0, 137, 12, 31 }, Collider::Type::ENEMYWALL); // Floor 2 Left
	App->collisions->AddCollider({ 212, 137, 12, 31 }, Collider::Type::ENEMYWALL); // Floor 2 Right
	App->collisions->AddCollider({ 0, 97, 20, 32 }, Collider::Type::ENEMYWALL); // Floor 3 Left
	App->collisions->AddCollider({ 204, 97, 20, 32 }, Collider::Type::ENEMYWALL); // Floor 3 Right
	App->collisions->AddCollider({ 0, 60, 28, 28 }, Collider::Type::ENEMYWALL); // Floor 4 Left
	App->collisions->AddCollider({ 196, 60, 28, 28 }, Collider::Type::ENEMYWALL); // Floor 4 Right
	App->collisions->AddCollider({ 71 + 2 + 2, 56, 74, 32 }, Collider::Type::ENEMYWALL); // TopBar Left

	activeColliders += 11; totalColliders += 11;

	// Adding enemy
	App->enemies->AddEnemy(Enemy_Type::ENEMY_FIREMINION, 132, 248 - 12 - 80); //  Enemy floor 2
	App->enemies->AddEnemy(Enemy_Type::ENEMY_FIREMINION, 132, 248 - 12); // Enemy at the base
	App->enemies->AddEnemy(Enemy_Type::ITEM_NUT, 56, 207);
	App->enemies->AddEnemy(Enemy_Type::ITEM_NUT, 160, 207);
	App->enemies->AddEnemy(Enemy_Type::ITEM_NUT, 56, 167);
	App->enemies->AddEnemy(Enemy_Type::ITEM_NUT, 160, 167);
	App->enemies->AddEnemy(Enemy_Type::ITEM_NUT, 56, 127);
	App->enemies->AddEnemy(Enemy_Type::ITEM_NUT, 160, 127);
	App->enemies->AddEnemy(Enemy_Type::ITEM_NUT, 56, 87);
	App->enemies->AddEnemy(Enemy_Type::ITEM_NUT, 160, 87);
	App->enemies->AddEnemy(Enemy_Type::ITEM_IRON, 197-20, 199);
	App->enemies->AddEnemy(Enemy_Type::ITEM_UMBRELLA, 28, 72);
	App->enemies->AddEnemy(Enemy_Type::ITEM_BAG, 127, 238);
	App->enemies->AddEnemy(Enemy_Type::ITEM_HAMMER, 105, 98);
	App->enemies->AddEnemy(Enemy_Type::ITEM_HAMMER, 8, 138);

	activeColliders += 15; totalColliders += 15;


	//Starting position of the Mario
	App->player->position.x = 0;
	App->player->position.y = 232;
	App->player->speed.y = 0;

	return ret;
}

update_status ModuleScene::Update()
{

	if (App->input->keys[SDL_SCANCODE_F4] == KEY_DOWN )
	{
		Nuts = 0;
	}

	if (Nuts == 0)
	{
		App->audio->PlayFx(FX_Win);
		App->fade->FadeToBlack(this, (Module*)App->level4win, 10);
	}

	if (App->player->destroyed && App->hud->lives > 0) {

		if (resetCounter >= 180)
		{
			resetCounter = -60;
			App->fade->FadeToBlack(this, this);
		}

		if (resetCounter == 1)
		{
			Mix_HaltMusic();
			App->audio->PlayFx(FX_Lose);
			App->particles->AddParticle(App->particles->marioDeath, App->player->playerCollider->rect.x, App->player->playerCollider->rect.y);
		}

		++resetCounter;
	}
	else if (App->player->destroyed && App->hud->lives == 0) {
		
		if (resetCounter >= 500)
		{
			resetCounter = -60;
			App->fade->FadeToBlack(this, (Module*)App->intro);
		}

		if (resetCounter == 1)
		{
			Mix_HaltMusic();
			App->audio->PlayFx(FX_Lose);
			App->particles->AddParticle(App->particles->marioDeath, App->player->playerCollider->rect.x, App->player->playerCollider->rect.y);
		}
		++resetCounter;
	}

	++donkeyCounterFX;
	if (donkeyCounterFX == 150 || donkeyCounterFX == 180 || donkeyCounterFX == 210)
	{
		App->audio->PlayFx(FX_Stomp);
	}
	else if (donkeyCounterFX > 210)
	{
		donkeyCounterFX = 0;
	}

	princessPathLeft.Update();
	princessPathRight.Update();

	if (App->player->position.x >= SCREEN_WIDTH / 2)
		currentAnimPrincess = princessPathRight.GetCurrentAnimation();
	else
		currentAnimPrincess = princessPathLeft.GetCurrentAnimation();
	currentAnimPrincess->Update();

	donkeyPath.Update();
	currentAnimDonkey = donkeyPath.GetCurrentAnimation();

	++helpCounter;
	if (helpCounter >= 210)
	{
		helpCounter = 0;
	}
	return update_status::UPDATE_CONTINUE;
}

// Update: draw background
update_status ModuleScene::PostUpdate()
{
	// Draw everything --------------------------------------
	App->render->Blit(bgTexture, 0, 0, &level_4);

	if (currentAnimDonkey != nullptr)
	{
		LOG("Drawing DONKEY KONG of the WinScene");
		App->render->Blit(bgTexture, 92, 56, &(currentAnimDonkey->GetCurrentFrame()));
	}

	if (currentAnimPrincess != nullptr)
	{
		LOG("Drawing DONKEY KONG of the WinScene");
		App->render->Blit(bgTexture, 105, 26, &(currentAnimPrincess->GetCurrentFrame()));
		if (helpCounter >= 150)
		{
			if (currentAnimPrincess == &prAnimLeft)
				App->render->Blit(bgTexture, 78, 24, &helpLeft);
			else if (currentAnimPrincess == &prAnimRight)
				App->render->Blit(bgTexture, 120, 24, &helpRight);
		}
	}

	if (App->player->destroyed && App->hud->lives == 0) {
		App->render->DrawQuad(blackQuad, 0, 0, 0, 80 );
	}

	return update_status::UPDATE_CONTINUE;
}

bool ModuleScene::CleanUp()
{
	// TODO 2: Enable (and properly disable) the player module
	activeTextures = activeColliders = activeFonts = activeFx = 0;

	App->particles->Disable();
	App->collisions->Disable();
	App->player->Disable();
	App->enemies->Disable();

	App->textures->Unload(bgTexture);
	--totalTextures;

	App->audio->UnloadFx(FX_Win);
	--totalFx;

	App->audio->UnloadFx(FX_Lose);
	--totalFx;

	return true;
}