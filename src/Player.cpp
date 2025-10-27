#include "Player.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"
#include "EntityManager.h"
#include "Map.h"
Player::Player() : Entity(EntityType::PLAYER)
{
	name = "Player";
}

Player::~Player() {

}

bool Player::Awake() {

	//L03: TODO 2: Initialize Player parameters
	position = Vector2D(86, 86);
	return true;
}

bool Player::Start() {

	// load


	std::unordered_map<int, std::string> aliases = { {15,"idle"},{0,"move"},{6,"jump"},{18,"death"} };


	anims.LoadFromTSX("Assets/Textures/ghost-export.tsx", aliases);
	anims.SetCurrent("idle");

	//L03: TODO 2: Initialize Player parameters
	texture = Engine::GetInstance().textures->Load("Assets/Textures/ghost-export.png");

	// L08 TODO 5: Add physics to the player - initialize physics body
	//Engine::GetInstance().textures->GetSize(texture, texW, texH);
	texW = 32;
	texH = 32;
	pbody = Engine::GetInstance().physics->CreateCircle((int)position.getX(), (int)position.getY(), texW / 2, bodyType::DYNAMIC);

	// L08 TODO 6: Assign player class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
	pbody->listener = this;

	// L08 TODO 7: Assign collider type
	pbody->ctype = ColliderType::PLAYER;

	//initialize audio effect
	pickCoinFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/coin-collision-sound-342335.wav");

	return true;
}

bool Player::Update(float dt)
{
	GetPhysicsValues();
	Move();
	Jump();
	ApplyPhysics();
	Draw(dt);

	
	Vector2D mapSize = Engine::GetInstance().map->GetMapSizeInPixels();
	float limitLeft = Engine::GetInstance().render->camera.w / 4;
	float limitRight = mapSize.getX() - Engine::GetInstance().render->camera.w * 3 / 4;
	if (position.getX() - limitLeft > 0 && position.getX() < limitRight) {
		Engine::GetInstance().render->camera.x = -position.getX() + Engine::GetInstance().render->camera.w / 4;
	}
	float limitUp = Engine::GetInstance().render->camera.h / 4;
	float limitDown = mapSize.getY() - Engine::GetInstance().render->camera.h * 3 / 4;
	if (position.getY() - limitUp > 0 && position.getY() < limitDown) {
		Engine::GetInstance().render->camera.y = -position.getY() + Engine::GetInstance().render->camera.h / 4;
	}


		
		Engine::GetInstance().render->camera.y = (int)(-position.getY() + Engine::GetInstance().render->camera.h / 2);

		if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN) {

			godMode = !godMode;
			if (godMode) { b2Body_SetGravityScale(pbody->body, 0); }else{ b2Body_SetGravityScale(pbody->body, 1); }

		}
	//}
	///*else if (position.getX() > 1280 - Engine::GetInstance().render->camera.w / 2) {
	//
	//	Engine::GetInstance().render->camera.x = 1280 - Engine::GetInstance().render->camera.w;
	//
	//}*/
	//else {

	//Engine::GetInstance().render->camera.x = (int)(-position.getX() + Engine::GetInstance().render->camera.w / 2);

	//}
	return true;
}

void Player::GetPhysicsValues() {
	// Read current velocity
	velocity = Engine::GetInstance().physics->GetLinearVelocity(pbody);
	if(!godMode){ velocity = { 0, velocity.y }; }else{ velocity = { 0, 0 }; }
	 // Reset horizontal velocity by default, this way the player stops when no key is pressed
}

void Player::Move() {
	
	// Move left/right
	
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && !isJumping) {
		velocity.x = -speed;
		anims.SetCurrent("move");
		isWalking = true;
	}
	else if(Engine::GetInstance().input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT){
		velocity.x = -speed;
		isWalking = true;
	}
	else if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && !isJumping) {
		velocity.x = speed;
		anims.SetCurrent("move");
		isWalking = true;
	}
	else if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT ) {
		velocity.x = speed;
		isWalking = true;
		
	}
	if (godMode ) {
		
		if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT && !isJumping) {
			velocity.y = -speed;
			anims.SetCurrent("move");
			isWalking = true;
		}
		else if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {
			velocity.y = -speed;
			isWalking = true;
		}
		else if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT && !isJumping) {
			velocity.y = speed;
			anims.SetCurrent("move");
			isWalking = true;
		}
		else if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
			velocity.y = speed;
			isWalking = true;

		}
	}
	else { isWalking = false; }
	/*else if (isdead) {
		anims.SetCurrent("death");
	}*/
	if(isJumping){
		
			anims.SetCurrent("jump");
		
	}
	if (!isWalking && !isJumping) { anims.SetCurrent("idle"); }
}

void Player::Jump() {
	// This function can be used for more complex jump logic if needed
	if ((Engine::GetInstance().input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && isJumping == false )|| Engine::GetInstance().input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && isJumping && !secondJump) {
		Engine::GetInstance().physics->ApplyLinearImpulseToCenter(pbody, 0.0f, -jumpForce, true);
		anims.SetCurrent("jump");
		isJumping = true;
		if (firstJump) {
		
			secondJump = false;
		}
		else { secondJump = true; }
		firstJump = false;
		
	}
	if (godMode && (Engine::GetInstance().input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)) {
	
	
		Engine::GetInstance().physics->ApplyLinearImpulseToCenter(pbody, 0.0f, -jumpForce, true);
		anims.SetCurrent("jump");
	}
	
}

void Player::ApplyPhysics() {
	// Preserve vertical speed while jumping
	if (isJumping == true) {
		velocity.y = Engine::GetInstance().physics->GetYVelocity(pbody);
	}

	// Apply velocity via helper
	Engine::GetInstance().physics->SetLinearVelocity(pbody, velocity);
}

void Player::Draw(float dt) {

	anims.Update(dt);
	const SDL_Rect& animFrame = anims.GetCurrentFrame();

	// Update render position using your PhysBody helper
	int x, y;
	pbody->GetPosition(x, y);
	position.setX((float)x);
	position.setY((float)y);
	Engine::GetInstance().render->DrawTexture(texture, x - texW / 2, y - texH / 2, &animFrame);
}

bool Player::CleanUp()
{
	LOG("Cleanup player");
	Engine::GetInstance().textures->UnLoad(texture);
	return true;
}

void Player::Reset()
{
	
	b2Vec2 initialPos = { PIXEL_TO_METERS(86), PIXEL_TO_METERS(86) };
	b2Rot rotation = b2MakeRot(0.0f);
	b2Body_SetTransform(pbody->body, initialPos, rotation);
}

// L08 TODO 6: Define OnCollision function for the player. 
void Player::OnCollision(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype)
	{
	case ColliderType::PLATFORM:
		LOG("Collision PLATFORM");
		//reset the jump flag when touching the ground
		isJumping = false;
		firstJump = true;
		
		break;	
	case ColliderType::ITEM:
		LOG("Collision ITEM");
		Engine::GetInstance().audio->PlayFx(pickCoinFxId);
		physB->listener->Destroy();
		break;
	case ColliderType::UNKNOWN:

	

		if (!godMode) {
			LOG("Collision DEATH");
			isdead = true;
			Reset();
		}

		break;
	default:
		break;
	}
}

void Player::OnCollisionEnd(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::PLATFORM:
		LOG("End Collision PLATFORM");
		break;
	case ColliderType::ITEM:
		LOG("End Collision ITEM");
		break;
	case ColliderType::UNKNOWN:
		LOG("End Collision UNKNOWN");
		break;
	default:
		break;
	}
}

