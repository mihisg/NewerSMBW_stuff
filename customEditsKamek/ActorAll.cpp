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

virtual bool dEn_c::preSpriteCollision(ActivePhysics* apThis, ActivePhysics* apOther) {
	if(apOther->info.category2 == 9) {
		return dEn_c::collidedWithCat9_RollingObject(apThis, apOther);
	} else {
		return 0;
	}
}

virtual bool dEn_c::prePlayerCollision(ActivePhysics* apThis, ActivePhysics* apOther) {
	if(apOther->owner->stageActorType == 1) {		//category1 == 1 -> player who performs an attack
		if(apOther->info.category2 == 3) {
			return dEn_c::collidedWithCat3_StarPower(apThis, apOther);
		} else if(apOther->info.category2 == 7) {
			if(daPlBase_c::canGroundPound(apOther->owner) == 0) {
				return dEn_c::collidedWithCat7_GroundPound(apThis, apOther);
			} else {
				return 0;
			}
		} else if(apOther->info.category2 == 5) {
			if(apThis->info.bitfield2 & 0x20 == 0) {
				return 0;
			} else {
				return dEn_c::collidedWithCat5_MarioWut(apThis, apOther);
			}
		} else if(apOther->info.category2 == 0xa) {
			if(apThis->bitfield2 & 0x400 == 0) {
				return 0;
			} else {
				dEn_c::collidedWithCatA_PenguinMario(apThis, apOther);
				return 1;
			}
		} else if(apOther->category2 == 0xd) {
			if(apThis->bitfield2 & 0x2000 == 0) {
				return 0;
			} else {
				dEn_c::collidedWithCatD_Drill(apThis, apOther);
				return 1;
			}
		} else if(apOther.category2 == 0x11) {
			if(apThis->bitfield2 & 0x20000 == 0) {
				return 0;
			} else {
				dEn_c::collidedWithCat11_PipeCannon(apThis, apOther);
				return 1;
			}
		} else if(apOther.category2 == 0x8) {
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

virtual bool dEn_c::preYoshiCollision(ActivePhysics* apThis, ActivePhysics* apOther) {
	if(apOther->owner->stageActorType == 2) {		//yoshi
		if(apOther->info.category2 == 3) {
			return dEn_c::collidedWithCat3_StarPower(apThis, apOther);
		} else if(apOther->info.category2 == 7) {
			return dEn_c::collidedWithCat7_GroundPoundYoshi(apThis, apOther);
		} else if(apOther->info.category2 == 5) {
			if(apThis->info.bitfield2 & 0x20 != 0) {
				return collidedWithCat5_MarioWut(apThis, apOther);
			} else { return 0; }
		}
	} else {
		return 0;
	}
}

virtual bool dEn_c::stageActorCollision(ActivePhysics* apThis, ActivePhysics* apOther) {
	if(apOther->info.category2 == 9) {
		return collidedWithCat9_RollingObject(apThis, apOther);
	} else if(apOther->info.category2 == 1 && (apThis->info.bitfield2 & 2) != 0) {
		return dEn_c::collidedWithCat1_Fireball_or_E_Explosion(apThis, apOther);
	} else if(apOther->info.category2 == 0x14 && (apThis->info.bitfield2 & 0x100000) != 0) {
		return dEn_c::collidedWithCat14_YoshiFire(apThis, apOther);
	} else if(apOther->info.category2 == 0xE) {
		return dEn_c::collidedWithCat1_Fireball_or_E_Explosion(apThis, apOther);
	} else {
		u8 bVar1 = apOther->info.category2;
		if(((bVar1 == 2) && (apThis->info.bitfield2 & 4) != 0) || 
			(bVar1 == 0x15 && (apThis->info.bitfield2 & 0x200000) != 0)) {
				return dEn_c::collidedWithCat2_Iceball_Or_15_YoshiIce(apThis, apOther);
			}
		else if(apOther->info.category2 == 0x13 && (apThis->info.bitfield2 & 0x80000) != 0) {
			return dEn_c::collidedWithCat13_Hammer(apThis, apOther);
		} else {
			return 0;
		}
	}
}

virtual void dEn_c::spriteCollision(ActivePhysics* apThis, ActivePhysics* apOther) {
	return;
}

virtual void dEn_c::playerCollision(ActivePhysics* apThis, ActivePhysics* apOther) {
	char ret;
	dStageActor_c* one = apThis->owner;
	dStageActor_c* two = apOther->owner;
	
	if(two->pos.z <= 8000.0 || one->appearsOnBackFence != 1) {
		if(8000.0 <= two->pos.z) {
			ret = 0;
		} else if(one->appearsOnBackFence != 0) {
			ret = 0;
		}
	} else {
		uVar = &two[4].cSensor.tileBelowSubType;
		if((uVar & 0x80000 == 0) && (uVar & 0x100000 == 0)) {
			ret = 0;
		} else {
			ret = 1;
		}
	}
	
	if(ret == 0) {
		dEn_c::hurtPlayer();
	}
}

virtual void dEn_c::yoshiCollision(ActivePhysics* apThis, ActivePhysics* apOther) {
	char ret;
	dStageActor_c* one = apThis->owner;
	dStageActor_c* two = apOther->owner;
	
	if(two->pos.z <= 8000.0 || one->appearsOnBackFence != 1) {
		if(8000.0 <= two->pos.z) {
			ret = 0;
		} else if(one->appearsOnBackFence != 0) {
			ret = 0;
		}
	} else {
		uVar = &two[4].cSensor.tileBelowSubType;
		if((uVar & 0x80000 == 0) && (uVar & 0x100000 == 0)) {
			ret = 0;
		} else {
			ret = 1;
		}
	}
	
	if(ret == 0) {
		dEn_c::hurtPlayer();
	}
}

static void dEn_c::collisionCallback(ActivePhysics* one, ActivePhysics* two) {
	dStageActor_c* var7 = one->owner;
	dStageActor_c* var6 = two->owner;
	
	if(var7[1].field_0xa0 == 0) {
		//stageActorType: 0=Normal 1=Player 2=Yoshi 3=Entity
		if(var6->stageActorType == 3) {
			bool s = dEn_c::preSpriteCollision(one, two);
			if(s == 0) {
				dEn_c::spriteCollision(one, two);
			} else {
				one->some_flag_byte |= 2;
			}
		} else if(var6->stageActorType == 1) {
			bool s = dEn_c::prePlayerCollision(one, two);
			if(s == 0) {
				if(two->info.category1 != 1) {
					byte* pbVar4 = dStageActor_c::returnPtrToPlayerID();
					if(*pbVar4 < 4 && /*some other weird thing */) {
						dEn_c::playerCollision(one, two);
					}
				}
			} else {
				var7[1].field_0xa2 = &one->field_0x18;
				one->some_flag_byte |= 2;
			}
		} else if(var6->stageActorType == 2) {
			if(dStageActor_c::isOutOfView(var6) < 4) {
				if(two->info.category2 == 0xF) {
					dEn_c::eatenByYoshi();
				} else {
					int unk = ClassWithCameraInfo::instance->screenTop <
                       (float)(posY + (double)(aPhysics->info).y_dist_to_center) -
                       (aPhysics->info).y_dist_to_edge) << 2) << 0x1c) >> 0x1e;
					if(unk == 0) {
						bool s = dEn_c::preYoshiCollision(one, two);
						if(s == 0) {
							//something strange here, if-condition
							dEn_c::yoshiCollision(one, two);
						} else {
							var7[1].field_0xa2 = &one->field_0x18;
							one->some_flag_byte |= 2;
						}
					}
				}
			}
		} else {
			bool s = dEn_c::stageActorCollision(one, two);
			if(s != 0) {
				var7[1].field_0xa2 = &one->field_0x18;
				one->some_flag_byte |= 2;
			}
		}			
	} else {
		one->some_flag_byte |= 2;
	}
}











