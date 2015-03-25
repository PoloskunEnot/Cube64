#include "ccubestate.h"

CCubeState::CCubeState()
{
    for (int i = 0; i < EDGE_SZ; i++){
        for (int j = 0; j < EDGE_SZ; j++){
            for (int k = 0; k < EDGE_SZ; k++){
                state[i][j][k] = 0;
            }
        }
    }
    state2msg();
    duration = 200;
}

CCubeState::~CCubeState()
{

}


void CCubeState::state2msg()
{
    int cnt = 0;
    for (int i = 0; i < EDGE_SZ; i++){
        for (int j = 0; j < EDGE_SZ; j++){
            for (int k = 0; k < EDGE_SZ; k++){
                message[cnt] = '0'+ state[i][j][k];
                cnt++;
            }
        }
    }
    message[cnt] = 'e';
}

void CCubeState::msg2state()
{
    int cnt = 0;
    for (int i = 0; i < EDGE_SZ; i++){
        for (int j = 0; j < EDGE_SZ; j++){
            for (int k = 0; k < EDGE_SZ; k++){
                state[i][j][k] = message[cnt] - '0';
                cnt++;
            }
        }
    }
}

