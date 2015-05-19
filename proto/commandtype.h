#ifndef COMMANDTYPE
#define COMMANDTYPE

namespace proto
{

enum CommandType
{
    CtStartGame = 1,
    CtStopGame = 2,
    CtChangeConfig = 3,
    CtGetConfig = 4,

    CtAddCreature = 10,

    CtObsoletableCommands = 100,

    CtGetGameState = 120,

    CtGameState = 130,
    CtGameData = 131,
    CtGameConfig = 132,

    CtLast
};

}

#endif // COMMANDTYPE

