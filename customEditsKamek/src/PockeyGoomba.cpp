#include <common.h>
#include <game.h>
#include <g3dhax.h>


const char* TEarcNameList[] = {
	"kuribo",
	"sanbo",
	"wing",
	NULL
};


class daPockeyGoomba : dEn_c {
	int onCreate();
	int onDelete();
	int onExecute();
	int onDraw();

	mHeapAllocator_c allocator;
	nw4r::g3d::ResFile resFile;
	m3d::mdl_c bodyModel;
	m3d::mdl_c burstModel;

	dStageActor_c* Goomba;
	u32 timer;
	bool is Bursting;

	static daPockeyGoomba* build();

	void playerCollision(ActivePhysics* apThis, ActivePhysics* apOther);
	void yoshiCollision(ActivePhysics* apThis, ActivePhysics* apOther);
	void spriteCollision(ActivePhysics* apThis, ActivePhysics* apOther);

	bool collisionCat3_StarPower(ActivePhysics* apThis, ActivePhysics* apOther);
	bool collisionCat14_YoshiFire(ActivePhysics* apThis, ActivePhysics* apOther);
	bool collisionCatD_Drill(ActivePhysics* apThis, ActivePhysics* apOther);
	bool collisionCat7_GroundPound(ActivePhysics* apThis, ActivePhysics* apOther);
	bool collisionCat7_GroundPoundYoshi(ActivePhysics* apThis, ActivePhysics* apOther);
	bool collisionCat9_RollingObject(ActivePhysics* apThis, ActivePhysics* apOther);
	bool collisionCat1_Fireball_E_Explosion(ActivePhysics* apThis, ActivePhysics* apOther);
	bool collisionCat13_Hammer(ActivePhysics* apThis, ActivePhysics* apOther);
	bool collisionCatA_PenguinMario(ActivePhysics* apThis, ActivePhysics* apOther);

	void _vf148();
	void _vf14C();
	bool CreateIceActors();
	void addScoreWhenHit(void* other);

	USING_STATES(daPockeyGoomba);
	DECLARE_STATE(Follow);
	DECLARE_STATE(Burst);
};


daPockeyGoomba* daPockeyGoomba::build() {
	void* buffer = AllocFromGameHeap1(sizeof(daPockeyGoomba));
	return new(buffer) daPockeyGoomba;
}