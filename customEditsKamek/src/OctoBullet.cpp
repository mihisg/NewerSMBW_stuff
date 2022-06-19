#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <profile.h>

const char* TBarcNameList[] = { "octo", NULL };

//TODO: explosion Soundeffect

class daTakoboBu : dEn_c {
	int onCreate();
	int onExecute();
	int onDraw();
	int onDelete();

	mHeapAllocator_c allocator;
	nw4r::g3d::ResFile resFile;
	m3d::mdl_c bodyModel;

	bool isDying;
	int deathTimer;
	bool removeJustOnce;
	int movingDir;
	f32 initPosX;

	void updateModelMatrix();

	void playerCollision(ActivePhysics* apThis, ActivePhysics* apOther);
	void yoshiCollision(ActivePhysics* apThis, ActivePhysics* apOther);

	bool collisionCat3_StarPower(ActivePhysics* apThis, ActivePhysics* apOther);
	bool collisionCat5_Mario(ActivePhysics* apThis, ActivePhysics* apOther);
	bool collisionCatD_Drill(ActivePhysics* apThis, ActivePhysics* apOther);
	bool collisionCat8_FencePunch(ActivePhysics* apThis, ActivePhysics* apOther);
	bool collisionCat7_GroundPound(ActivePhysics* apThis, ActivePhysics* apOther);
	bool collisionCat7_GroundPoundYoshi(ActivePhysics* apThis, ActivePhysics* apOther);
	bool collisionCatA_PenguinMario(ActivePhysics* apThis, ActivePhysics* apOther);
	bool collisionCat11_PipeCannon(ActivePhysics* apThis, ActivePhysics* apOther);
	bool collisionCat9_RollingObject(ActivePhysics* apThis, ActivePhysics* apOther);
	bool collisionCat1_Fireball_E_Explosion(ActivePhysics* apThis, ActivePhysics* apOther);
	bool collisionCat2_IceBall_15_YoshiIce(ActivePhysics* apThis, ActivePhysics* apOther);
	bool collisionCat13_Hammer(ActivePhysics* apThis, ActivePhysics* apOther);
	bool collisionCat14_YoshiFire(ActivePhysics* apThis, ActivePhysics* apOther);

public: static dActor_c* build();
};

// #								-ID- ----  -X Offs- -Y Offs-  -RectX1- -RectY1- -RectX2- -RectY2-  -1C- -1E- -20- -22-  Flag ----
const SpriteData OctoBulletSpriteData = { ProfileId::OctoBullet, 0, 0, 0, 0, 0x100, 0x100, 0, 0, 0, 0, 0 };
Profile OctoBulletProfile(&daTakoboBu::build, SpriteId::OctoBullet, &OctoBulletSpriteData, ProfileId::OctoBullet, ProfileId::OctoBullet, "OctoBullet", TBarcNameList);

dActor_c* daTakoboBu::build() {
	void* buffer = AllocFromGameHeap1(sizeof(daTakoboBu));
	return new(buffer) daTakoboBu;
}

void daTakoboBu::playerCollision(ActivePhysics* apThis, ActivePhysics* apOther) {
	char hitType;
	hitType = usedForDeterminingStatePress_or_playerCollision(this, apThis, apOther, 0);

	if (hitType == 1 || hitType == 3)	//normal jump or spin jump
	{
		apOther->someFlagByte |= 2;
		PlaySound(this, SE_EMY_DOWN);
		isDying = true;
	}
	else if (hitType == 0) {
		this->_vf220(apOther->owner);
		//explosion sound
		SpawnEffect("Wm_en_sphitsmoke", 0, &pos, &(S16Vec){0, 0, 0}, & (Vec){0.3, 0.3, 0.3});
		this->removeMyActivePhysics();
		this->Delete(1);
	}

	//weird stuff
	deathInfo.isDead = 0;
	this->flags_4FC |= (1 << (31 - 7));
	this->counter_504[apOther->owner->which_player] = 0;
}

void daTakoboBu::yoshiCollision(ActivePhysics* apThis, ActivePhysics* apOther) {
	this->playerCollision(apThis, apOther);
}

bool daTakoboBu::collisionCat3_StarPower(ActivePhysics* apThis, ActivePhysics* apOther) {
	PlaySound(this, SE_EMY_DOWN);
	isDying = true;
	return true;
}

bool daTakoboBu::collisionCat5_Mario(ActivePhysics* apThis, ActivePhysics* apOther) {
	this->_vf220(apOther->owner);
	return true;
}

bool daTakoboBu::collisionCatD_Drill(ActivePhysics* apThis, ActivePhysics* apOther) {
	return this->collisionCat3_StarPower(apThis, apOther);
}

bool daTakoboBu::collisionCat8_FencePunch(ActivePhysics* apThis, ActivePhysics* apOther) {
	playerCollision(apThis, apOther);
	return true;
}

bool daTakoboBu::collisionCat7_GroundPound(ActivePhysics* apThis, ActivePhysics* apOther) {
	return this->collisionCat3_StarPower(apThis, apOther);
}

bool daTakoboBu::collisionCat7_GroundPoundYoshi(ActivePhysics* apThis, ActivePhysics* apOther) {
	return this->collisionCat3_StarPower(apThis, apOther);
}

bool daTakoboBu::collisionCatA_PenguinMario(ActivePhysics* apThis, ActivePhysics* apOther) {
	return this->collisionCat5_Mario(apThis, apOther);
}

bool daTakoboBu::collisionCat11_PipeCannon(ActivePhysics* apThis, ActivePhysics* apOther) {
	return this->collisionCat3_StarPower(apThis, apOther);
}

bool daTakoboBu::collisionCat9_RollingObject(ActivePhysics* apThis, ActivePhysics* apOther) {
	return true;
}

bool daTakoboBu::collisionCat1_Fireball_E_Explosion(ActivePhysics* apThis, ActivePhysics* apOther) {
	return true;
}

bool daTakoboBu::collisionCat2_IceBall_15_YoshiIce(ActivePhysics* apThis, ActivePhysics* apOther) {
	return true;
}

bool daTakoboBu::collisionCat13_Hammer(ActivePhysics* apThis, ActivePhysics* apOther) {
	return true;
}

bool daTakoboBu::collisionCat14_YoshiFire(ActivePhysics* apThis, ActivePhysics* apOther) {
	return true;
}

void daTakoboBu::updateModelMatrix() {
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);
}

int daTakoboBu::onCreate() {
	this->isDying = false;
	this->deathTimer = 0;
	this->removeJustOnce = false;
	this->initPosX = pos.x;
	this->movingDir = (settings == 0) ? 0 : 1;

	allocator.link(-1, GameHeaps[0], 0, 0x20);

	this->resFile.data = getResource("octo", "g3d/t00.brres");
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("TakoboBullet");
	bodyModel.setup(mdl, &allocator, 0x224, 1, 0);
	SetupTextures_Enemy(&bodyModel, 0);

	allocator.unlink();

	ActivePhysics::Info physicsInfo;
	physicsInfo.xDistToCenter = 0.0;
	physicsInfo.yDistToCenter = 0.0;
	physicsInfo.xDistToEdge = 8.0;
	physicsInfo.yDistToEdge = 8.0;
	physicsInfo.category1 = 0x3;
	physicsInfo.category2 = 0x0;
	physicsInfo.bitfield1 = 0x4F;
	physicsInfo.bitfield2 = 0xFFBAFFFE;
	physicsInfo.unkShort1C = 0;
	physicsInfo.callback = &dEn_c::collisionCallback;
	this->aPhysics.initWithStruct(this, &physicsInfo);
	this->aPhysics.addToList();

	this->scale.x = 0.1;
	this->scale.y = 0.1;
	this->scale.z = 0.1;

	this->onExecute();

	return true;
}

int daTakoboBu::onExecute() {
	if (isDying) {
		deathTimer += 1;
		if (!removeJustOnce) {
			this->removeMyActivePhysics();
			removeJustOnce = true;
		}
		if (deathTimer > 300) {
			this->Delete(1);
		}
		dEn_c::dieFall_Execute();
	}
	else {
		pos.x += (movingDir == 1) ? -1.0f : 1.0f;
		if ((pos.x - initPosX) * (pos.x - initPosX) > 40000) {
			//explosion sound
			SpawnEffect("Wm_en_sphitsmoke", 0, &pos, &(S16Vec){0, 0, 0}, & (Vec){0.3, 0.3, 0.3});
			this->removeMyActivePhysics();
			this->Delete(1);
		}
	}
	
	updateModelMatrix();

	return true;
}

int daTakoboBu::onDelete() {
	return true;
}

int daTakoboBu::onDraw() {
	bodyModel.scheduleForDrawing();
	return true;
}