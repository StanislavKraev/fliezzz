#ifndef COMMANDTYPE
#define COMMANDTYPE

enum CommandType
{
    CtStartGame = 1,
    CtStopGame = 2,
    CtChangeConfig = 3,

    CtAddCreature = 10,

    CtGetGameState = 20,
    CtGetGameDate = 21,

    CtLast
};

#endif // COMMANDTYPE

