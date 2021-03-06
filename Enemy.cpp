#include "Enemy.h"

#include "Application.h"
#include "ModuleCollisions.h"
#include "Level4.h"
#include "ModuleAudio.h"
#include "ModuleRender.h"
#include "ModuleParticles.h"
#include "ModuleHud.h"
#include "ModuleEnemies.h"
#include "ModuleHammer.h"

Enemy::Enemy(int x, int y) : position(x, y)
{
	spawnPos = position;
}

Enemy::~Enemy()
{
	if (collider != nullptr)
		collider->pendingToDelete = true;
}

const Collider* Enemy::GetCollider() const
{
	return collider;
}

void Enemy::Update()
{
	if (currentAnim != nullptr)
		currentAnim->Update();

	if (collider != nullptr)
		collider->SetPos(position.x + tempx, position.y);
}

void Enemy::Draw()
{
	if (currentAnim != nullptr)
		App->render->Blit(texture, position.x, position.y, &(currentAnim->GetCurrentFrame()));
}

void Enemy::OnCollision(Collider* collider)
{
	if (collider->type == Collider::Type::HAMMER)
	{
		// For enemies
		SetToDelete();
		App->particles->AddParticle(App->particles->plasma, this->position.x, this->position.y, Collider::Type::NONE, 10);
		//Particle 300
		App->particles->AddParticle(App->particles->score300, this->position.x - 5, this->position.y - 7, Collider::Type::NONE, 60);
		App->hud->score += 300;
		App->audio->PlayFx(destroyedFx);
	}

	
	if ((collider->type == Collider::Type::FIREBARREL || collider->type == Collider::Type::HAMMER))
	{
		if (this->collider->item == Collider::MINECART)
		{
			App->enemies->AddEnemy(Enemy_Type::ENEMY_MINECART, spawnPos.x, spawnPos.y);
			SetToDelete();
		}
	}

	if (this->collider->type == Collider::Type::Item_Type && collider->type == Collider::Type::PLAYER)
	{
		// For score items
		if (this->collider->item == Collider::Items::SCOREITEMS) {
			App->hud->score += 500;
			//Particle 500
			App->particles->AddParticle(App->particles->score500, this->position.x - 5, this->position.y - 7, Collider::Type::NONE,5);
			SetToDelete();
			App->audio->PlayFx(destroyedFx);
		}
		// For nuts
		if (((collider->rect.x < this->collider->rect.x) && (collider->rect.x + collider->rect.w >= this->collider->rect.x + this->collider->rect.w)) && this->collider->IMPORTANTITEMS) {
			App->level4->Nuts--;
			App->hud->score += 100;
			//Particle 100
			App->particles->AddParticle(App->particles->score100, this->position.x - 5, this->position.y - 7, Collider::Type::NONE,5);
			SetToDelete();
			App->audio->PlayFx(destroyedFx);

			App->particles->AddParticle(App->particles->wall, this->collider->rect.x, this->collider->rect.y, Collider::Type::WALL, 120);
			App->particles->AddParticle(App->particles->wall, this->collider->rect.x, this->collider->rect.y, Collider::Type::ENEMYWALL, 120);
			App->level4->activeColliders += 2;
			App->level4->totalColliders += 2;
		}
		// For items like hammer
		if (this->collider->item == Collider::POWERITEMS ){
			App->hammer->hammerExist = true;
			SetToDelete();
			App->audio->PlayFx(destroyedFx);
		}
	}
	
	if (this->collider->type == Collider::Type::ENEMY && (collider->type == Collider::Type::ENEMYWALL) && this->collider->item != Collider::Items::MINECART) {
		if (this->position.x <	collider->rect.x + collider->rect.w / 2) // Left wall collider
		{
			this->position.x = collider->rect.x - this->collider->rect.w;
			this->enemySpeed.x = -this->enemySpeed.x;
			goingLeft = true;
		}
		else if (this->position.x > collider->rect.x + collider->rect.w / 2) // Right wall collider
		{
			this->position.x = collider->rect.x + collider->rect.w;
			this->enemySpeed.x = -this->enemySpeed.x;
			goingLeft = false;
		}
	}
	

	if (this->collider->type == Collider::Type::ENEMY && this->collider->item == Collider::Items::None && collider->item != Collider::Items::ELEVATINGLADDER && collider->type == Collider::Type::LADDER &&
		(this->position.x + 4 < collider->rect.x && this->position.x + 9 > collider->rect.x + 1)) {

		if (climbingUP == false && climbingDOWN == false)
		{
			climbingUP = (rand() % 100) < 5;
			if (climbingUP)
				climbingDOWN = false;
			else
			{
				climbingDOWN = (rand() % 100) < 5;
			}
			if (climbingDOWN)
				climbingUP = false;
		}


		if (climbingUP)
		{
			this->enemySpeed.x = 0;
			if (this->position.y > collider->rect.y)
			{
			--this->position.y;
			}
			else if (this->position.y - this->collider->rect.h  <= collider->rect.y)
			{
				if (climbingUP)
				{
					climbingUP = false;
					this->position.y = collider->rect.y - this->collider->rect.h;
					this->enemySpeed.x = -1;
				}
			}
		}
		if (climbingDOWN)
		{
			this->enemySpeed.x = 0;
			if (this->position.y + this->collider->rect.h < collider->rect.y + collider->rect.h)
			{
				++this->position.y;
			}
			else if (this->position.y + this->collider->rect.h >= collider->rect.y + collider->rect.h)
			{
				if (climbingDOWN)
				{
					climbingDOWN = false;
					this->position.y = collider->rect.y + collider->rect.h - this->collider->rect.h;
					this->enemySpeed.x = +1;
				}
			}
		}
	}
	
}

void Enemy::SetToDelete()
{
	pendingToDelete = true;
	if (collider != nullptr)
		collider->pendingToDelete = true;
}