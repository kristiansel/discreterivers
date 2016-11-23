#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

class GameObject
{
public:
    template<class ... Types>
    GameObject create(Types ...args);



private:
    int id; // just an ID
};



#endif // GAMEOBJECT_H
