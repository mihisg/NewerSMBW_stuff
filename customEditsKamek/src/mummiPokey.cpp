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
	float transPeriodes[4] = {
		0.98f, 0.978f, 0.979f, 0.98f
	};
	

	//boss-specific variables
	int timer;
	float BaseLine;
	char dying;
	float xSpeed = 0.6;
	float ySpeed = 0.6;
	Vec initialPos;
	char damage;
	char isInvulnerable;
	dAc_Py_c* target;
	u32 cmgr_returnValue;

	//for building the boss and loading it on the screen
	static daBossSanbo* build();

	//setting up the body model
	void setupBodyModels();

	//for updating the 3d models based on positions and rotation
	void updateModelMatrices();

	//collision functions
	void playerCollision(ActivePhysics* apThis, ActivePhysics* apOther);
	void yoshiCollision(ActivePhysics* apThis, ActivePhysics* apOther);
	bool calculateTileCollisions();

	bool collisionCat3_StarPower(ActivePhysics* apThis, ActivePhysics* apOther);
	bool collisionCat14_YoshiFire(ActivePhysics* apThis, ActivePhysics* apOther);
	bool collisionCatD_Drill(ActivePhysics* apThis, ActivePhysics* apOther);
	bool collisionCat7_GroundPound(ActivePhysics* apThis, ActivePhysics* apOther);
	bool collisionCat7_GroundPoundYoshi(ActivePhysics* apThis, ActivePhysics* apOther);
	bool collisionCat9_RollingObject(ActivePhysics* apThis, ActivePhysics* apOther);
	bool collisionCat1_Fireball_E_Explosion(ActivePhysics* apThis, ActivePhysics* apOther);
	bool collisionCat2_IceBall_15_YoshiIce(ActivePhysics* apThis, ActivePhysics* apOther);
	bool collisionCat13_Hammer(ActivePhysics* apThis, ActivePhysics* apOther);
	bool collisionCatA_PenguinMario(ActivePhysics* apThis, ActivePhysics* apOther);

	//other functions if needed
	int randomPlayer();			//returns a random player of max 4 players. This is the Player the boss will follow
	//void addScoreWhenHit(void* other);
	void updateAllPositions();
	void smoothMovement(int timer);
	bool IntroduceBoss(daKameckDemo* Kameck, int timer);

	//Now tell the game that we are using states in this class
	USING_STATES(daBossSanbo);

	//Create our actual states
	DECLARE_STATE(Intro);
	DECLARE_STATE(Walk);
	//DECLARE_STATE(Spit);
	//DECLARE_STATE(GoDown);
	//DECLARE_STATE(ComeUp);
	//DECLARE_STATE(Damage);
	//DECLARE_STATE(Wait);
	//DECLARE_STATE(Outro);
};


///
/// Externs and states
///

CREATE_STATE(daBossSanbo, Intro);
CREATE_STATE(daBossSanbo, Walk);
//CREATE_STATE(daBossSanbo, Spit);
//CREATE_STATE(daBossSanbo, GoDown);
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

void daBossSanbo::playerCollision(ActivePhysics* apThis, ActivePhysics* apOther) {
	DamagePlayer(this, apThis, apOther);
}

void daBossSanbo::yoshiCollision(ActivePhysics* apThis, ActivePhysics* apOther) {
	DamagePlayer(this, apThis, apOther);
}

bool daBossSanbo::collisionCat3_StarPower(ActivePhysics* apThis, ActivePhysics* apOther) {
	return true;
}

bool daBossSanbo::collisionCat14_YoshiFire(ActivePhysics* apThis, ActivePhysics* apOther) {
	return true;
}

bool daBossSanbo::collisionCatD_Drill(ActivePhysics* apThis, ActivePhysics* apOther) {
	return this->collisionCat7_GroundPound(apThis, apOther);
}

bool daBossSanbo::collisionCat7_GroundPoundYoshi(ActivePhysics* apThis, ActivePhysics* apOther) {
	return this->collisionCat7_GroundPound(apThis, apOther);
}

bool daBossSanbo::collisionCat9_RollingObject(ActivePhysics* apThis, ActivePhysics* apOther) {

	dActor_c* block = apOther->owner;
	dEn_c* blah = (dEn_c*)block;

	if (block->name == AC_LIGHT_BLOCK) {
		if (blah->_12C & 3 || strcmp(blah->acState.getCurrentState()->getName(), "daLightBlock_c::StateID_Throw")) {
			return true;
		}
	}

	if (blah->direction == 0) { blah->direction = 1; }
	else { blah->direction = 0; }

	blah->speed.x = -blah->speed.x;
	blah->pos.x += blah->speed.x;

	if (blah->speed.y < 0) {
		blah->speed.y = -blah->speed.y;
	}

	blah->doSpriteMovement();
	blah->doSpriteMovement();

	if (this->isInvulnerable == 1) {

		if (blah->direction == 0) { blah->direction = 1; }
		else { blah->direction = 0; }

		return true;
	}

	this->pos.x -= blah->speed.x;
	updateAllPositions();

	this->timer = 0;
	this->damage = this->damage + 5;


	PlaySound(this, SE_EMY_DOWN);
	S16Vec nullRot = { 0,0,0 };
	Vec oneVec = { 1.0f, 1.0f, 1.0f };
	SpawnEffect("Wm_mr_kickhit", 0, &blah->pos, &nullRot, &oneVec);

	//for testing
	this->isInvulnerable = 1;

	//if (this->damage > 14) { doStateChange(&StateID_Outro); }
	//else { doStateChange(&StateID_Damage); }
	return true;
}

bool daBossSanbo::collisionCat1_Fireball_E_Explosion(ActivePhysics* apThis, ActivePhysics* apOther) {
	return true;
}

bool daBossSanbo::collisionCat2_IceBall_15_YoshiIce(ActivePhysics* apThis, ActivePhysics* apOther) {
	return true;
}

bool daBossSanbo::collisionCatA_PenguinMario(ActivePhysics* apThis, ActivePhysics* apOther) {
	DamagePlayer(this, apThis, apOther);
	return true;
}

bool daBossSanbo::collisionCat13_Hammer(ActivePhysics* apThis, ActivePhysics* apOther) {
	return true;
}

bool daBossSanbo::collisionCat7_GroundPound(ActivePhysics* apThis, ActivePhysics* apOther) {

	if (this->isInvulnerable == 1) {
		DamagePlayer(this, apThis, apOther);
		return true;
	}

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

	this->damage += 5;
	PlaySoundAsync(this, SE_EMY_DOWN_BY_PUNCH);
	if (this->damage > 14) {
		//doStateChange(&StateID_Outro);
	}
	else {
		//doStateChange(&StateID_Damage);
	}
	return true;
}

bool daBossSanbo::calculateTileCollisions() {
	HandleXSpeed();
	HandleYSpeed();
	doSpriteMovement();

	cmgr_returnValue = collMgr.isOnTopOfTile();
	collMgr.calculateBelowCollisionWithSmokeEffect();

	if (collMgr.isOnTopOfTile()) {
		speed.y = 0.0f;
	}
	
	collMgr.calculateAdjacentCollision(0);

	if (collMgr.outputMaybe & (0x15 << direction)) {
		pos.x += direction ? 2.0 : -2.0;
		updateAllPositions();
		return true;
	}
	return false;
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

	
	ActivePhysics::Info HitMeBaby;
	HitMeBaby.xDistToCenter = 0.0;
	HitMeBaby.yDistToCenter = 50.0;
	HitMeBaby.xDistToEdge = 12.0;
	HitMeBaby.yDistToEdge = 60.0;
	HitMeBaby.category1 = 0x3;
	HitMeBaby.category2 = 0x0;
	HitMeBaby.bitfield1 = 0x4F;
	HitMeBaby.bitfield2 = 0x8028E;
	HitMeBaby.unkShort1C = 0;
	HitMeBaby.callback = &dEn_c::collisionCallback;

	this->aPhysics.initWithStruct(this, &HitMeBaby);
	this->aPhysics.addToList();
	

	speed.x = 0.0;
	speed.y = 0.0;
	this->pos.y -= 145;
	this->pos.z = -3000.0f;
	this->initialPos = this->pos;
	this->BaseLine = this->pos.y;
	this->isInvulnerable = 0;
	this->damage = 0;
	this->dying = 0;
	this->disableEatIn();

	this->rot.x = 0; // X is vertical axis
	this->rot.y = 0; // Y is horizontal axis
	this->rot.z = 0; // Z is ... an axis >.>
	this->bodyRotations[4].y = 0xE800;
	this->direction = 0;

	//posisitons
	updateAllPositions();

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
	
	for (int i = 0; i < 4; i++) {
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

	for (int i = 0; i < 4; i++) {
		mMtx partMatrix;
		Vec partPos = this->bodyPositions[i];
		S16Vec partRot = this->bodyRotations[i];

		partMatrix.translation(partPos.x, partPos.y, partPos.z);
		partMatrix.applyRotationYXZ(&partRot.x, &partRot.y, &partRot.z);

		this->bodyModels[i].setDrawMatrix(partMatrix);
		this->bodyModels[i].setScale(&scale);
		this->bodyModels[i].calcWorld(false);
	}

	Vec* headPos = &this->bodyPositions[4];
	S16Vec* headRot = &this->bodyRotations[4];

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
	this->pos.z = 3000.0;
	updateAllPositions();
}

void daBossSanbo::beginState_Walk() {
	this->timer = 0;
	//S16Vec nullRot = { 0,0,0 };
	//Vec oneVec = { 1.0f, 1.0f, 1.0f };
	//SpawnEffect("Wm_en_sanbospillsand", 0, &this->pos, &nullRot, &oneVec);
	this->target = GetSpecificPlayerActor(this->randomPlayer());
	this->speed.x = (this->direction) ? this->xSpeed : -this->xSpeed;
}

void daBossSanbo::executeState_Walk() {
	this->timer += 1;
	u8 lastDir = this->direction;

	if (this->target->pos.x > this->pos.x && this->direction == 0) {
		this->speed.x += 0.008;
		int done = SmoothRotation(&this->bodyRotations[4].y, 0x1800, 0x500);
		if (done) {
			this->direction = 1;
			this->speed.x = this->xSpeed;
		}
	} 
	else if(this->target->pos.x < this->pos.x && this->direction == 1) {
		this->speed.x -= 0.008;
		int done = SmoothRotation(&this->bodyRotations[4].y, 0xE800, 0x500);
		if (done) {
			this->direction = 0;
			this->speed.x = -this->xSpeed;
		}
	}

	this->pos.x += this->speed.x;

	smoothMovement(this->timer);
	//rotate body
		
	
}

void daBossSanbo::endState_Walk() {

}





bool daBossSanbo::IntroduceBoss(daKameckDemo* Kameck, int timer) {
	if (timer == 130) { this->Kameck->doStateChange(&daKameckDemo::StateID_DemoSt); }
	if (timer == 400) { this->Kameck->doStateChange(&daKameckDemo::StateID_DemoSt2); }

	if (timer == 150) { PlaySound(this, SE_EMY_MEGA_BROS_EARTHQUAKE); }

	if ((timer > 150) && (timer < 230)) {
		//ShakeScreen(this, 2, 1, 1, 0);
		this->pos.y += 4.7 - 0.07 * (timer - 150);
		updateAllPositions();
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

void daBossSanbo::updateAllPositions() {
	for (int i = 0; i < 5; i++) {
		this->bodyPositions[i].x = this->pos.x;
		this->bodyPositions[i].z = this->pos.z + 100 * i;

		this->bodyPositions[i].y = this->pos.y + 24.0 * i;
	}
}

void daBossSanbo::smoothMovement(int timer) {
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
				tmp = 0;
			}
			this->bodyPositions[i].x = sin(2 * (3.14 / this->transPeriodes[i - 1]) * timer + tmp) * 2 + this->pos.x;
		}
		this->bodyPositions[i].y = this->pos.y + 24.0 * i;
		this->bodyPositions[i].z = this->pos.z + 100 * i;
	}

	//rotation
	for (int i = 0; i < 5; i++) {
		if (i == 4) {
			//head rotation
		}
		else {
			char tmp;
			if (i % 2 == 0) {
				tmp = 1;
			}
			else {
				tmp = 0;
			}
			this->bodyRotations[i].y = sin(2 * (3.14 / this->transPeriodes[i - 1]) * timer + tmp) * 2 + this->rot.y;
		}
	}
	
}

int daBossSanbo::randomPlayer() {
	int players[4];
	int playerCount = 0;

	for (int i = 0; i < 4; i++) {
		if (Player_Active[i] != 0 && Player_Lives[Player_ID[i]] > 0) {
			players[playerCount] = i;
			playerCount++;
		}
	}
	return players[MakeRandomNumber(playerCount)];
}