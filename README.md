# snake
##this is the popular snake game

Unlike tradition implementations,  this game is event-drivenen and the idea comes from ''libevent''.
The reason why use event is that I want to have an abstract and segregated view of event and the game.

1. The events are keyboard directions, user inputs, timers and so on
2. The game are something like Snake, Tetris and so on.

The event.h and event.c are the event abstraction like timer and user inputs and they are binding with the corresponding callback function.
For example, a repeated time is binding with a timer-fire function and the function will be fired when the timer expires.

