#ifndef CCUBESTATE_H
#define CCUBESTATE_H

#define EDGE_SZ 4
#define CUBE_SZ 64

class CCubeState
{
public:
    int state[EDGE_SZ][EDGE_SZ][EDGE_SZ];// размерность вдоль направлений
    int duration;    //длительность состояния
    char message[CUBE_SZ+1]; //сообщение, отправляемое в куб. побайтово пишем в массив из 65 элементов
    void state2msg();   //состояние в сообщение
    void msg2state();  //сообщение в состояние
   explicit CCubeState();
    ~CCubeState();
};

#endif // CCUBESTATE_H
