#include <common.h>
#include <game.h>
#include <sfx.h>
#include <g3dhax.h>
#include "boss.h"

const char* SBarcNameList[] = {
	"sanbo",
	NULL
};

class daBossSanbo : daBoss {
	//Let's take some functions and add them to this boss
	int onCreate();
	int onDelete();
	int onExecute();
	int onDraw();

	//for loading the 3d model
	mHeapAllocator_c allocator;
	nw4r::g3d::ResFile resFile;

	//the body objects
	m3d::mdl_c headModelSpike;
	m3d::mdl_c headModelOrange;
	m3d::mdl_c bodyModels[4];
	Vec bodyPositions[5];
	S16Vec bodyRotations[5];
	float transPeriode = 0.979f;
	float rotSpeed[5] = {
		0.11f, 0.12f, 0.125f, 0.12f, 0.14f 
	};
	s16 headRot = 0xE800;

	ActivePhysics bodyPhysics[5];
	

	//boss-specific variables
	int timer;
	long movementTimer;
	float BaseLine;
	char dying;
	float xSpeed = 0.6;
	float ySpeed = 0.6;
	Vec initialPos;
	char damage;
	char isInvulnerable;
	dAc_Py_c* target;
	int activeObjects;

	//for building the boss and loading it on the screen
	static daBossSanbo* build();

	//setting up the body model
	void setupBodyModels();

	//for updating the 3d models based on positions and rotation
	void updateModelMatrices();

	//collision functions
	bool prePlayerCollision(ActivePhysics* apThis, ActivePhysics* apOther);
	void playerCollision(ActivePhysics* apThis, ActivePhysics* apOther);

	bool collisionCat3_StarPower(ActivePhysics* apThis, ActivePhysics* apOther);		//ja
	bool collisionCatD_Drill(ActivePhysics* apThis, ActivePhysics* apOther);			//ja
	bool collisionCat7_GroundPound(ActivePhysics* apThis, ActivePhysics* apOther);		//ja
	bool collisionCat9_RollingObject(ActivePhysics* apThis, ActivePhysics* apOther);	//ja
	bool collisionCat1_Fireball_E_Explosion(ActivePhysics* apThis, ActivePhysics* apOther); //ja
	bool collisionCat2_IceBall_15_YoshiIce(ActivePhysics* apThis, ActivePhysics* apOther);	//ja
	bool collisionCat13_Hammer(ActivePhysics* apThis, ActivePhysics* apOther);				//ja
	bool collisionCatA_PenguinMario(ActivePhysics* apThis, ActivePhysics* apOther);			//ja

	//other functions if needed
	int nearestPlayer();			//returns a random player of max 4 players. This is the Player the boss will follow
	void addScoreWhenHit(void* other);
	void smoothMovement();
	bool IntroduceBoss(daKameckDemo* Kameck, int timer);
	void removeMyActivePhysics();
	void addMyActivePhysics();
	void createMyActivePhysics();

	//Now tell the game that we are using states in this class
	USING_STATES(daBossSanbo);

	//Create our actual states
	DECLARE_STATE(Intro);
	DECLARE_STATE(Walk);
	DECLARE_STATE(Spit);
	DECLARE_STATE(GoDown);
	//DECLARE_STATE(ComeUp);
	//DECLARE_STATE(Peek);
	//DECLARE_STATE(Fake);
	//DECLARE_STATE(Damage);
	//DECLARE_STATE(Wait);
	//DECLARE_STATE(Outro);
};


///
/// Externs and states
///

CREATE_STATE(daBossSanbo, Intro);
CREATE_STATE(daBossSanbo, Walk);
CREATE_STATE(daBossSanbo, Spit);
CREATE_STATE(daBossSanbo, GoDown);
//CREATE_STATE(daBossSanbo, ComeUp);
//CREATE_STATE(daBossSanbo, Damage);
//CREATE_STATE(daBossSanbo, Outro);


///
/// Build me for creating on screen
/// 
daBossSanbo* daBossSanbo::build() {
	void* buffer = AllocFromGameHeap1(sizeof(daBossSanbo));
	return new(buffer) daBossSanbo;
}


///
/// Collision functions
///

bool daBossSanbo::prePlayerCollision(ActivePhysics* apThis, ActivePhysics* apOther) {
	if (apOther->owner->stageActorType == 1) {
		if (apOther->info.category2 == 7) {
			if (collisionCat7_GroundPound(apThis, apOther))
				return true;
		}
	}

	return dEn_c::prePlayerCollision(apThis, apOther);
}

void daBossSanbo::addScoreWhenHit(void* other) { }

void daBossSanbo::playerCollision(ActivePhysics* apThis, ActivePhysics* apOther) {
	if(apOther->owner->last_pos.y > this->bodyPositions[activeObjects].y) {
		char ret = usedForDeterminingStatePress_or_playerCollision(this, apThis, apOther, 0);
		
		if (ret == 0) {
			this->dEn_c::playerCollision(apThis, apOther);
			this->_vf220(apOther->owner);
		}

		//FIXME hack to make multiple playerCollisions work
		deathInfo.isDead = 0;
		this->flags_4FC |= (1 << (31 - 7));
		this->counter_504[apOther->owner->which_player] = 0;
	}
	else {
		DamagePlayer(this, apThis, apOther);
	}
}

bool daBossSanbo::collisionCat3_StarPower(ActivePhysics* apThis, ActivePhysics* apOther) {
	return false;
}

bool daBossSanbo::collisionCatD_Drill(ActivePhysics* apThis, ActivePhysics* apOther) {
	return collisionCat7_GroundPound(apThis, apOther);
}

bool daBossSanbo::collisionCat9_RollingObject(ActivePhysics* apThis, ActivePhysics* apOther) {

	dActor_c* block = apOther->owner;
	dEn_c* blah = (dEn_c*)block;

	if (blah->direction == 0) { blah->direction = 1; }
	else { blah->direction = 0; }

	blah->speed.x = -blah->speed.x * 0.5;
	blah->pos.x += blah->speed.x;

	if (blah->speed.y < 0) {
		blah->speed.y = -blah->speed.y;
	}

	blah->doSpriteMovement();
	blah->doSpriteMovement();

	this->pos.x -= blah->speed.x;
	smoothMovement();

	/*this->damage = this->damage + 5;
	this->activeObjects -= 1;
	this->bodyPhysics[activeObjects].removeFromList();*/


	PlaySound(this, SE_EMY_DOWN);
	S16Vec nullRot = { 0,0,0 };
	Vec oneVec = { 1.0f, 1.0f, 1.0f };
	SpawnEffect("Wm_mr_kickhit", 0, &blah->pos, &nullRot, &oneVec);

	//for testing

	//if (this->damage > 14) { /*doStateChange(&StateID_Outro);*/ }
	//else { /*doStateChange(&StateID_GoDown);*/ }
	return false;
}

bool daBossSanbo::collisionCat1_Fireball_E_Explosion(ActivePhysics* apThis, ActivePhysics* apOther) {
	return false;
}

bool daBossSanbo::collisionCat2_IceBall_15_YoshiIce(ActivePhysics* apThis, ActivePhysics* apOther) {
	return false;
}

bool daBossSanbo::collisionCatA_PenguinMario(ActivePhysics* apThis, ActivePhysics* apOther) {
	DamagePlayer(this, apThis, apOther);
	return false;
}

bool daBossSanbo::collisionCat13_Hammer(ActivePhysics* apThis, ActivePhysics* apOther) {
	return false;
}

bool daBossSanbo::collisionCat7_GroundPound(ActivePhysics* apThis, ActivePhysics* apOther) {

	dActor_c* block = apOther->owner;
	dEn_c* mario = (dEn_c*)block;

	S16Vec nullRot = { 0,0,0 };
	Vec oneVec = { 1.0f, 1.0f, 1.0f };
	SpawnEffect("Wm_en_vshit", 0, &mario->pos, &nullRot, &oneVec);

	mario->speed.y = -mario->speed.y;
	mario->pos.y += mario->speed.y;

	if (mario->direction == 0) {
		mario->speed.x = 4.0;
	}
	else {
		mario->speed.x = -4.0;
	}

	mario->doSpriteMovement();
	mario->doSpriteMovement();
	
	if (this->isInvulnerable == 1) {
		_vf220(apOther->owner);
	}
	else {
		this->damage += 5;
		apOther->someFlagByte |= 2;
		PlaySoundAsync(this, SE_EMY_DOWN);
		if (this->damage > 14) {
			//doStateChange(&StateID_Outro);
		}
		else {
			doStateChange(&StateID_GoDown);
		}
	}
	return true;
}


///
/// 3d model
///

void daBossSanbo::setupBodyModels() {
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	this->resFile.data = getResource("sanbo", "g3d/t00.brres");
	nw4r::g3d::ResMdl headMdl = this->resFile.GetResMdl("sanbo_head");
	nw4r::g3d::ResMdl bodyMdl = this->resFile.GetResMdl("sanbo_body");
	nw4r::g3d::ResMdl headMdlOrange = this->resFile.GetResMdl("sanbo_head_orange");
	//nw4r::g3d::ResMdl bodyMdlOrange = this->resFile.GetResMdl("sanbo_orange");

	for (int i = 0; i < 4; i++)
	{
		this->bodyModels[i].setup(bodyMdl, &allocator, 0x224, 1, 0);
		SetupTextures_Enemy(&bodyModels[i], 0);
		//this->bodyModelsOrange[i].setup(bodyMdlOrange, &allocator, 0x224, 1, 0);
	}

	this->headModelSpike.setup(headMdl, &allocator, 0x224, 1, 0);
	SetupTextures_Enemy(&headModelSpike, 0);
	this->headModelOrange.setup(headMdlOrange, &allocator, 0x224, 1, 0);
	SetupTextures_Enemy(&headModelOrange, 0);

	allocator.unlink();
}


///
/// main functions
///

int daBossSanbo::onCreate() {
	setupBodyModels();

	this->scale = (Vec){ 1.5, 1.5, 1.5 };

	
	this->createMyActivePhysics();
	

	speed.x = 0.0;
	speed.y = 0.0;
	this->BaseLine = this->pos.y - 12;
	this->pos.y -= 145;
	this->pos.z = -3000.0f;
	this->initialPos = this->pos;
	this->isInvulnerable = 0;
	this->damage = 0;
	this->dying = 0;
	this->disableEatIn();

	this->rot.x = 0; // X is vertical axis
	this->rot.y = 0; // Y is horizontal axis
	this->rot.z = 0; // Z is ... an axis >.>
	this->bodyRotations[4].y = 0xE800;
	this->direction = 0;
	this->movementTimer = 0;
	this->activeObjects = 4;

	//posisitons
	//updateAllPositions();
	smoothMovement();

	doStateChange(&StateID_Intro);
	this->onExecute();
	return true;
}

int daBossSanbo::onDelete() {
	return true;
}

int daBossSanbo::onExecute() {
	acState.execute();
	updateModelMatrices();

	return true;
}

int daBossSanbo::onDraw() {
	
	for (int i = 0; i < activeObjects; i++) {
		this->bodyModels[i].scheduleForDrawing();
	}
	
	if (isInvulnerable == 1) {
		this->headModelSpike.scheduleForDrawing();
	}
	else {
		this->headModelOrange.scheduleForDrawing();
	}

	return true;
}

///
/// Update the model each frame with correct positions and rotations
///

void daBossSanbo::updateModelMatrices() {

	for (int i = 0; i < activeObjects; i++) {
		mMtx partMatrix;
		Vec partPos = this->bodyPositions[i];
		S16Vec partRot = this->bodyRotations[i];

		partMatrix.translation(partPos.x, partPos.y, partPos.z);
		partMatrix.applyRotationYXZ(&partRot.x, &partRot.y, &partRot.z);

		this->bodyModels[i].setDrawMatrix(partMatrix);
		this->bodyModels[i].setScale(&scale);
		this->bodyModels[i].calcWorld(false);

		this->bodyPhysics[i].info.xDistToCenter = partPos.x - this->pos.x;
		this->bodyPhysics[i].info.yDistToCenter = partPos.y - this->pos.y;
	}

	Vec* headPos = &this->bodyPositions[activeObjects];
	S16Vec* headRot = &this->bodyRotations[activeObjects];

	matrix.translation(headPos->x, headPos->y, headPos->z);
	matrix.applyRotationYXZ(&headRot->x, &headRot->y, &headRot->z);


	if (this->isInvulnerable == 1) {
		this->headModelSpike.setDrawMatrix(matrix);
		this->headModelSpike.setScale(&scale);
		this->headModelSpike.calcWorld(false);
	}
	else {
		this->headModelOrange.setDrawMatrix(matrix);
		this->headModelOrange.setScale(&scale);
		this->headModelOrange.calcWorld(false);
	}

	this->bodyPhysics[4].info.xDistToCenter = headPos->x - this->pos.x;
	this->bodyPhysics[4].info.yDistToCenter = headPos->y - this->pos.y;
}

///
/// state implementations
///

void daBossSanbo::beginState_Intro() {
	this->timer = 0;

	// Stop the BGM Music
	StopBGMMusic();

	// Set the necessary Flags and make Mario enter Demo Mode
	dStage32C_c::instance->freezeMarioBossFlag = 1;
	WLClass::instance->_4 = 4;
	WLClass::instance->_8 = 0;

	MakeMarioEnterDemoMode();

	// Make sure to use the correct position
	Vec pos = (Vec){ this->pos.x - 135.0f, this->pos.y + 254.0f, 3564.0f };
	S16Vec rot = (S16Vec){ 0, 0, 0 };

	// Create And use Kameck
	this->Kameck = (daKameckDemo*)this->createChild(KAMECK_FOR_CASTLE_DEMO, (dStageActor_c*)this, 0, &pos, &rot, 0);
	this->Kameck->doStateChange(&daKameckDemo::StateID_DemoWait);
	this->scale = (Vec){ 1.5, 1.5, 1.5 };
}

void daBossSanbo::executeState_Intro() {
	this->timer += 1;

	bool ret;
	ret = IntroduceBoss(Kameck, this->timer);

	if (ret) {
		doStateChange(&StateID_Walk);
	}
}

void daBossSanbo::endState_Intro() {
	CleanupKameck(this, Kameck);
	this->BaseLine = this->pos.y;
	this->pos.z = 4000.0;
	smoothMovement();
}

void daBossSanbo::beginState_Walk() {
	this->timer = 0;
	//S16Vec nullRot = { 0,0,0 };
	//Vec oneVec = { 1.0f, 1.0f, 1.0f };
	//SpawnEffect("Wm_en_sanbospillsand", 0, &this->pos, &nullRot, &oneVec);
	this->target = GetSpecificPlayerActor(this->nearestPlayer());
	this->speed.x = (this->direction) ? this->xSpeed : -this->xSpeed;
}

void daBossSanbo::executeState_Walk() {
	this->timer += 1;

	if (this->target->pos.x > this->pos.x && this->direction == 0) {
		this->speed.x += 0.004;
		int done = SmoothRotation(&this->headRot, 0x1600, 0x500);
		if (done) {
			this->direction = 1;
			this->speed.x = this->xSpeed;
		}
	} 
	else if(this->target->pos.x < this->pos.x && this->direction == 1) {
		this->speed.x -= 0.004;
		int done = SmoothRotation(&this->headRot, 0xE800, 0x500);
		if (done) {
			this->direction = 0;
			this->speed.x = -this->xSpeed;
		}
	}

	this->pos.x += this->speed.x;

	smoothMovement();
		
	if (this->timer > 150) {
		char over = 0;
		for (int i = 1; i < activeObjects; i++) {
			if (abs(bodyPositions[i - 1].x - bodyPositions[i].x) < 1) {
				over = 1;
			}
			else {
				over = 0;
				break;
			}
		}
		if (over == 1) {
			int rand;
			rand = GenerateRandomNumber(30);
			if (rand == 0 || rand == 1) {
				f32 dx= target->pos.x - this->bodyPositions[this->activeObjects].x;
				f32 dy = target->pos.y - this->bodyPositions[this->activeObjects].y;
				if (abs(dy/dx) < 1.3) {
					doStateChange(&StateID_Spit);
				}
			}
			else if (this->timer > 600) {
				doStateChange(&StateID_GoDown);
			}
		}
	}
	
}

void daBossSanbo::endState_Walk() {

}


void daBossSanbo::beginState_Spit() {
	this->timer = 0;
}

void daBossSanbo::executeState_Spit() {
	this->timer += 1;

	for (int i = 0; i < activeObjects + 1; i++) {
		if (i % 2 == 0) {
			bodyRotations[i].y += (0x10000 / 20);
		}
		else {
			bodyRotations[i].y -= (0x10000 / 20);
		}
	}

	if (this->timer == 1) {
		Vec tempPos = (Vec){ this->pos.x, this->BaseLine - 12.0f, this->pos.z };
		S16Vec nullRot = { 0,0,0 };
		Vec oneVec = { 0.5f, 1.0f, 1.0f };
		SpawnEffect("Wm_mr_sanddive_smk", 0, &tempPos, &nullRot, &oneVec);
	}

	if (this->timer == 10) {

		f32 dx = target->pos.x - this->bodyPositions[this->activeObjects].x;
		f32 dy = target->pos.y - this->bodyPositions[this->activeObjects].y;
		dStageActor_c* spawner = CreateActor(106, 0, this->bodyPositions[this->activeObjects], 0, 0);
		PlaySound(this, SE_BOSS_JR_FIRE_SHOT);

		float normalizer = sqrtf(dx * dx + dy * dy);

		spawner->speed.x = 4 * (1 / normalizer) * dx;
		spawner->speed.y = 4 * (1 / normalizer) * dy;
		spawner->scale = (Vec){ 2.0, 2.0, 2.0 };
	}

	if (this->timer > 19) {
		doStateChange(&StateID_Walk);
	}
}

void daBossSanbo::endState_Spit() {

}


void daBossSanbo::beginState_GoDown() {
	this->timer = 0;
	this->pos.z = -3000;
	this->isInvulnerable = 1;
	this->removeMyActivePhysics();
}

void daBossSanbo::executeState_GoDown() {
	this->timer += 1;
	if ((timer > 150) && (timer < 230)) {
		//ShakeScreen(this, 2, 1, 1, 0);
		this->pos.y -= 4.7 - 0.07 * (timer - 150);
	}
	smoothMovement();
}

void daBossSanbo::endState_GoDown() {

}



bool daBossSanbo::IntroduceBoss(daKameckDemo* Kameck, int timer) {
	if (timer == 130) { this->Kameck->doStateChange(&daKameckDemo::StateID_DemoSt); }
	if (timer == 400) { this->Kameck->doStateChange(&daKameckDemo::StateID_DemoSt2); }

	
	Vec tempPos2 = (Vec){ this->pos.x, this->BaseLine, this->pos.z };
	S16Vec nullRot2 = { 0,0,0 };
	Vec oneVec2 = { 2.0f, 1.0f, 1.0f };

	if (timer == 160) {
		SpawnEffect("Wm_mr_sanddive_out", 0, &tempPos2, &nullRot2, &oneVec2);
		SpawnEffect("Wm_mr_sanddive_smk", 0, &tempPos2, &nullRot2, &oneVec2);
	}
	

	if ((timer > 150) && (timer < 230)) {
		//ShakeScreen(this, 2, 1, 1, 0);
		this->pos.y += 4.7 - 0.07 * (timer - 150);
		smoothMovement();
	} 
	if (timer > 230) {
		smoothMovement();
	}

	if (timer == 360) {
		Vec tempPos = (Vec){ this->pos.x - 40.0f, this->pos.y + 120.0f, 3564.0f };
		S16Vec nullRot = { 0,0,0 };
		Vec oneVec = { 1.0f, 1.0f, 1.0f };
		SpawnEffect("Wm_ob_greencoinkira", 0, &tempPos, &nullRot, &oneVec);
		SpawnEffect("Wm_mr_yoshiicehit_a", 0, &tempPos, &nullRot, &oneVec);
		SpawnEffect("Wm_mr_yoshiicehit_b", 0, &tempPos, &nullRot, &oneVec);
		SpawnEffect("Wm_ob_redringget", 0, &tempPos, &nullRot, &oneVec);
		SpawnEffect("Wm_ob_keyget01", 0, &tempPos, &nullRot, &oneVec);
		SpawnEffect("Wm_ob_greencoinkira_a", 0, &tempPos, &nullRot, &oneVec);
		SpawnEffect("Wm_ob_keyget01_c", 0, &tempPos, &nullRot, &oneVec);
	}

	if (timer > 420) { return true; }
	return false;
}

void daBossSanbo::smoothMovement() {
	this->movementTimer += 1;
	//translation
	for (int i = 0; i < 5; i++) {
		if (i == 0) {
			this->bodyPositions[i].x = this->pos.x;
		}
		else {
			char tmp;
			if (i % 2 == 0) {
				tmp = 1;
			}
			else {
				tmp = -1;
			}
			this->bodyPositions[i].x = tmp * sin(2 * (3.14 / this->transPeriode) * this->movementTimer) * 1.5 + this->pos.x;
		}
		this->bodyPositions[i].y = this->pos.y + 24.0 * i;
		this->bodyPositions[i].z = this->pos.z + 100 * i;
	}

	for (int i = 0; i < 4; i++) {
		this->bodyRotations[i].y = (int)(sin(this->rotSpeed[i] * this->movementTimer) * 0x650);
	}

	this->bodyRotations[activeObjects].y = (int)(sin(this->rotSpeed[4] * this->movementTimer) * 0x800) + this->headRot;
	
}

int daBossSanbo::nearestPlayer() {
	int players[4];
	int playerCount = 0;
	int smallestDistance = 100000;
	int whichPlayer = 0;

	for (int i = 0; i < 4; i++) {
		if (Player_Active[i] != 0 && Player_Lives[Player_ID[i]] > 0) {
			players[playerCount] = i;
			playerCount++;
		}
	}

	return players[GenerateRandomNumber(playerCount)];

	/*for (int i = 0; i < playerCount; i++) {
		dAc_Py_c* target = GetSpecificPlayerActor(players[i]);
		if (abs(target->pos.x - this->pos.x) < smallestDistance) {
			smallestDistance = abs(target->pos.x - this->pos.x);
			whichPlayer = i;
		}
	}
	return players[whichPlayer];*/
}

void daBossSanbo::removeMyActivePhysics() {
	for (int i = 0; i < 5; i++) {
		this->bodyPhysics[i].removeFromList();
	}
}

void daBossSanbo::addMyActivePhysics() {
	for (int i = 0; i < 5; i++) {
		this->bodyPhysics[i].addToList();
	}
}

void daBossSanbo::createMyActivePhysics() {
	for (int i = 0; i < 5; i++) {
		ActivePhysics::Info HitMeBaby;
		HitMeBaby.xDistToCenter = 0.0;
		HitMeBaby.yDistToCenter = i * 24 + 12;
		HitMeBaby.xDistToEdge = 12.0;
		HitMeBaby.yDistToEdge = 9.0;
		HitMeBaby.category1 = 0x3;
		HitMeBaby.category2 = 0x0;
		HitMeBaby.bitfield1 = 0x4F;
		//HitMeBaby.bitfield2 = 0x8028E;
		HitMeBaby.bitfield2 = 0xFFBAFFFE;
		HitMeBaby.unkShort1C = 0;
		HitMeBaby.callback = &dEn_c::collisionCallback;

		this->bodyPhysics[i].initWithStruct(this, &HitMeBaby);
	}

	this->addMyActivePhysics();
}