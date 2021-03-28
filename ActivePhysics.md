# Collision and Callbacks - The ActivePhysics

The ActivePhysics-class is necessary for collision detection between two sprites. There is another class which deals with collision, called ``Physics``, but this class is used
for collision with solid objects like a wall or the ground, where Mario can't move through. But the ``ActivePhysics`` are for collision between two actors and can have custom callbacks, for example a clock should not be solid, but it should disappear and increase the timer when collided with the player. And this is where the ActivePhysics comes into play. It defines a hitbox for your sprite and reacts to collision with other sprites/powerups/...
___
## Adding and removing ActivePhysics - The ``aPhysics`` object 

So to start it is necessary to say that every sprite which inherits from ``dEn_c`` (Standard sprite class, most of custom sprites should inherit from this) already has an ActivePhysics-object.
This object is called ``aPhysics`` and can be used everywhere inside your sprite-class. But this ActivePhysics won't do anything if it is not initialized manually inside the sprite class. 
Usually the intialization part is done in ``onCreate`` when the sprite is loaded and created. But how can we actually initialize it?
The command to add an ActivePhysics-object to the list that is checked for collision every frame, is:
```c++
this->aPhysics.addToList();
```
After calling this the aPhysics will be checked for collision every frame and apply the apropriate actions(when calling ``this->aPhysics.removeFromList();`` then it will be removed from the list and no longer be updated.)

## Setting up hitbox and collision

But it would not work yet because we haven't specified how big our hitbox is, or what should be executed when collided with a player, or a sprite, ....
So basically we have to initialize our aPhysics var with all that information in order to work correctly. This is done by a class(actually a struct) inside the ActivePhysics, called ``ActivePhysics::Info``. It looks like this:

```c++
typedef void (*Callback)(ActivePhysics *self, ActivePhysics *other);		//This declares "Callback" as a pointer to a function which takes two ActivePhysics-pointers as parameters

struct Info {
	float xDistToCenter;			//x distance to the center of the aPhysics-hitbox, relative to sprite's pos.x
	float yDistToCenter;			//y distance to the center of the aPhysics-hitbox, relative to sprite's pos.y
	float xDistToEdge;			//half-width of the hitbox, from the center to the edge
	float yDistToEdge;			//half-height of the hitbox, from the center to the edge
	u8 category1;				//The categories define what kind of collider the sprite has (act like a player, like an enemy, like an object(e.g. fireball)
	u8 category2;				//same than category1, but this one is just used when category1 is equal to 1, which means this sprite acts like a player
	u32 bitfield1;				//The bitfields define what we can collide with, so should it react to fireball-collision, can be eaten by yoshi(collision with tongue)
	u32 bitfield2;				//same than bitfield1, but this one is just used when category1 is equal to 1, which means this sprite acts like a player
	u16 unkShort1C;				//what the hell is this? 
	Callback callback;			//what function should be executed when a collision occurs? This is a ``Callback`` (see above -> function pointer)
};
```

So, now for a detailed explanation:
The first four vars are kind of easy, whereas the other ones can get confusing.
- xDistToCenter/yDistToCenter define the offset from the sprite's position to the center of the hitbox. For example a goomba has its pos at the bottom-center. But our hitbox should have its center in the middle of the goomba. Because the goomba is 16x16 and its center is at the bottom-center, we have to move it up by 8 (half height) in order to be exactly at the center. So our ``xDistToCenter`` would be 0 since we don't have to move the hitbox to the left or right, but we have to move it upwards by 8, so ``yDistToCenter = 8``
- Now we have our hitbox where we want it to be. xDistToEdge/yDistToEdge now define how big our hitbox is. Because the goomba is 16x16, from center to edge we have 8(xDist) and 8(yDist). So the values would be ``xDistToEdge = 8; yDistToEdge = 8``

### Categories
After we created our aPhysics-hitbox properly, we have to define what should be executed on collision and what this sprite can collide with. 
As mentioned earlier, the categories determine what sort of collider we have. So, for example:
```
Player/Yoshi A -> category1 = 0, category2 varies
Player/Yoshi B -> category1 = 1, category2 varies
Most general entities/sprites/enemies -> category1 = 3, category2 = 0
Balloons -> category1 = 4, category2 = 0
Collectibles/Coins -> category1 = 5, category2 = 0
Player fireballs -> category1 = 6, category2 = 1
Player iceballs -> category1 = 6, category2 = 2
```
where Player/Yoshi A refer to a normal Player/Yoshi hitbox (when walking or standing), and Player/Yoshi B refer to a player/yoshi which is attacking. In this case, the ``category2`` determines what attack is currently performed. For example: Sliding penguin mario has category2 = 0xA, ground-pounding has category2 = 7, mario with a star-powerup has category2 = 3, ...

### Bitfields
The bitfields determine what this sprite can collide with. When a collision between two actors occur, the categories and bitfields are combined to check if there is
a callback for this collision which should be called. ``bitfield1`` is a 8-bit integer, and every bit specifies a collision-type. For a collision between actors A and
B to occur, then ``A.bitfield1 & (1 << B.category1)`` must be non-zero, and ``B.bitfield1 & (1 << A.category1)`` must also be non zero. This means, at the beginning you have your 8-bit value ``0000 0000``. If your sprite should be able to collide with normal Mario, which has ``category1 = 0``, then the right most bit of your ``bitfield1`` has to be set to 1. If it should be able to collide with attacking-Mario(``category1 = 1``), then the second bit has to be set to 1. So we have
``bitfield1 = 0000 0011``. If your sprite can collide with other sprites(``category1 = 3``), the fourth bit has to be set to 1. Same for the other bits(fifth bit if it should collide with a balloon, sixth bit if shold collide with collectibles, seventh if should collide with fireballs/iceballs. Btw: No idea what the third and eight bit are doing, just set the third to 1 and the eight to 0). So a normal sprite would have a value like ``01001111`` which is ``bitfield1 = 0x4F`` in hex. 

``bitfield2`` and ``category2`` have the same connection, but are only used when ``category1 == 1``, which is a collision with Player/Yoshi while he's attacking. Then ``bitfield2`` and ``category2`` determine what kind of attack is currently performed and if the sprite should react to it. the second bitfield is a 32-bit integer, and it is calculated by setting each bit to 0 or 1 based on if the sprite can collide with the apropriate attack. So the order of bits is from right to left:
```
0 = ???
1 = Fire
2 = Ice
3 = Star
4 = ???
5 = Slip (slide)
6 = ???
7 = HipAttk/HipAttack (ground pound)
8 = WireNet/NetPunch (hitting fence)
9 = Shell
10 = PenguinSlide/PenguinSlip (penguin slide)
11 = Spin
12 = ???
13 = SpinFall
14 = Fire(No FireBall, but like an explosion)
15 = YoshiEat
16 = YoshiMouth
17 = Cannon
18 = SpinLiftUp
19 = YoshiBullet
20 = YoshiFire
21 = Ice(shot by Yoshi)
```
Every bit above 21 is probably unused, they can be set to 1. A typical value for bitfield2 would be: ``11111111101110101111111111111110`` -> ``bitfield2 = 0xFFBAFFFE``

### unkShort1C
This is a very strange value, I don't know much about it, but when ``unkShort1C & 4 != 0`` then the collision callback will just be executed for this sprite and not for the other object which collides with this one. But typically you don't want that, so just set ``unkShort1C = 0`` in most cases

### CollisionCallback
If a collision occurs, then this function will be called. It usually takes two ``ActivePhysics``-pointers (the two objects that collide at the moment) and performs the action specified in this function. This function should usually be set to ``dEn_c::collisionCallback(ActivePhysics *one, ActivePhysics *two)`` since this function performs the category-bitfield check explained above and also calls the right function depending on what type of collision it is. But you can also set the ``callback`` to a custom function if needed.

### Putting everything together
So to create an actual ActivePhysics-object for your sprite, you should first create a ``ActivePhysics::Info`` object in the ``onCreate`` method of your sprite. After 
that you can initialize your ``aPhysics`` variable with this ``info`` you created by calling ``this->aPhysics.void initWithStruct(dActor_c *owner, const Info *info)``
At last you have to add it to the list so it is updated every frame(``aPhysics.addToList()``)
```c++
ActivePhysics::Info PhysicsInfo;
PhysicsInfo.xDistToCenter = 0.0;
PhysicsInfo.yDistToCenter = 8.0;
PhysicsInfo.xDistToEdge = 8.0;
PhysicsInfo.yDistToEdge = 8.0;
PhysicsInfo.category1 = 0x4;
PhysicsInfo.category2 = 0x0;
PhysicsInfo.bitfield1 = 0x4F;
PhysicsInfo.bitfield2 = 0xFFBAFFFE;
PhysicsInfo.unkShort1C = 0;
PhysicsInfo.callback = &dEn_c::collisionCallback;

this->aPhysics.initWithStruct(this, &PhysicsInfo);
this->aPhysics.addToList();
```

## Collision Callbacks
Depending on what type of collision occured, the ``dEn_c::collisionCallback`` will automatically call the correct function to solve that collision. Those functions
are contained in the ``dEn_c`` class and are all virtual(they contain nothing except ``return true``). To specify what should happen with your sprite when a collision occurs, you will have to override these methods in your sprite. There are three main functions:
```c++	
virtual void spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther);	//called when collided with another sprite
virtual void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);	//called when collided with a player
virtual void yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther);	//called when collided with Yoshi
```
But there are a lot more functions you can override. They are used when your sprite collides with the player performing a certain action or attack. All of them are
automatically called by the ``dEn_c::collisionCallback``, too:
```c++
virtual bool collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther);			//Mario has a star on collision
virtual bool collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther);			//Mario is sliding on collision
virtual bool collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther);			//Mario drills down with Propeller
virtual bool collisionCat8_FencePunch(ActivePhysics *apThis, ActivePhysics *apOther);			//punched by a fence?
virtual bool collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther);			//Mario performs a groundpound
virtual bool collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther);		//Mario performs a groundpound with yoshi
virtual bool collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther);			//Mario slides with penguin
virtual bool collisionCat11_PipeCannon(ActivePhysics *apThis, ActivePhysics *apOther);			//mario collides while shot out by a pipe cannon
virtual bool collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther);		//collision with a rolling object thrown by Mario
virtual bool collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther);		//collision with a fireball or a explosion
virtual bool collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther);		//collision with iceball shot by player or yoshi
virtual bool collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther);			//collision with hammer or bullet spit by yoshi
virtual bool collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther);			//collision with fire spit by yoshi
```
So, that's it. Everything I know about the ActivePhysics. There are some aspects to note when using custom ``ActivePhysics``-objects and not the ``aPhysics`` standard var (for example if you want your sprite to have more than one hitbox -> pokey). But that will be explained soon...
