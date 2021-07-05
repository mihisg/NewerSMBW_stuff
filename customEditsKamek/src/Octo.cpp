#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <profile.h>

//This list contains all the arc files from the "Object" folder needed for our sprite
const char* TKarcNameList[] = {
	"Octo",
	NULL
};

//types:
//	0 - Octooomba
//	1 - OctoGuy
//	2 - EliteOctoomba

//The collision callback -> What should be done on collision
void takTakCollisionCallback(ActivePhysics* one, ActivePhysics* two);


class daTakobo : dEn_c {
	int onCreate();			//called once when created
	int onExecute();		//called every frame -> updates the sprite
	int onDelete();			//called once when deleted
	int onDraw();			//called every frame -> draws the model

	int afterExecute(int param);

	mHeapAllocator_c allocator;
	nw4r::g3d::ResFile resFile;		//file our model and animations are contained in
	m3d::mdl_c bodyModel;			//the 3d model
	m3d::anmChr_c chrAnimation;		//the model animation

	mEf::es2 effect;

	int timer;
	int type;
	bool direction;
	int range;
	bool shouldTurn;
	float BaseLine;
	float XSpeed;
	bool isBouncing;
	bool stillFalling;
	char isDying;
	float dying;
	bool frozen;
	u16 randomAmt;
	u32 cmgr_returnValue;
	dStageActor_c* target;
	//more variables...
	ActivePhysics extraPhysics;

	//set a new animation
	void bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate);
	//load model and animations
	void setupBodyModels();
	//update our model - should be called every frame
	void updateModelMatrices();

	//calculate collisions with tiles in the level -> should I turn?
	bool calculateTileCollisions();
	//basic collision functions
	void spriteCollision(ActivePhysics* apThis, ActivePhysics* apOther);
	void playerCollision(ActivePhysics* apThis, ActivePhysics* apOther);
	void yoshiCollision(ActivePhysics* apThis, ActivePhysics* apOther);

	//some more collision functions
	bool collisionCat1_Fireball_E_Explosion(ActivePhysics* apThis, ActivePhysics* apOther);
	bool collisionCat3_StarPower(ActivePhysics* apThis, ActivePhysics* apOther);
	bool collisionCat5_Mario(ActivePhysics* apThis, ActivePhysics* apOther);
	bool collisionCat7_GroundPound(ActivePhysics* apThis, ActivePhysics* apOther);
	bool collisionCat7_GroundPoundYoshi(ActivePhysics* apThis, ActivePhysics* apOther);
	bool collisionCat9_RollingObject(ActivePhysics* apThis, ActivePhysics* apOther);
	bool collisionCatA_PenguinMario(ActivePhysics* apThis, ActivePhysics* apOther);
	bool collisionCatD_Drill(ActivePhysics* apThis, ActivePhysics* apOther);
	bool collisionCat11_PipeCannon(ActivePhysics* apThis, ActivePhysics* apOther);
	bool collisionCat13_Hammer(ActivePhysics* apThis, ActivePhysics* apOther);
	bool collisionCat14_YoshiFire(ActivePhysics* apThis, ActivePhysics* apOther);

	//setDeathInfo_IceBreak -> what should be done when in an iceblock and crashes a wall -> normally the sprite should die
	void _vf148();
	//setDeathInfo_IceVanish -> what should be done when collision with an iceblock thrown by Mario -> normally the sprite should die
	void _vf14C();
	//create an ice block when collided with Iceball
	bool CreateIceActors();

	//which score should be added when the player jumps on it?
	void addScoreWhenHit(void* other);

	//check if sprite is going to fall down in the next step, so turn around before. If returns false, the sprite must turn
	bool canMoveWithoutTurning();

	int nearestPlayer();

	//our sprite uses states
	USING_STATES(daTakobo);

	DECLARE_STATE(Idle);
	DECLARE_STATE(Walk);
	DECLARE_STATE(Turn);
	DECLARE_STATE(Reaction);
	DECLARE_STATE(Shoot);
	DECLARE_STATE(Dizzy);
	DECLARE_STATE(Die);
	DECLARE_STATE(FallDown);
	
	//build function has to be public
	public: static dActor_c* build();
};

const SpriteData OctoSpriteData = { ProfileId::Octo, 8, -0xC, 0, 0, 0x100, 0x100, 0, 0, 0, 0, 0 };
//const SpriteData OctoSpriteData = { ProfileId::Octo, 0x8, -0xC, 0, 0x10, 0x10, 0x40, 0x40, 0x40, 0, 0, 0 };
// #								-ID- ----  -X Offs- -Y Offs-  -RectX1- -RectY1- -RectX2- -RectY2-  -1C- -1E- -20- -22-  Flag ----
Profile OctoProfile(&daTakobo::build, SpriteId::Octo, OctoSpriteData, ProfileId::Octo, ProfileId::Octo, "Octo", TKarcNameList);

dActor_c* daTakobo::build() {
	void* buffer = AllocFromGameHeap1(sizeof(daTakobo));
	return new(buffer) daTakobo;
}

////////////////////////////////
// Extern functions and States 
////////////////////////////////

extern "C" char usedForDeterminingStatePress_or_playerCollision(dEn_c * t, ActivePhysics * apThis, ActivePhysics * apOther, int unk1);
extern "C" void sub_80024C20(void);
extern "C" void __destroy_arr(void*, void(*)(void), int, int);

CREATE_STATE(daTakobo, Idle);
CREATE_STATE(daTakobo, Walk);
CREATE_STATE(daTakobo, Turn);
CREATE_STATE(daTakobo, Reaction);
CREATE_STATE(daTakobo, Shoot);
CREATE_STATE(daTakobo, Dizzy);
CREATE_STATE(daTakobo, Die);
CREATE_STATE(daTakobo, FallDown);

///////////////////////
// Collision functions
///////////////////////

void takTakCollisionCallback(ActivePhysics* one, ActivePhysics* two) {
	//nothing for now
	//if apOther is a projectile shot by a Octoomba, do nothing then
	//else:
	dEn_c::collisionCallback(one, two);
}

void daTakobo::addScoreWhenHit(void* other) { }

void daTakobo::spriteCollision(ActivePhysics* apThis, ActivePhysics* apOther) {
	u16 otherName = ((dEn_c*)apOther->owner)->name;

	if (name == EN_COIN || name == EN_EATCOIN || name == AC_BLOCK_COIN || name == EN_COIN_JUGEM || name == EN_COIN_ANGLE
		|| name == EN_COIN_JUMP || name == EN_COIN_FLOOR || name == EN_COIN_VOLT || name == EN_COIN_WIND
		|| name == EN_BLUE_COIN || name == EN_COIN_WATER || name == EN_REDCOIN || name == EN_GREENCOIN
		|| name == EN_JUMPDAI || name == EN_ITEM)
	{
		return;
	}

	if (acState.getCurrentState() == &StateID_Walk) {
		pos.x = ((pos.x - ((dEn_c*)apOther->owner)->pos.x) > 0) ? pos.x + 1.5 : pos.x - 1.5;
		doStateChange(&StateID_Turn);
	}

	dEn_c::spriteCollision(apThis, apOther);
}

void daTakobo::playerCollision(ActivePhysics* apThis, ActivePhysics* apOther) {
	if (this->isDying == 0) {
		char hitType;
		hitType = usedForDeterminingStatePress_or_playerCollision(this, apThis, apOther, 0);
		if (hitType == 1 || hitType == 3) {		//player performs regular jump or spinning jump on it
			PlaySound(this, SE_EMY_HANACHAN_STOMP);
			doStateChange(&StateID_Dizzy);
		}
		else if (hitType == 0) {	//player is just walking into sprite
			this->_vf220(apOther->owner);
		}

		deathInfo.isDead = 0;
		this->flags_4FC |= (1 << (31 - 7));
		this->counter_504[apOther->owner->which_player] = 0;
	}
}

void daTakobo::yoshiCollision(ActivePhysics* apThis, ActivePhysics* apOther) {
	this->playerCollision(apThis, apOther);
}


bool daTakobo::collisionCat1_Fireball_E_Explosion(ActivePhysics* apThis, ActivePhysics* apOther) { 
	PlaySound(this, SE_EMY_HANACHAN_STOMP);
	doStateChange(&StateID_Dizzy);
	return true;
}
bool daTakobo::collisionCat3_StarPower(ActivePhysics* apThis, ActivePhysics* apOther) { 
	bool ret = dEn_c::collisionCat3_StarPower(apThis, apOther);
	doStateChange(&StateID_FallDown);
	return ret;
}
bool daTakobo::collisionCat5_Mario(ActivePhysics* apThis, ActivePhysics* apOther) { 
	return this->collisionCat9_RollingObject(apThis, apOther);
}
bool daTakobo::collisionCat7_GroundPound(ActivePhysics* apThis, ActivePhysics* apOther) { 
	PlaySound(this, SE_EMY_DOWN);
	SpawnEffect("Wm_en_vshit", 0, &pos, &(S16Vec){0, 0, 0}, & (Vec){1.0, 1.0, 1.0});
	doStateChange(&StateID_Die);
	return true;
}
bool daTakobo::collisionCat7_GroundPoundYoshi(ActivePhysics* apThis, ActivePhysics* apOther) { 
	return this->collisionCat7_GroundPound(apThis, apOther); 
}
bool daTakobo::collisionCat9_RollingObject(ActivePhysics* apThis, ActivePhysics* apOther) { 
	bool ret = dEn_c::collisionCat9_RollingObject(apThis, apOther);
	doStateChange(&StateID_FallDown);
	return ret;
}
bool daTakobo::collisionCatA_PenguinMario(ActivePhysics* apThis, ActivePhysics* apOther) { 
return this->collisionCat5_Mario(apThis, apOther);
}
bool daTakobo::collisionCatD_Drill(ActivePhysics* apThis, ActivePhysics* apOther) {
	PlaySound(this, SE_EMY_DOWN);
	SpawnEffect("Wm_mr_hardhit", 0, &pos, &(S16Vec){0, 0, 0}, & (Vec){1.0, 1.0, 1.0});
	doStateChange(&StateID_Die);
	return true;
}
bool daTakobo::collisionCat11_PipeCannon(ActivePhysics* apThis, ActivePhysics* apOther) {
	return this->collisionCat9_RollingObject(apThis, apOther);
}
bool daTakobo::collisionCat13_Hammer(ActivePhysics* apThis, ActivePhysics* apOther) {
	StageE4::instance->spawnCoinJump(pos, 0, 1, 0);
	return this->collisionCat9_RollingObject(apThis, apOther);
}
bool daTakobo::collisionCat14_YoshiFire(ActivePhysics* apThis, ActivePhysics* apOther) {
	return this->collisionCat1_Fireball_E_Explosion(apThis, apOther);
}

void daTakobo::_vf148() {
	dEn_c::_vf148();
	doStateChange(&StateID_FallDown);
}

void daTakobo::_vf14C() {
	dEn_c::_vf14C();
	doStateChange(&StateID_FallDown);
}

//this does weird shit, but it creates the iceblock around it
bool daTakobo::CreateIceActors() {
	struct DoSomethingCool my_struct = { 0, this->pos, {1.5, 2.0, 1.8}, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
	this->frzMgr.Create_ICEACTORs((void*)&my_struct, 1);
	__destroy_arr((void*)&my_struct, sub_80024C20, 0x3C, 1);
	//this->chrAnimation.setCurrentFrame(0.0);
	this->chrAnimation.setUpdateRate(0.0);
	//this->frozen = true;
	return true;
}

bool daTakobo::calculateTileCollisions() {
	//returns true if the sprite should turn
	HandleXSpeed();
	HandleYSpeed();
	doSpriteMovement();

	cmgr_returnValue = collMgr.isOnTopOfTile();
	collMgr.calculateBelowCollisionWithSmokeEffect();

	if (isBouncing) {
		stuffRelatingToCollisions(0.1875f, 1.0f, 0.5f);
		if (speed.y != 0.0f) { isBouncing = false; }
	}

	float xDelta = pos.x - last_pos.x;
	if (xDelta >= 0.0f) { direction = 0; }
	else { direction = 1; }

	if (collMgr.isOnTopOfTile()) {
		if (cmgr_returnValue == 0) {
			isBouncing = true;
		}
		speed.y = 0.0f;
		max_speed.x = (direction == 1) ? -XSpeed : XSpeed;
	}
	else {
		x_speed_inc = 0.0f;
	}

	//bouncing checks
	if (_34A & 4) {
		Vec v = (Vec){ 0.0f, 1.0f, 0.0f };
		collMgr.pSpeed = &v;

		if (collMgr.calculateAboveCollision(collMgr.outputMaybe)) {
			speed.y = 0.0f;
		}
		collMgr.pSpeed = &speed;
	}
	else {
		if (collMgr.calculateAboveCollision(collMgr.outputMaybe)) {
			speed.y = 0.0f;
		}
	}

	collMgr.calculateAdjacentCollision(0);

	//Switch direction
	if (collMgr.outputMaybe & (0x15 << direction)) {
		if (collMgr.isOnTopOfTile()) {
			isBouncing = true;
		}
		return true;
	}
	return false;
}

bool daTakobo::canMoveWithoutTurning() {
	static const float deltas[] = { 3.0f, -3.0f };
	VEC3 checkWhere = {
			pos.x + deltas[direction],
			4.0f + pos.y,
			pos.z };

	u32 props = collMgr.getTileBehaviour2At(checkWhere.x, checkWhere.y, currentLayerID);

	//if (getSubType(props) == B_SUB_LEDGE)
	if (((props >> 16) & 0xFF) == 8)
		return false;

	float someFloat = 0.0f;
	if (collMgr.sub_800757B0(&checkWhere, &someFloat, currentLayerID, 1, -1)) {
		if (someFloat < checkWhere.y && someFloat >(pos.y - 5.0f))
			return true;
	}

	return false;
}

////////////////////////////
// Animating and bodyModel
////////////////////////////

void daTakobo::bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr(name);
	this->chrAnimation.bind(&this->bodyModel, anmChr, unk);
	this->bodyModel.bindAnim(&this->chrAnimation, unk2);
	this->chrAnimation.setUpdateRate(rate);
}

void daTakobo::setupBodyModels() {
	this->allocator.link(-1, GameHeaps[0], 0, 0x20);

	if (this->type == 0) {
		this->resFile.data = getResource("Octo", "g3d/t00.brres");
	}
	else if (this->type == 1) {
		this->resFile.data = getResource("Octo", "g3d/t00.brres");
	}
	else if (this->type == 2) {
		this->resFile.data = getResource("Octo", "g3d/t00.brres");
	}

	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("Takobo");
	bodyModel.setup(mdl, &this->allocator, 0x224, 1, 0);

	//animations
	//this->anmFile.data = getResource("Octoomba", "g3d/Animations.brres");
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr("Wait");
	this->chrAnimation.setup(mdl, anmChr, &this->allocator, 0);

	allocator.unlink();
}

void daTakobo::updateModelMatrices() {
	if (this->frzMgr._mstate == 1) {
		matrix.translation(pos.x, pos.y + 4.0, pos.z);
	}
	else {
		if (acState.getCurrentState() == &StateID_Walk || acState.getCurrentState() == &StateID_Idle || acState.getCurrentState() == &StateID_Turn) {
			matrix.translation(pos.x, pos.y + 4.0, pos.z);
		}
		else {
			matrix.translation(pos.x, pos.y, pos.z);
		}
	}
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);
}
////////////////////////////
// Main functions
////////////////////////////

int daTakobo::onCreate() {
	this->type = this->settings >> 28 & 0xF;
	this->direction = ((this->settings >> 23) & 0x1) ? 0 : 1;
	this->range = (this->settings >> 8) & 0xFF;
	this->shouldTurn = (this->settings >> 22) & 0x1;
	/*OSReport("type: %i\n", this->type);
	OSReport("direction: %i\n", this->direction);
	OSReport("range: %i\n", this->range);
	OSReport("turn: %i\n", this->shouldTurn);
	*/
	setupBodyModels();


	this->scale = (Vec){ 0.18f, 0.18f, 0.18f };
	this->rot.x = 0;
	this->rot.y = (this->direction) ? 0xE800 : 0x1600;
	this->rot.z = 0;

	this->isDying = 0;
	this->stillFalling = false;
	this->dying = 0.0f;
	this->speed.x = 0.0;
	this->speed.y = 0.0;
	this->BaseLine = this->pos.y;
	OSReport("BaseLine: %f", BaseLine);
	this->XSpeed = 0.6f;
	this->pos.z = 1000.0;
	this->randomAmt = 0;

	ActivePhysics::Info HitMeBaby;

	HitMeBaby.xDistToCenter = 0.0;
	HitMeBaby.yDistToCenter = 12.0;
	HitMeBaby.xDistToEdge = 10.0;
	HitMeBaby.yDistToEdge = 12.0;
	HitMeBaby.category1 = 0x3;
	HitMeBaby.category2 = 0x0;
	HitMeBaby.bitfield1 = 0x4F;
	HitMeBaby.bitfield2 = 0xFFBAFFFE;
	HitMeBaby.unkShort1C = 0;
	HitMeBaby.callback = &takTakCollisionCallback;

	this->aPhysics.initWithStruct(this, &HitMeBaby);
	this->aPhysics.addToList();

	ActivePhysics::Info ExtraInfo;

	ExtraInfo.xDistToCenter = 0.0;
	ExtraInfo.yDistToCenter = 28.0;
	ExtraInfo.xDistToEdge = 10.0;
	ExtraInfo.yDistToEdge = 4.0;
	ExtraInfo.category1 = 0x3;
	ExtraInfo.category2 = 0x0;
	ExtraInfo.bitfield1 = 0x4F;
	ExtraInfo.bitfield2 = 0xFFBAFFFE;
	ExtraInfo.unkShort1C = 0;
	ExtraInfo.callback = &takTakCollisionCallback;

	this->extraPhysics.initWithStruct(this, &ExtraInfo);

	spriteSomeRectX = 28.0f;
	spriteSomeRectY = 32.0f;
	_320 = 0.0f;
	_324 = 16.0f;

	static const lineSensor_s below(-5 << 12, 5 << 12, 0 << 12);
	static const pointSensor_s above(0 << 12, 24 << 12);
	static const lineSensor_s adjacent(8 << 12, 16 << 12, 10 << 12);

	collMgr.init(this, &below, &above, &adjacent);
	collMgr.calculateBelowCollisionWithSmokeEffect();

	cmgr_returnValue = collMgr.isOnTopOfTile();

	if (collMgr.isOnTopOfTile()) { this->isBouncing = false; }
	else { this->isBouncing = true; }

	if (this->type == 0 || this->type == 1) {
		doStateChange(&StateID_Idle);
	}
	else if (this->type == 2) {
		bindAnimChr_and_setUpdateRate("Wait", 1, 0.0, 1.0);
		doStateChange(&StateID_Walk);
	}

	this->onExecute();
	return true;
}

int daTakobo::onDelete() {
	return true;
}

int daTakobo::onExecute() {
	acState.execute();
	updateModelMatrices();
	bodyModel._vf1C();

	return true;
}

int daTakobo::afterExecute(int param) {
	this->extraPhysics.clear();
	return dEn_c::afterExecute(param);
}

int daTakobo::onDraw() {
	bodyModel.scheduleForDrawing();
	return true;
}

void daTakobo::beginState_Idle() {
	this->timer = 0;
	this->rot.y = (direction) ? 0xE800 : 0x1600;
	this->speed.x = 0.0f;
	
	//this->max_speed.y = -4.0;
	//this->speed.y = -4.0;
	//this->y_speed_inc = -0.1875;

	bindAnimChr_and_setUpdateRate("Wait", 1, 0.0, 1.0);
}

void daTakobo::executeState_Idle() {
	this->timer = this->timer + 1;

	int whichPlayer = this->nearestPlayer();
	if (whichPlayer != -1) {
		this->target = GetSpecificPlayerActor(whichPlayer);
		doStateChange(&StateID_Reaction);
	}

	calculateTileCollisions();

	if (this->chrAnimation.getCurrentFrame() == 8.0 || this->chrAnimation.getCurrentFrame() == 33.0) {
		this->extraPhysics.addToList();
	} 
	if(this->chrAnimation.getCurrentFrame() == 17.0 || this->chrAnimation.getCurrentFrame() == 42.0) {
		this->extraPhysics.removeFromList();
	}

	if (this->chrAnimation.isAnimationDone()) {
		this->chrAnimation.setCurrentFrame(0.0);
	}
}

void daTakobo::endState_Idle() {
	
}

void daTakobo::beginState_Walk() {
	this->timer = 0;
	this->rot.y = (direction) ? 0xE800 : 0x1600;

	this->max_speed.x = (direction) ? -this->XSpeed : this->XSpeed;
	this->speed.x = (direction) ? -0.4f : 0.4f;

	this->max_speed.y = -4.0;
	this->speed.y = -4.0;
	this->y_speed_inc = -0.1875;

	if (frozen) {
		bindAnimChr_and_setUpdateRate("Wait", 1, 0.0, 1.0);
		frozen = false;
	}
}

void daTakobo::executeState_Walk() {
	this->timer = this->timer + 1;
	this->pos.x += (direction) ? -0.4 : 0.4;

	if (shouldTurn) {
		if (collMgr.isOnTopOfTile()) {
			stillFalling = false;
			if (!canMoveWithoutTurning()) {
				pos.x = direction ? pos.x + 1.5 : pos.x - 1.5;
				doStateChange(&StateID_Turn);
			}
		}
		else {
			if (!stillFalling) {
				stillFalling = true;
			}
		}
	}

	bool ret = calculateTileCollisions();
	if (ret) {
		doStateChange(&StateID_Turn);
	}

	int whichPlayer = this->nearestPlayer();
	if (whichPlayer != -1) {
		this->target = GetSpecificPlayerActor(whichPlayer);
		doStateChange(&StateID_Reaction);
	}

	if (this->chrAnimation.getCurrentFrame() == 8.0 || this->chrAnimation.getCurrentFrame() == 33.0) {
		this->extraPhysics.addToList();
	}
	if (this->chrAnimation.getCurrentFrame() == 17.0 || this->chrAnimation.getCurrentFrame() == 42.0) {
		this->extraPhysics.removeFromList();
	}

	if (this->chrAnimation.isAnimationDone()) {
		this->chrAnimation.setCurrentFrame(0.0);
	}
}

void daTakobo::endState_Walk() {
	
}

void daTakobo::beginState_Turn() {
	this->direction ^= 1;
	this->speed.x = 0.0;
	this->timer = 0;

	if (frozen) {
		bindAnimChr_and_setUpdateRate("Wait", 1, 0.0, 1.0);
		frozen = false;
	}
}

void daTakobo::executeState_Turn() {
	timer += 1;

	if (this->chrAnimation.isAnimationDone()) {
		this->chrAnimation.setCurrentFrame(0.0);
	}

	if (this->chrAnimation.getCurrentFrame() == 8.0 || this->chrAnimation.getCurrentFrame() == 33.0) {
		this->extraPhysics.addToList();
	}
	if (this->chrAnimation.getCurrentFrame() == 17.0 || this->chrAnimation.getCurrentFrame() == 42.0) {
		this->extraPhysics.removeFromList();
	}

	u16 amt = (this->direction == 0) ? 0x1600 : 0xE800;
	int done = SmoothRotation(&this->rot.y, amt, 0x800);

	if (done) {
		this->doStateChange(&StateID_Walk);
	}
}

void daTakobo::endState_Turn() {

}

void daTakobo::beginState_Reaction() {
	this->timer = 0;

	this->max_speed.x = 0.0;
	this->speed.x = 0.0;
	this->max_speed.y = 0.0;
	this->speed.y = 0.0;
	this->y_speed_inc = 0.0;
	bindAnimChr_and_setUpdateRate("Reaction", 1, 0.0, 1.0);
	this->extraPhysics.removeFromList();
}

void daTakobo::executeState_Reaction() {
	timer += 1;
	if (this->chrAnimation.isAnimationDone()) {
		doStateChange(&StateID_Shoot);
	}

	if (this->target->pos.x > this->pos.x && this->direction == 1) {
		randomAmt = 0x1600;
		SmoothRotation(&this->rot.y, randomAmt, 0x1000);
	}
	else if (this->target->pos.x <= this->pos.x && this->direction == 0) {
		randomAmt = 0xE800;
		SmoothRotation(&this->rot.y, randomAmt, 0x1000);
	}
}

void daTakobo::endState_Reaction() {
	if (this->randomAmt == 0x1600) {
		this->direction = 0;
	}
	else if (this->randomAmt == 0xE800) {
		this->direction = 1;
	}
}

void daTakobo::beginState_Shoot() {
	this->timer = 0;

	this->max_speed.x = 0.0;
	this->speed.x = 0.0;
	this->max_speed.y = 0.0;
	this->speed.y = 0.0;
	this->y_speed_inc = 0.0;
	bindAnimChr_and_setUpdateRate("AttackSign", 1, 0.0, 1.0);
	this->extraPhysics.removeFromList();
}

void daTakobo::executeState_Shoot() {
	this->timer += 1;

	if (this->chrAnimation.isAnimationDone()) {
		if (this->type == 0 || this->type == 1) {
			doStateChange(&StateID_Idle);
		}
		else if (type == 2) {
			bindAnimChr_and_setUpdateRate("Wait", 1, 0.0, 1.0);
			doStateChange(&StateID_Walk);
		}
	}
}

void daTakobo::endState_Shoot() {
	
}

void daTakobo::beginState_Dizzy() {
	this->timer = 0;

	this->max_speed.x = 0.0;
	this->speed.x = 0.0;
	this->max_speed.y = 0.0;
	this->speed.y = 0.0;
	this->y_speed_inc = 0.0;
	bindAnimChr_and_setUpdateRate("Reaction", 1, 0.0, 1.0);
	this->extraPhysics.removeFromList();
}

void daTakobo::executeState_Dizzy() {
	this->timer += 1;
	effect.spawn("Wm_en_spindamage", 0, &(Vec){this->pos.x, this->pos.y + 24.0, 0}, & (S16Vec){0, 0, 0}, & (Vec){1.0, 1.0, 1.0});
	if (this->chrAnimation.isAnimationDone()) {
		if (this->type == 0 || this->type == 1) {
			doStateChange(&StateID_Idle);
		}
		else if (type == 2) {
			bindAnimChr_and_setUpdateRate("Wait", 1, 0.0, 1.0);
			doStateChange(&StateID_Walk);
		}
	}
}

void daTakobo::endState_Dizzy() {

}

void daTakobo::beginState_Die() {
	this->timer = 0;
	this->removeMyActivePhysics();
	this->extraPhysics.removeFromList();
	bindAnimChr_and_setUpdateRate("PressDown", 1, 0.0, 1.0);
}

void daTakobo::executeState_Die() {
	timer += 1;
	if (timer > 60) {
		this->Delete(1);
	}
}

void daTakobo::endState_Die() {

}

void daTakobo::beginState_FallDown() {
	this->timer = 0;
	this->dying = 0.0f;
	this->removeMyActivePhysics();
	this->extraPhysics.removeFromList();
}

void daTakobo::executeState_FallDown() {
	this->timer += 1;
	this->dying = this->dying + 0.05f;
	this->pos.x += 0.15;
	this->pos.y -= (-0.1 * (this->isDying * this->dying)) + 5;

	dEn_c::dieFall_Execute();
	if (this->timer > 450) {
		this->Delete(1);
	}
}

void daTakobo::endState_FallDown() {

}

int daTakobo::nearestPlayer() {
	int players[4];
	int playerCount = 0;

	for (int i = 0; i < 4; i++) {
		if (Player_Active[i] != 0 && Player_Lives[Player_ID[i]] > 0) {
			players[playerCount] = i;
			playerCount++;
		}
	}

	for (int i = 0; i < playerCount; i++) {
		dStageActor_c* currentOne = GetSpecificPlayerActor(players[i]);
		f32 dx = abs(currentOne->pos.x - this->pos.x);
		f32 dy = abs(currentOne->pos.y - this->pos.y);
		if (dx <= this->range && dy < 32.0) {
			return players[i];
		}
	}

	return -1;
}