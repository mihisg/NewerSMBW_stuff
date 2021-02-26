# Newer Super Mario Bros Wii Modding

custom edit of the Newer SMBW game mod, containing new enemies, levels, bosses and maybe worldmaps.
If we manage to figure out how the code injection with assembly files work, we will probably add a new powerup (maybe the shell of NSMB for the DS)
___
# Adding a new sprite into the game

- First create a .cpp file that goes in /src/ -> This file will contain all the code for the custom sprite. A basic structure would be:
```c++
#include <common.h>       //some common functions and useful stuff
#include <game.h>         //main game functions, most important file. Almost everything you use from NSMBW is located here
#include <g3dhax.h>       //Used for displaying 3d-models and updating graphics
#include <sfx.h>          //sound effects which can be played by this sprite (e.g. die-sound when Mario jumps on it)

//now the class itself -> dEn_c is the base class for sprites and NPCs
class daTestSprite: public dEn_c {
    //main functions:
    int onCreate();       //called when a new object of this sprite is created
    int onDelete();       //called when an object of this sprite is deleted
    int onExecute();      //loop. Called every frame after the sprite was created with onCreate()
    int onDraw();         //draws the sprite. Called every frame
    
    //this is necessary for allocating memory on the heap(RAM) for loading the actual 3D-Model
    mHeapAllocator_c allocator;
    
    //resources file which contains the 3d model and its animations. This is usually a .brres-file
    nw4r::g3d::ResFile resFile;
    //actual 3D-bodyModel contained in the .brres-file. This is a .mdl0-file
    m3d::mdl_c bodyModel;
    //actual animation, if the model has one. Contained in the .brres file and stored in a .chr0-file
    m3d::anmChr_c chrAnimation;
    
    //now here are basic variables and values. This depends on the actual type of sprite. Basic ones are
    int timer;
    float dying;
    char damage;
    char isDown;
    Vec initialPos;
    
    //now the actual hitbox used for the physics and collision
    ActivePhysics Physics;
    
    //now we add some functions
    //first, this one is static and creates a new object of this class
    static daTestSprite* build();
    
    //set a new animation
    void bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate);
    
    //update the Drawmatrix; used for drawing the 3d-model in the right "perspective"
    void updateModelMatrices();
    
    //check for collision with tiles like a wall -> sprite should maybe turn
    bool calculateTileCollisions();
    
    //now all the collision functions. Actually there are a lot, but the most common used ones are:
    void playerCollision(ActivePhysics* apThis, ActivePhysics apOther);
    void spriteCollision(ActivePhysics* apThis, ActivePhysics apOther);
    void yoshiCollision(ActivePhysics* apThis, ActivePhysics apOther);
    
    //collision with Mario when he has a Star
    bool collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther);	
    //collision with sliding-Mario
    bool collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther);
    //Player drills with Propeller
    bool collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther);
    //Player punches fence behind
    bool collisionCat8_FencePunch(ActivePhysics *apThis, ActivePhysics *apOther);	
    //When Player ground pounds
    bool collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther);
     //Player groundpounds with yoshi
    bool collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther);	
    //When Player slides with penguin suit
    bool collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther);	
    //When player is shot from pipe cannon
    bool collisionCat11_PipeCannon(ActivePhysics *apThis, ActivePhysics *apOther);
    //when object is thrown on Sprite (koopa-shell, barrel)
    bool collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther);		
    //collision with fireball
    bool collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther);
    //collides with iceball
    bool collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther);		        
    //collides with hammer from the hammer-suit-powerup
    bool collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther);	
    //when collides with yosgi fire spit
    bool collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther);	                  
    
    //At last there are the states the sprite can be in. A sprite can be in only one state at a specific time
    //The states are coded seperately
    //if your sprite does not need multiply states (for example a star coin has just one state, 
    //which is spinning at one certain point) 
    //yout don't need the following section
    
    //first make clear that this class can use states
    USING_STATES(daTestEnemy);
    
    //now declare all states you need for the sprite
    DECLARE_STATE(Walk);
    DECLARE_STATE(Turn);
    DECLARE_STATE(Jump);
    DECLARE_STATE(Die);
    
    //So thats basically everything, but now the methods have to be implemented
}
```
