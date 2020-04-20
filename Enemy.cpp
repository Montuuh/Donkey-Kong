#include "Enemy.h"

#include "Application.h"
#include "ModuleCollisions.h"
#include "Level4.h"
#include "ModuleAudio.h"
#include "ModuleRender.h"
#include "ModuleParticles.h"

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
		collider->SetPos(position.x, position.y);
}

void Enemy::Draw()
{
	if (currentAnim != nullptr)
		App->render->Blit(texture, position.x, position.y, &(currentAnim->GetCurrentFrame()));
}

void Enemy::OnCollision(Collider* collider)
{
	//App->audio->PlayFx(destroyedFx);
	if (collider->type == Collider::Type::HAMMER)
	{
		SetToDelete();
		App->particles->AddParticle(App->particles->plasma, this->position.x, this->position.y, Collider::Type::NONE, 10);
	}

	if (this->collider->type == Collider::Type::Item_Type && collider->type == Collider::Type::PLAYER)
	{
		if (((collider->rect.x < this->collider->rect.x) && (collider->rect.x + collider->rect.w >= this->collider->rect.x + this->collider->rect.w))) {
			App->scene->Nuts--;
			SetToDelete();
		}
	}
}

void Enemy::SetToDelete()
{
	pendingToDelete = true;
	if (collider != nullptr)
		collider->pendingToDelete = true;
}