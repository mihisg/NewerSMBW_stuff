# Using States for making more complex sprites

When dealing with sprites, the code will grow very soon, and if  it isn't well structured you will probably get into huge problems when trying to find errors
or keeping the gist on what's going on with your sprite. For avoiding an unstructured mess like that there are several concepts of how to structure your code
when making game-objects. One of the most common ideas is to encapsulate the sprite's behaviour into different parts called "states". Every state represents a 
specific kind of behaviour, and the sprite can switch between the states to do different things ingame. This requires the sprite to be in only one state at the same
time and move via transitions to the next states (based on events like ``sprite's health gets zero -> change to Die state``). An example of a system of states would be seen
[here](https://img1.daumcdn.net/thumb/R800x0/?scode=mtistory2&fname=https%3A%2F%2Ft1.daumcdn.net%2Fcfile%2Ftistory%2F277435395262C92A1C).
