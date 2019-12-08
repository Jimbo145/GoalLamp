#include "Draw.h"


void ClearLamp(NeoPixelStrip* ledStrip){
        ledStrip->ClearTo(RgbColor(0, 0, 0));
        ledStrip->Show();
}

bool lightSpinningLamp(NeoPixelStrip* ledStrip, uint16_t frameNum, NhlGame* game, AnimationExtra extra){
    //Recommended Frame Time of 100ms
    if(frameNum <= extra.uInteger * NUMBER_OF_STRANDS){
        //for(int col=0; col<NUMBER_OF_STRANDS; col++){
            int rotationNumber = frameNum / NUMBER_OF_STRANDS;

            int col = frameNum - (NUMBER_OF_STRANDS * rotationNumber);
            //ledStrip->ClearTo(RgbColor(0, 0, 0));
            for(int row=0; row<=PIXEL_PER; row++){
                //Serial.println(String(row) + " " + String(col) + " " + String(rotationNumber));
                /*if(col-7>=0){
                    ledStrip->SetPixelColor(matrix[col-7][row], RgbColor(255, 0, 0));
                }
                if(col-6>=0){
                    ledStrip->SetPixelColor(matrix[col-6][row], RgbColor(255, 0, 0));
                }
                if(col-5>=0){
                    ledStrip->SetPixelColor(matrix[col-5][row], RgbColor(255, 0, 0));
                }
                if(col-4>=0){
                    ledStrip->SetPixelColor(matrix[col-4][row], RgbColor(255, 0, 0));
                }
                if(col-3>=0){
                    ledStrip->SetPixelColor(matrix[col-3][row], RgbColor(255, 0, 0));
                }
                if(col-2>=0){
                    ledStrip->SetPixelColor(matrix[col-2][row], RgbColor(255, 0, 0));
                }
                if(col-1>=0){
                    ledStrip->SetPixelColor(matrix[col-1][row], RgbColor(255, 0, 0));
                }
                */
                
                if(col-1 > 0){
                    ledStrip->SetPixelColor(matrix[col-1][row], RgbColor(255, 0, 0));
                }
                if(col+1< NUMBER_OF_STRANDS){
                    ledStrip->SetPixelColor(matrix[col+1][row], RgbColor(255, 0, 0));
                }
                ledStrip->SetPixelColor(matrix[col][row], RgbColor(255, 0, 0));
                ledStrip->SetPixelColor(TOP_PIXEL, RgbColor(255, 0, 0));
            }
            ledStrip->Show();
            //delay(100);
        //}
        return false;
    }
    return true;
}

bool fillWithColor(NeoPixelStrip* ledStrip, uint16_t frameNum, NhlGame* game, AnimationExtra extra){
    Serial.println("fill With Color " + String(extra.uInteger));
    uint16_t framesPerAnimation = extra.uInteger;
    if(frameNum < framesPerAnimation){
        for(int i=0; i<PIXEL_COUNT; i++){
            ledStrip->SetPixelColor(i, extra.color);
        }
        if(frameNum == framesPerAnimation-1){
            return true;
        }
    }
    return false;
}

int lightLine(int vert, int x, int y, int len, RgbColor color, NeoPixelStrip* ledStrip){
    for(int i=0; i<len; i++){
        if(vert){
            ledStrip->SetPixelColor(matrix[x][y+i], color);
        }else{
            ledStrip->SetPixelColor(matrix[x+i][y], color);
        }
     }
    ledStrip->Show();
    return 0;
}

void showNumber(int num, int x, int wrap, RgbColor color, NeoPixelStrip* ledStrip){
    int matrixCol = x;
    for(int col = 0; col<4; col++){
        matrixCol++;
        for(int row = 0; row<7; row++){
            if(matrixCol>=NUMBER_OF_STRANDS){
                if(wrap){
                    matrixCol = 0;
                }else{
                    break;
                }
            } else{
                if(number[num][row][col]){
                    ledStrip->SetPixelColor(matrix[matrixCol][6-row], color);
                }
            }
        }
    }
    ledStrip->Show();
}

void showLetter(char character, int x, int wrap, RgbColor color, NeoPixelStrip* ledStrip){
    int num = character - 65;
    int matrixCol = x;
    for(int col = 0; col<4; col++){
        matrixCol++;
        for(int row = 0; row<7; row++){
            if(matrixCol>=NUMBER_OF_STRANDS){
                if(wrap){
                    matrixCol = 0;
                }else{
                    break;
                }
            } else{
                if(letter[num][row][col]){
                    ledStrip->SetPixelColor(matrix[matrixCol][6-row], color);
                }
            }
        }
    }
    ledStrip->Show();
}

void print(String strIn, RgbColor color, NeoPixelStrip* ledStrip,Team team, void (*backgroundFunc)(NeoPixelStrip*, RgbColor, uint16_t, bool)){
    String str = strIn;
    str.toUpperCase();
    int index = 7;
    for(int j=0; j<(6*str.length()); j++){
        (*backgroundFunc)(ledStrip, RgbColor(0,0,255), 0, false);
        for(int letters=0; letters<str.length(); letters++){
            if(index+(4*letters)>=0){
                if(str[letters]>64 && str[letters]<91){
                    showLetter(str[letters], index+(4*letters),0,color, ledStrip);
                } else if(str[letters]>47 && str[letters]<58){
                    showNumber(str[letters]-48, index+(4*letters),0,color, ledStrip);
                } else if(str[letters] == 32){
                    showNumber(14, index+(4*letters),0,color, ledStrip);
                } else if(str[letters] == '+'){
                    showNumber(13, index+(4*letters),0,color, ledStrip);
                } else if(str[letters] == '-'){
                    showNumber(12, index+(4*letters),0,color, ledStrip);
                }
            }
        }
        index--;
        delay(300);
        ClearLamp(ledStrip);
    }
}

void showScoreNumbers(NhlGame game, NeoPixelStrip* ledStrip){
    for(int j=(NUMBER_OF_STRANDS+4); j>=-5; j--){
        ClearLamp(ledStrip);
        if(j<NUMBER_OF_STRANDS){
            showNumber(game.home.score, j, 0, RgbColor(NHLCOLORS[game.home.id][0][0],NHLCOLORS[game.home.id][0][1],NHLCOLORS[game.home.id][0][2]), ledStrip);
        }
        if(j>=3 && j<NUMBER_OF_STRANDS+3){
            showNumber(10, j-4, 0, RgbColor(255,0,0), ledStrip);
        }
        if(j>=5 && j<NUMBER_OF_STRANDS*3){
            showNumber(game.away.score, j-8, 0, RgbColor(NHLCOLORS[game.away.id][0][0],NHLCOLORS[game.away.id][0][1],NHLCOLORS[game.away.id][0][2]), ledStrip);
        }
        delay(800);
    }
}

void showPixelScore(NhlGame game, NeoPixelStrip* ledStrip){
    for(int i=0; i<NUMBER_OF_STRANDS; i++){
        if(i%2==0){
            for(int j=0; j<game.home.score; j++){
                ledStrip->SetPixelColor(matrix[i][j], RgbColor(NHLCOLORS[game.home.id][0][0],NHLCOLORS[game.home.id][0][1],NHLCOLORS[game.home.id][0][2]));
            }
        }else {
            for(int j=0; j<game.away.score; j++){
                ledStrip->SetPixelColor(matrix[i][j], RgbColor(NHLCOLORS[game.away.id][0][0],NHLCOLORS[game.away.id][0][1],NHLCOLORS[game.away.id][0][2]));
             }
        }
        ledStrip->Show();
    }
}

void animateTeamColor(Team team, uint8_t animations, NeoPixelStrip* ledStrip){
    bool frame = false;
    ClearLamp(ledStrip);
    for(int j=0; j<animations; j++){
        frame = !frame;
        for(int i=0; i<PIXEL_PER; i++){
            if(i%2){
                lightLine(false, 0, i, 8, RgbColor(NHLCOLORS[team.id][frame][0],NHLCOLORS[team.id][frame][1],NHLCOLORS[team.id][frame][2]), ledStrip);
            }else{
                lightLine(false, 0, i, 8, RgbColor(NHLCOLORS[team.id][!frame][0],NHLCOLORS[team.id][!frame][1],NHLCOLORS[team.id][!frame][2]), ledStrip);
            }
        }
        
        delay(800);
        ClearLamp(ledStrip);

    }
}

void sideBySideTeamColorBlocking(Team team, NeoPixelStrip* ledStrip){
    sideBySideTeamColor(team, ledStrip);
    delay(4000);
}

void sideBySideTeamColor(Team team, NeoPixelStrip* ledStrip){
    for(int i=0; i<NUMBER_OF_STRANDS/2; i++){
        lightLine(true, i, 0, 8, RgbColor(NHLCOLORS[team.id][0][0],NHLCOLORS[team.id][0][1],NHLCOLORS[team.id][0][2]), ledStrip);
    }
    for(int i=NUMBER_OF_STRANDS/2; i<NUMBER_OF_STRANDS; i++){
        lightLine(true, i, 0, 8, RgbColor(NHLCOLORS[team.id][1][0],NHLCOLORS[team.id][1][1],NHLCOLORS[team.id][1][2]), ledStrip);
    }
}

void flashColor(RgbColor color, uint8_t numberOfFlashes){

}

bool testAniFunc(NeoPixelStrip* s, uint16_t frameNum, NhlGame* g, AnimationExtra extra){
    Serial.println("im a function " + String(extra.uInteger));
    return true;
}

void gameIntroAnimate(Team team, NeoPixelStrip* ledStrip){
    
    /*lightLamp(ledStrip, 1000);
    delay();
    lightLamp(strip, 1000);
    delay
    sideBySideTeamColor(team, ledStrip);*/

}

//hex format: #FFFFFF
void hexColorToRGB(String hex, int* rgbReturnArray){
    String str0 =  hex.substring(1,2);
    String str1 = hex.substring(3,4);
    String str2 = hex.substring(5,6);
    
    
}
