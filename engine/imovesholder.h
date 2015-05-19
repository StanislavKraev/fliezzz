#ifndef IMOVESHOLDER_H
#define IMOVESHOLDER_H


namespace engine
{

class IMovesHolder
{
public:
    IMovesHolder();
    virtual ~IMovesHolder();
public:
    virtual void addMove(double time, double velocity) = 0;
};

}

#endif // IMOVESHOLDER_H
