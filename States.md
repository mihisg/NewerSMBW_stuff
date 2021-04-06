# Using States for making more complex sprites

When dealing with sprites, the code will grow very soon, and if  it isn't well structured you will probably get into huge problems when trying to find errors
or keeping the gist on what's going on with your sprite. For avoiding an unstructured mess like that there are several concepts of how to structure your code
when making game-objects. One of the most common ideas is to encapsulate the sprite's behaviour into different parts called "states". Every state represents a 
specific kind of behaviour, and the sprite can switch between the states to do different things ingame. This requires the sprite to be in only one state at the same
time and move via transitions to the next states (based on events like ``sprite's health gets zero -> change to Die state``). An example of a system of states can be seen [here](https://img1.daumcdn.net/thumb/R800x0/?scode=mtistory2&fname=https%3A%2F%2Ft1.daumcdn.net%2Fcfile%2Ftistory%2F277435395262C92A1C).

___
## Declaring states in NSMBW

New Super Mario Bros. Wii uses the discribed state-technique very often, and it's a main concept of sprite programming there. But how do we use states in NSMBW's code?
Let's consider we have a simple goomba-sprite, which states does it maybe have? This is basically the same like asking what different behaviour a goomba has. 
First, a goomba is able to walk in a certain direction, so we can use a state called ``Walk`` for this. Also the goomba has to turn around when hitting a wall or 
another sprite, so we would have a ``Turn``-state. Finally the goomba should be able to die when the player jumps on it or throws a fireball or something similar.
So we would have three states. To basically tell the NSMBW-engine that we are using states for our sprite, we simply write
```c++
class MyGoomba : dEn_c {
      //stuff like onCreate, onExecute, collision functions, ...
      
      USING_STATES(MyGoomba);
}
```
This line will tell the game that this class uses states. In the next step we can now define what specific states our sprite should have. This is done with the
``DECLARE_STATE(MyState);`` command. So in our goomba-example we would have something like:
```c++
class MyGoomba : dEn_c {
      //stuff like onCreate, onExecute, collision functions, ...
      
      USING_STATES(MyGoomba);
      
      DECLARE_STATE(Walk);
      DECLARE_STATE(Turn);
      DECLARE_STATE(Die);
}
```
Have in mind that this commands have to be called inside the sprite-class. The last step for creating your states is done by using the ``CREATE_STATE(Class, State)``
function that will finally intialize your states. This function has to be called once for each state, but this time it is necessary to do it outside of you class. So:
```c++
class MyGoomba : dEn_c {
      //stuff like onCreate, onExecute, collision functions, ...
      
      USING_STATES(MyGoomba);
      
      DECLARE_STATE(Walk);
      DECLARE_STATE(Turn);
      DECLARE_STATE(Die);
}

//Now we are outside
CREATE_STATE(MyGoomba, Walk);
CREATE_STATE(MyGoomba, Turn);
CREATE_STATE(MyGoomba, Die);
```
This step maybe seems confusing because why do we have to do it twice, one time with ``DECLARE_STATE`` inside the class and the other time by using ``CREATE_STATE``
outside of your class? Well, this is how the code is structured in NSMBW (explanation is given at the end of the document in the ``insider-Knowledge``-section.

## Use the states and tell the game what do to in each state

After finishing the previous steps there will automatically be new methods for each of your sprite's states that you can use in code now. Every state you created before has three methods: ``beginState_NAME``, ``executeState_NAME`` and ``endState_NAME``. Also there is a variable for each state you declared, called ``StateID_NAME``. 
To implement the actual code for what the sprite should do in each state, you will have to override the 3 methods listened above. You do this the exact same way
as implementing the ``onCreate()`` or any other function: Outside your class(basically after the declaration). So what we have is:
```c++
class MyClass : dEn_c {
      //stuff like onCreate, onExecute, collision functions, ...
      
      USING_STATES(MyClass);
      
      DECLARE_STATE(Test);
}

//Now we are outside
CREATE_STATE(MyClass, Test);

//we have 3 functions now and can call them:
void MyClass::beginState_Test() {
    //this function is called once when the sprite enters this state
}

void MyClass::executeState_Test() {
    //this function should be called every frame, it's the behaviour of the sprite in that state
}

void MyClass::endState_Test() {
    //this function is called once when the sprite leaves this state and changes into a new one
}
```
Keep in mind that the ``executeState_NAME`` function will not be called automatically every frame. You have to do it yourself. But it's easy:
The ``dEn_c``-class has a variable called ``acState``. This variable holds the actual state the sprite is in. To call your ``executeState_NAME`` function every frame,
you could just add this to the ``onExecute()``-function of the sprite(which is automatically called every frame):
```c++
int MyClass::onExecute() {
    acState.execute();
    //do some other things
    return true;
}
```
No matter which state your sprite is in at the moment, it will always be updated every frame now. 
When we look at our Goomba-example, we would have something like:
```c++
class MyGoomba : dEn_c {
      //stuff like onCreate, onExecute, collision functions, ...
      
      USING_STATES(MyGoomba);
      
      DECLARE_STATE(Walk);
      DECLARE_STATE(Turn);
      DECLARE_STATE(Die);
}

//Now we are outside
CREATE_STATE(MyGoomba, Walk);
CREATE_STATE(MyGoomba, Turn);
CREATE_STATE(MyGoomba, Die);

//Walk State
void MyGoomba::beginState_Walk() {
    //called once -> maybe set the correct animation or define a speed for the movement
}
void MyGoomba::executeState_Walk() {
    pos.x += 2;     //move it a little bit
    //upadte animation
}
void MyGoomba::endState_Walk() {
    //I don't know. The end state functions are barely used. You could just set the speed to zero again
}

//Turn State
void MyGoomba::beginState_Turn() {
    //called once -> define if should turn right or left
}
void MyGoomba::executeState_Turn() {
    rot.y += 2;     //rotate it a little bit in the right direction
    //upadte animation
}
void MyGoomba::endState_Turn() {
    //nothing here
}

//Die state
void MyGoomba::beginState_Die() {
    //called once -> remove my ActivePhysics to not react to collision anymore
}
void MyGoomba::executeState_Die() {
    //play a sound
    //fall down the screen
    //upadte die-animation
}
void MyGoomba::endState_Die() {
    //Delete the object
}
```

## Transition between states
There are some conditions where we want our sprite to change its state. If our Goomba is walking and hits a wall, it should turn. So let's consider we have a function
which tells us that the goomba collides with a wall and should turn. We then could use ``doStateChange(dStateBase_c* state)`` to change the ``acState`` variable and move to another state. The ``doStateChange`` function is also included in the ``dEn_c`` class. Remember that there is a variable for every state created automatically by the ``DECLARE_STATE`` and ``CREATE_STATE``. This variable is called ``StateID_NAME`` and this is the one we have to put in as a parameter in ``doStateChange``(as a pointer!). Our goomba would call the ``endState_Walk()`` function now and sets its ``acState`` to ``StateID_Turn``. Then it calls ``beginState_Turn()`` function once, and after that now ``executeState_Turn()`` is called every frame instead of ``executeState_Walk()``.
So the final version of our goomba would be(note that I used non-existing functions like ``collisionWithWall`` for simplicity):
```c++
class MyGoomba : dEn_c {
      //stuff like onCreate, onExecute, collision functions, ...
      
      USING_STATES(MyGoomba);
      
      DECLARE_STATE(Walk);
      DECLARE_STATE(Turn);
      DECLARE_STATE(Die);
}

//Now we are outside
CREATE_STATE(MyGoomba, Walk);
CREATE_STATE(MyGoomba, Turn);
CREATE_STATE(MyGoomba, Die);

//Walk State
void MyGoomba::beginState_Walk() {
    //called once -> maybe set the correct animation or define a speed for the movement
}
void MyGoomba::executeState_Walk() {
    pos.x += 2;     //move it a little bit
    //upadte animation
    if(collisionWithWall) {
          doStateChange(&StateID_Turn);
    }
    if (marioIsJumpingOnMyHead) {
          doStateChange(&StateID_Die);
    }
}
void MyGoomba::endState_Walk() {
    //I don't know. The end state functions are barely used. You could just set the speed to zero again
}

//Turn State
void MyGoomba::beginState_Turn() {
    //called once -> define if should turn right or left
}
void MyGoomba::executeState_Turn() {
    rot.y += 2;     //rotate it a little bit in the right direction
    //upadte animation
    if (finishedTurn) {
         doStateChange(&StateID_Walk);   
    }
}
void MyGoomba::endState_Turn() {
    //nothing here
}

//Die state
void MyGoomba::beginState_Die() {
    //called once -> remove my ActivePhysics to not react to collision anymore
}
void MyGoomba::executeState_Die() {
    //play a sound
    //fall down the screen
    //upadte die-animation
}
void MyGoomba::endState_Die() {
    //Delete the object
}
```

So that is everything related to states. They allow you to seperate behaviour and keep the code clean. Try them and have fun! :)

___
# Insider knowledge about states
TODO
