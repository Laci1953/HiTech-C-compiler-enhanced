The ALMAZAR game (https://github.com/Laci1953/RC2014-CPM/tree/main/Games/almazar)

(executed on RomWBW)

C>c -v -o almazar.c attempt.c saverest.as xrnd.as

HI-TECH C COMPILER (CP/M-80) V3.09
Copyright (C) 1984-87 HI-TECH SOFTWARE
ALMAZAR.C
0:CPP -DCPM -DHI_TECH_C -Dz80 -I ALMAZAR.C $CTMP1.$$$

0:P1 $CTMP1.$$$ $CTMP2.$$$ $CTMP3.$$$
0:CGEN $CTMP2.$$$ $CTMP1.$$$
ALMAZAR.C:272:  constant relational expression
ALMAZAR.C:932:  constant relational expression
0:OPTIM $CTMP1.$$$ $CTMP2.$$$
0:ZAS -J -N -oALMAZAR.OBJ $CTMP2.$$$
Z80AS Macro-Assembler V4.8

Errors: 0

Jump optimizations done: 252
Finished.
ATTEMPT.C
0:CPP -DCPM -DHI_TECH_C -Dz80 -I ATTEMPT.C $CTMP1.$$$

0:P1 $CTMP1.$$$ $CTMP2.$$$ $CTMP3.$$$
0:CGEN $CTMP2.$$$ $CTMP1.$$$
0:OPTIM $CTMP1.$$$ $CTMP2.$$$
0:ZAS -J -N -oATTEMPT.OBJ $CTMP2.$$$
Z80AS Macro-Assembler V4.8

Errors: 0

Jump optimizations done: 366
Finished.
SAVEREST.AS
0:ZAS -J -OSAVEREST.OBJ SAVEREST.AS
Z80AS Macro-Assembler V4.8

Errors: 0
Finished.
XRND.AS
0:ZAS -J -OXRND.OBJ XRND.AS
Z80AS Macro-Assembler V4.8

Errors: 0

Finished.
ERA $CTMP1.$$$
ERA $CTMP2.$$$
ERA $CTMP3.$$$
0:LINK -Z -Ptext=0,data,bss -C100H -OALMAZAR.COM CRTCPM.OBJ ALMAZAR.OBJ ATTEMPT.OBJ SAVEREST.OBJ XRND.OBJ LIBC.LIB
ERA $$EXEC.$$$

C>almazar
                The Search For Almazar - Part I
This program is the first of a projected series of programs whose central theme
 is the continuing search for the super being Almazar.
This game, however, will play to a satisfactory ending if the player does not
 wish to continue the series.
The game begins at one end of a road, by an old abandoned shack.
(Just like the original ADVENTURE program.)
The player will then explore the many different scenarios in the game.
But be careful.
There are many traps to catch the careless and unthinking adventurer.
The program accepts one- or two-word commands, using a simple syntax:
 verb (noun)<CR> (lowercase or uppercase, first 4 letters are significant).
Some examples:
  Type ENTER to enter a shack or LEAVE to leave a shack.
  (Of course, if there is no way out you cannot leave.)
  To take an object, type TAKE OBJECT, or T OBJECT for short.
  To go north, type NORTH, or N for short.
  To go south, type SOUTH, or S for short.
  To go east, type EAST, or E for short.
  To go west, type WEST, or W for short.
  To go up, type UP, or U for short.
  To go down, type DOWN, or D for short.
  To see what you're carrying, type INVE(NTORY).
  Type SCORE and the program will give you your current score.
  Type SAVE and the program will save the game for later play.
  Type RESTORE and the program will load the saved game.
  Type QUIT and the game will be terminated.
  To light or turn off a lamp, type LIGHT LAMP or OFF LAMP.
  To get a description of the room, type LOOK.
Other commands: SMASH, TOSS, SHOW, CROSS, JUMP, DROP, WAVE, READ, FILL,
 LIGHT, BURN, KILL, ATTACK, KICK, BREAK, EAT, DRINK, FEED, TURN, UNLOCK,
 SWIM, OPEN, CLOSE, OFF, PULL, EMPTY.
The objects: DIAMOND, EYE, CHAIN, APPLE, PEARLS, SPICES, DAGGER, FIGURINE,
 LAMP, KEY, RING, ROD, STAFF, INCENSE, HAY, CHEESE, MOUSE, MATCHES, BOTTLE,
 IDOL, SNAKE, CHASM, WIZARD, RIVER, HEAD, BOOK, MESSAGE, BALL, OIL, WATER,
 VAULT, CHEST, DRAWER, WINDOW, ROPE, SIGN, GNOME
A hint: Caves are dark and often dangerous.
You have a limited number of turns in which to accomplish your task.
If you don't finish in time, you will lose...
From time to time you may need a hint, so just type HINT or HELP.
But remember, nothing is free in the world today.

Good luck, and may Almazar guide you to a safe journey.

You are outside an old shack.


? quit
Are you sure you want to quit? y

You have scored 0 points.

You obviously have no idea what's happening in this game.


C>
