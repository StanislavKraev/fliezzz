#ifndef COMMANDTYPE
#define COMMANDTYPE

namespace proto
{

enum CommandType
{
    CtStartGame = 1,
    CtStopGame = 2,
    CtChangeConfig = 3,

    CtAddCreature = 10,

    CtObsoletableCommands = 100,

    CtGetGameState = 120,

    CtGameState = 130,
    CtGameData = 131,

    CtLast
};

}

#endif // COMMANDTYPE

