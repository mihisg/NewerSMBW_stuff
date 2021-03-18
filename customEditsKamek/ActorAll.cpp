/////////////
///fBase_c///
/////////////


//all of those are virtual except for Delete/destroy
int fBase_c::beforeCreate() { return 1; }
int fBase_c::onCreate() { return 1; }
int fBase_c::afterCreate(int param) {
	//if param == 0 or param == 3 (beforeCreate returned 0, beforeCreate returned true and onCreate returned 0), do nothing
	
	//if beforeCreate returned true and onCreate returned 1
	if(param == 2) {
		List_Remove(&Create_Related, &this->link_created);
		List_Add_Sorted(&Execute_Related, &this->link_execute);
		List_Add_Sorted(&Draw_Related, &this->link_draw);
		this->created = true;
	} else if(param == 1) {
		//beforeCreate returned true and onCreate returned any other value than 0 or 1
		fBase_c::destroy();
	}
	return 0;			//the after... functions are actually "void"
}

int fBase_c::beforeDelete() {
	int ivar = fBase_c::getChild();
	if(ivar == 0) {
		return 1;
	} else {
		return 0;
	}
}
int fBase_c::onDelete() { return 1; }
int fBase_c::afterDelete(int param) {
	//if param == 0 or param == 3 (beforeDelete returned 0, beforeDelete returned true and onDelete returned 0), do nothing
	//also if param == 1(beforeDelete returned true and onDelete returned any other value than 0 or 1), do nothing 

	if(param == 2) {
		//if beforeDelete returned true an onDelete returned 1
		List_Remove(&Delete_Related);
	}
	return 0;		//the after... functions are actually "void"
}

int fBase_c::beforeExecute() { return 1; }				//does not always do this
int fBase_c::onExecute() { return 1; }
int fBase_c::afterExecute(int param) { return 0; }		//does actually return void

int fBase_c::beforeDraw() { return 1; }					//does not always do this, like beforeExecute
int fBase_c::onDraw() { return 1; }
int fBase_c::afterDraw(int param) { return 0; }		//does actually return void

void fBase_c::willBeDeleted() { return; }
void fBase_c::destroy() { /*delete me and repeat this for all children*/ }	//called "Delete" in game.h


/////////////
///dBase_c///
/////////////


dBase_c::dBase_c() {
	fBase_c::fBase_c();
	//set vtable and explanation string
	//get name from list
}

dBase_c::~dBase_c() {
	fBase_c::~fBase_c();
	FreeFromGameHeap1(this);
}


int dBase_c::beforeCreate() {
	int var = fBase_c::beforeCreate();
	return (-var | var) >> 0x1F;
	//if returns 0, onCreate is ignored and afterCreate receives 0 as parameter
	//if returns not 0, onCreate is called and depending what it returns afterCreate is called with the apropriate param
}

int dBase_c::afterCreate(int param) {
	//if param == 0 or param == 3 (beforeCreate returned 0, beforeCreate returned true and onCreate returned 0), do nothing
	
	//if beforeCreate returned true and onCreate returned 1
	if(param == 2) {
		List_Remove(&Create_Related, &this->link_created);
		List_Add_Sorted(&Execute_Related, &this->link_execute);
		List_Add_Sorted(&Draw_Related, &this->link_draw);
		this->created = true;
	} else if(param == 1) {
		//beforeCreate returned true and onCreate returned any other value than 0 or 1
		fBase_c::destroy();
	}
	return 0;			//the after... functions are actually "void"
}


int dBase_c::beforeDelete() {
	int var = fBase_c::beforeDelete();
	return (-var | var) >> 0x1F;
	//if returns 0, onDelete is ignored and afterDelete receives 0 as parameter
	//if returns not 0, onDelete is called and depending what it returns afterDelete is called with the apropriate param
}

int dBase_c::afterDelete(int param) {
	//if param == 0 or param == 3 (beforeDelete returned 0, beforeDelete returned true and onDelete returned 0), do nothing
	//also if param == 1(beforeDelete returned true and onDelete returned any other value than 0 or 1), do nothing 

	if(param == 2) {
		//if beforeDelete returned true and onDelete returned 1
		List_Remove(&Delete_Related);
	}
	return 0;		//the after... functions are actually "void"
}


int dBase_c::beforeExecute() {
	fBase_c::beforeExecute();
	//if returns 0, onExecute is ignored and afterExecute receives 0 as parameter
	//if returns not 0, onExecute is called and depending what it returns afterExecute is called with the apropriate param
}

int dBase_c::afterExecute(int param) {
	return 0;		//the after... functions are actually "void"
}


int dBase_c::beforeDraw() {
	fBase_c::beforeDraw();
	//if returns 0, onDraw is ignored and afterDraw receives 0 as parameter
	//if returns not 0, onDraw is called and depending what it returns afterDraw is called with the apropriate param
}

int dBase_c::afterDraw(int param) {
	return 0;		//the after... functions are actually "void"
}

fBase_c* dBase_c::createChild(Actors type, fBase_c* parent, u32 settings, int unk) {
	if(parent == (fBase_c*)0x0) {
		//I don't have a parent
		return (fBase_c*)0x0;
	} else {
		return fBase_c::createChildUnderTree(type, parent, settings, unk);		//where is this function??
	}
}


//////////////
///dActor_c///
//////////////


dActor_c::dActor_c() {
	dBase_c::dBase_c();
	id = 0;
	settings = 0;
	//set name and other variables like drawOrder 
}

dActor_c::~dActor_c() {
	dBase_c::~dBase_c();
	FreeFromGameHeap1(this);
}

int dActor_c::beforeExecute() {
	int var2 = dBase_c::beforeExecute();
	if(var2 == 0) {
		return 0;
	} else {
		//return 1 or some other value
	}
}

int dActor_c::beforeDraw() {
	int var = dBase_c::beforeDraw();
	if(var == 0) {
		return 0;
	} else {
		//return something else than 0
	}
}

void dActor_c::specialDraw1() { return; }
void dActor_c::specialDraw2() { return; }
void dActor_c::UpdateObjectPosBasedOnSpeedValuesReal() {
	pos.x += speed.x;
	pos.y += speed.y;
}

void dActor_c::handleXSpeed() {
	float max = this->max_speed.x;
	float newSpeed = this->speed.x + this->x_speed_inc;
	char neg;
	if(newSpeed < 0) { neg = true; } else { neg = false; }
	if(abs(newSpeed) > max) {
		newSpeed = max;
		if(neg) { newSpeed = -newSpeed; }
	}
	this->speed.x = newSpeed;
}

void dActor_c::handleYSpeed() {
	float max = this->max_speed.y;
	float newSpeed = this->speed.y + this->y_speed_inc;
	if(newSpeed < max) {
		newSpeed = max;
	}
	this->speed.y = newSpeed;
}

void dActor_c::create(Actors type, u32 settings, Vec* pos, S16Vec* rot) {
	dActor_c::setNextState(pos, rot);
	fBase_c* parent = SearchForItemByClassType_Wrap(current_scene, (fBase_c*)0x0);
	dBase_c::createChild(type, parent, settings, 2);
}




///////////////////
///dStageActor_c///
///////////////////

dStageActor_c::dStageActor_c() {
	dActor_c::dActor_c();
	//init ActivePhysics 
	//init collDetector
	//do lots of initialization stuff
}

dStageActor_c::~dStageActor_c() {
	ActivePhysics::remove(aPhysics);
	dActor_c::~dActor_c();
	FreeFromGameHeap1(this);
}

int dStageActor_c::beforeCreate() {
	int var = dActor_c::beforeCreate();
	return (-var | var) >> 0x1F;
}

int dStageActor_c::afterCreate(int param) {
	//if param == 0 or param == 3 (beforeCreate returned 0, beforeCreate returned true and onCreate returned 0), do nothing
	
	//if beforeCreate returned true and onCreate returned 1
	if(param == 2) {
		List_Remove(&Create_Related, &this->link_created);
		List_Add_Sorted(&Execute_Related, &this->link_execute);
		List_Add_Sorted(&Draw_Related, &this->link_draw);
		this->created = true;
	} else if(param == 1) {
		//beforeCreate returned true and onCreate returned any other value than 0 or 1
		fBase_c::destroy();
	}
	return 0;			//the after... functions are actually "void"
}

int dStageActor_c::beforeDelete() {
	int var = dActor_c::beforeDelete();
	if(var != 0) {
		this->field_134 = 0;
	}
	return var != 0;
}

int dStageActor_c::afterDelete(int param) {
	if(param == 2) {
		List_Remove(&Delete_Related);
	}
	return 0;			//the after... functions are actually "void"
}

int dStageActor_c::beforeExecute() {
	int var = dActor_c::beforeExecute();
	//return 0 or 1 based on strange calculations
}

int dStageActor_c::afterExecute(int param) {
	if(param == 2) {
		aPhysics.clear();
		//some other stuff
	}
	dActor_c::afterExecute(param);
	return 0;			//the after... functions are actually "void"
}

int dStageActor_c::beforeDraw() {
	int var = dActor_c::beforeDraw();
	if(var == 0) {
		return 0;
	} else {
		//return 0 or 1 based on strange calculations
	}
}

int dStageActor_c::afterDraw(int param) {
	return 0;			//the after... functions are actually "void"
}

void dStageActor_c::kill() { return; }
void dStageActor_c::disableEatIn() { return; }
void dStageActor_c::removeMyActivePhysics() {
	aPhysics.removeFromList();
}
void dStageActor_c::addMyActivePhysics() {
	aPhysics.addToList();
}

dStageActor_c* dStageActor_c::create(Actors type, u32 settings, Vec* pos, S16Vec* rot, u8 layer) {
	dStageActor_c::setNextLayerID(layer);
	dStageActor_c* var = (dStageActor_c*) dActor_c::create(type, settings, pos, rot);
	return var;
}

void dStageActor_c::Delete(int param) {
	fBase_c::destroy();
	//other weird stuff
}




////////////////////////
///dActorMultiState_c///
////////////////////////

//overridden by dEn_c
void dActorMultiState_c::doStateChange(dStateBase_c* state) {
	return;
}



///////////
///dEn_c///
///////////

dEn_c::dEn_c() {
	dStageActor_c::dStageActor_c();
	//initialize acState
	//initialize dStateMgr_c
	//set some fields
	//set velocity to 0.0f
}

dEn_c::~dEn_c() {
	dActorMultiState_c::~dActorMultiState_c();
	FreeFromGameHeap1(this);
}

void dEn_c::afterCreate(int param) {
	dEn_c::copyInitialScale();
	dStageActor_c::afterCreate(param);
}

int dEn_c::beforeExecute() {
	int var = dStageActor_c::beforeExecute();
	if(var == 0) {
		return 0;
	} else {
		//first do something with this->deleteForever + &dEn_c__StateID_Ice
		//set some weird things on the aPhysics
		//return something else than 0
	}
}

void dEn_c::afterExecute(int param) {
	//set something to 0?
	dStageActor_c::afterExecute(param);
}

int dEn_c::beforeDraw() {
	int var = dStageActor_c::beforeDraw();
	return (-var | var) >> 0x1F;
}

void dEn_c::kill() {
	dEn_c::spawnHitEffectAtPositionNonVirtual();
	ConvertStagePositionIntoScreenPosition__Maybe();
	AnotherSoundRelatedFunction();
	doStateChange(&StateID_DieFall);
}

int dEn_c::preSpriteCollision(ActivePhysics* apThis, ActivePhysics* apOther) {
	if(apOther->category1 == 9) {
		dEn_c::collidedWithCat9_RollingObject(apThis, apOther);
		return 1;
	} else {
		return 0;
	}
}

int dEn_c::prePlayerCollision(ActivePhysics* apThis, ActivePhysics* apOther) {
	if(apOther->owner->stageActorType == 1) {
		if(apOther->category1 == 3) {
			dEn_c::collidedWithCat3_StarPower(apThis, apOther);
			return 1;
		} else if(apOther->category1 == 7) {
			if(daPlBase_c::canGroundPound(this) == 0) {
				dEn_c::collidedWithCat7_GroundPound(apThis, apOther);
				return 1;
			} else {
				return 0;
			}
		} else if(apOther->category1 == 5) {
			if(apThis->bitfield2 & 200000 == 0) {
				return 0;
			} else {
				dEn_c::collidedWithCat5_MarioWut(apThis, apOther);
				return 1;
			}
		} else if(apOther->category1 == 0xa) {
			if(apThis->bitfield2 & 0x400 == 0) {
				return 0;
			} else {
				dEn_c::collidedWithCatA_PenguinMario(apThis, apOther);
				return 1;
			}
		} else if(apOther->category1 == 0xd) {
			if(apThis->bitfield2 & 0x2000 == 0) {
				return 0;
			} else {
				dEn_c::collidedWithCatD_Drill(apThis, apOther);
				return 1;
			}
		} else if(apOther.category1 == 0x11) {
			if(apThis->bitfield2 & 0x20000 == 0) {
				return 0;
			} else {
				dEn_c::collidedWithCat11_PipeCannon(apThis, apOther);
				return 1;
			}
		} else if(apOther.category1 == 0x8) {
			if(apThis->bitfield2 & 0x100 == 0) {
				return 0;
			} else {
				dEn_c::collidedWithCat8_FencePunch(apThis, apOther);
				return 1;
			}
		} else {
			return 0;
		}
	} else {
		return 0;
	}
}

