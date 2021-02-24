#include <common.h>
#include <game.h>
#include <g3dhax.h>


const char* TEarcNameList[] = {
	"testenemy",
	NULL
};


class daTestEnemy : dEn_c {
	int onCreate();
	int onDelete();
	int onExecute();
	int onDraw();

	mHeapAllocator_c allocator;

	nw4r::g3d::ResFile resFile;
	nw4r::g3d::ResFile anmFile;

	m3d::mdl_c bodyModel;
	m3d::anmChr_c chrAnimation;

	//not needed maybe
	mEf::es2 effect;

	//now some variables
	int timer;
	int jumpCounter;
	float dying;
	float BaseLine;
	char damage;
	char isDown;
	Vec initialPos;
	int distance;
	float XSpeed;
	StandOnTopCollider giantRider;
	ActivePhysics Chuckles;

	//needed maybe -> dunno
	bool isBouncing;
	char backFire;
	char spikeTurn;
	int directionStore;
	dStageActor_c* spikeA;
	dStageActor_c* spikeB;
	bool stillFalling;
	int type;


	//creates a new Object of this class
	static daTestEnemy* build();

	//set an animation
	void bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate);
	//void setupBodyModel();

	//update the matrix -> prepare for drawing
	void updateModelMatrices();

	//weird collision stuff
	bool calculateTileCollisions();
	void spriteCollision(ActivePhysics* apThis, ActivePhysics* apOther);
	void playerCollision(ActivePhysics* apThis, ActivePhysics* apOther);
	void yoshiCollision(ActivePhysics* apThis, ActivePhysics* apOther);

	bool collisionCat3_StarPower(ActivePhysics* apThis, ActivePhysics* apOther);
	bool collisionCat14_YoshiFire(ActivePhysics* apThis, ActivePhysics* apOther);
	bool collisionCatD_Drill(ActivePhysics* apThis, ActivePhysics* apOther);
	bool collisionCat7_GroundPound(ActivePhysics* apThis, ActivePhysics* apOther);
	bool collisionCat7_GroundPoundYoshi(ActivePhysics* apThis, ActivePhysics* apOther);
	bool collisionCat9_RollingObject(ActivePhysics* apThis, ActivePhysics* apOther);
	bool collisionCat1_Fireball_E_Explosion(ActivePhysics* apThis, ActivePhysics* apOther);
	bool collisionCat13_Hammer(ActivePhysics* apThis, ActivePhysics* apOther);
	bool collisionCatA_PenguinMario(ActivePhysics* apThis, ActivePhysics* apOther);
	bool collisionCat5_Mario(ActivePhysics* apThis, ActivePhysics* apOther);
	bool collisionCat11_PipeCannon(ActivePhysics* apThis, ActivePhysics* apOther);

	//weird methods I don't understand at the moment. Seem to handle some ice-crap
	void _vf148();
	void _vf14C();
	bool CreateIceActors();
	bool willWalkOnSuitableGround();

	//Finally the states
	//Enable states for this entity
	USING_STATES(daTestEnemy);

	//Declare all necessary states
	DECLARE_STATE(Walk);
	DECLARE_STATE(Turn);
	DECLARE_STATE(Jump);
	DECLARE_STATE(Sleep);
	DECLARE_STATE(Attack);
	DECLARE_STATE(GoDizzy);
	DECLARE_STATE(FireKnockBack);
	DECLARE_STATE(Recover);
	DECLARE_STATE(Die);

	//now we are finished with declaration, let's implement it
}