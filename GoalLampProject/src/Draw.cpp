#include "Draw.h"


void ClearLamp(NeoPixelStrip* ledStrip){
        ledStrip->ClearTo(RgbColor(0, 0, 0));
        ledStrip->Show();
}

RgbColor rgbBrightness(RgbColor currentColor, uint8_t newBrightness){
    uint8_t red = float(currentColor.R) * (newBrightness / 255.0f);
    uint8_t green = float(currentColor.G) * (newBrightness / 255.0f);
    uint8_t blue = float(currentColor.B) * (newBrightness / 255.0f);

    return RgbColor(red, green, blue);
}

bool lightSpinningLamp(NeoPixelStrip* ledStrip, uint16_t frameNum, NhlGame* game, AnimationExtra extra){
    //Recommended Frame Time of 100ms
    if(frameNum <= extra.uInteger * NUMBER_OF_STRANDS){
        int rotationNumber = frameNum / NUMBER_OF_STRANDS;

        int col = frameNum - (NUMBER_OF_STRANDS * rotationNumber);
        for(int row=0; row<=PIXEL_PER; row++){
            
            if(col-1 > 0){
                ledStrip->SetPixelColor(matrix[col-1][row], RgbColor(255, 0, 0));
            }
            if(col+1< NUMBER_OF_STRANDS){
                ledStrip->SetPixelColor(matrix[col+1][row], RgbColor(255, 0, 0));
            }
            ledStrip->SetPixelColor(matrix[col][row], RgbColor(255, 0, 0));
            ledStrip->SetPixelColor(TOP_PIXEL, RgbColor(255, 0, 0));
        }
        //ledStrip->Show();
        return false;
    }
    return true;
}

/*  lightLamp
* Description: Fills lamp with color light for set number of frames
/ Required: AnimationExtra.color 
                Fill Color
* Optional: AnimationExtra.minFrameNum
*               Number of Frames to run this function
*
*/
bool fillWithColor(NeoPixelStrip* ledStrip, uint16_t frameNum, NhlGame* game, AnimationExtra extra){
    if(frameNum < extra.minFrameNum){
        for(int i=0; i<PIXEL_COUNT; i++){
            ledStrip->SetPixelColor(i, extra.color);
        }
        if(frameNum == extra.minFrameNum-1){
            return true;
        }
    }
    return false;
}

void lightLine(int vert, int x, int y, int len, RgbColor color, NeoPixelStrip* ledStrip){
    for(int i=0; i<len; i++){
        if(vert){
            ledStrip->SetPixelColor(matrix[x][y+i], color);
        }else{
            ledStrip->SetPixelColor(matrix[x+i][y], color);
        }
     }
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
    //ledStrip->Show();
}

/* print
* Description : Scrolling Print of complete string accross lamp
*/
void print(String strIn, RgbColor color, NeoPixelStrip* ledStrip, Team team){
    String str = strIn;
    str.toUpperCase();
    int index = 7; //TODO - change this magic number
    for(uint j=0; j<(6*str.length()); j++){
        for(uint letters=0; letters<str.length(); letters++){
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
    }
}

//TODO - Test this function --- Can I do this just with the print function instead?
bool showScoreNumbers(NeoPixelStrip* ledStrip, uint16_t frameNum, NhlGame* game, AnimationExtra extra){
    
        if(frameNum<NUMBER_OF_STRANDS){
            showNumber(game->home.score, frameNum, 0, RgbColor(NHLCOLORS[game->home.id][0][0],NHLCOLORS[game->home.id][0][1],NHLCOLORS[game->home.id][0][2]), ledStrip);
        }
        if(frameNum>=3 && frameNum<NUMBER_OF_STRANDS+3){
            showNumber(10, frameNum-4, 0, RgbColor(255,0,0), ledStrip);
        }
        if(frameNum>=5 && frameNum<NUMBER_OF_STRANDS*3){
            showNumber(game->away.score, frameNum-8, 0, RgbColor(NHLCOLORS[game->away.id][0][0],NHLCOLORS[game->away.id][0][1],NHLCOLORS[game->away.id][0][2]), ledStrip);
        }

        return true;
}

/*
*   Displays scores in alternating columns ie:
*   
*        * * * *   
*        * * * *
*       ********
*/
bool showPixelScore(NeoPixelStrip* ledStrip, uint16_t frameNum, NhlGame* game, AnimationExtra extra){  
    for(int i=0; i<NUMBER_OF_STRANDS; i++){
        if(i%2==0){
            for(int j=0; j<game->home.score; j++){
                ledStrip->SetPixelColor(matrix[i][j], RgbColor(NHLCOLORS[game->home.id][0][0],NHLCOLORS[game->home.id][0][1],NHLCOLORS[game->home.id][0][2]));
            }
        }else {
            for(int j=0; j<game->away.score; j++){
                ledStrip->SetPixelColor(matrix[i][j], RgbColor(NHLCOLORS[game->away.id][0][0],NHLCOLORS[game->away.id][0][1],NHLCOLORS[game->away.id][0][2]));
             }
        }
        //ledStrip->Show();
    }
    return true;
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

/*  sideBySideTeamColor
* Description: Fills lamp halves with team primary and secondary colors
* Requires: AnimationExtra.uInteger
*               Home = 0
*               Away = 1
* Optional: AnimationExtra.minFrameNum
*               Number of Frames to run this function
*
*/
bool sideBySideTeamColor(NeoPixelStrip* ledStrip, uint16_t frameNum, NhlGame* game, AnimationExtra extra){
    //logger.log("sideBySideTeamColor " + String(frameNum));
    uint8_t id;

    if(extra.uInteger){
        id = game->away.id;
    }else{
        id = game->home.id;
    }

    for(int i=0; i<NUMBER_OF_STRANDS/2; i++){
        lightLine(true, i, 0, 8, RgbColor(NHLCOLORS[id][0][0],NHLCOLORS[id][0][1],NHLCOLORS[id][0][2]), ledStrip);
    }
    for(int i=NUMBER_OF_STRANDS/2; i<NUMBER_OF_STRANDS; i++){
        lightLine(true, i, 0, 8, RgbColor(NHLCOLORS[id][1][0],NHLCOLORS[id][1][1],NHLCOLORS[id][1][2]), ledStrip);
    }

    if(frameNum >= extra.minFrameNum){
        return true;
    }else{
        return false;
    }
}

void flashColor(RgbColor color, uint8_t numberOfFlashes){

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
