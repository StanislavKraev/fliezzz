#ifndef COMMANDTYPE
#define COMMANDTYPE

enum CommandType
{
    CtStartGame = 1,
    CtStopGame = 2,
    CtChangeConfig = 3,

    CtAddCreature = 10,

    CtGetGameState = 20,

    CtGameState = 30,
    CtGameData = 31,

    CtLast
};

#endif // COMMANDTYPE

