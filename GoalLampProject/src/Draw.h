#ifndef DRAW_H
#define DRAW_H

#include <NeoPixelBus.h>
#include <Arduino.h>
#include "OnlineLogger.h"
#include "Constants.h"
#include "Types.h"

struct AnimationExtra{
    RgbColor color;
    uint16_t uInteger;
    uint16_t minFrameNum;
};

const int matrix[8][7] = {
    {56,55,54,53,52,51,50},
    {43,44,45,46,47,48,49},
    {42,41,40,39,38,37,36},
    {29,30,31,32,33,34,35},
    {28,27,26,25,24,23,22},
    {15,16,17,18,19,20,21},
    {14,13,12,11,10,9,8},
    {0,1,2,3,4,5,6}  
};

void animateTeamColor(Team team,  uint8_t animations, NeoPixelStrip* ledStrip);


//helper functions
void lightLine(int vert, int x, int y, int len, RgbColor color, NeoPixelStrip* ledStrip);
void ClearLamp(NeoPixelStrip* ledStrip);
void hexColorToRGB(String hex, int* rgbArray);

//text functions
void showNumber(int num, int x, int wrap, RgbColor color, NeoPixelStrip* ledStrip);
void showLetter(char character, int x, int wrap, RgbColor color, NeoPixelStrip* ledStrip);
void printFunc(String strIn, RgbColor color, NeoPixelStrip* ledStrip, Team team, void (*backgroundFunc)(NeoPixelStrip*, RgbColor, uint16_t,bool));


//AnimationEngine Animations
//bool ExampleAnimationFunction(NeoPixelStrip* ledStrip, uint16_t frameNum, NhlGame* game, AnimationExtra extra);
bool fillWithColor(NeoPixelStrip* ledStrip, uint16_t frameNum, NhlGame* game, AnimationExtra extra);
bool lightSpinningLamp(NeoPixelStrip* ledStrip, uint16_t frameNum, NhlGame* game, AnimationExtra extra);
bool showPixelScore(NeoPixelStrip* ledStrip, uint16_t frameNum, NhlGame* game, AnimationExtra extra);
bool sideBySideTeamColor(NeoPixelStrip* ledStrip, uint16_t frameNum, NhlGame* game, AnimationExtra extra);
bool showScoreNumbers(NeoPixelStrip* ledStrip, uint16_t frameNum, NhlGame* game, AnimationExtra extra);

class Animation {
    public:
        Animation(){

        }
        /* Animation Function, Strip, Game, AnimationExtra, background function
        *
        * 
        * 
        */
        Animation(bool (*f)(NeoPixelStrip*, uint16_t,  NhlGame*, AnimationExtra), NeoPixelStrip* s,  AnimationExtra ext, NhlGame* g = nullptr, bool (*bF)(NeoPixelStrip*, uint16_t,  NhlGame*, AnimationExtra) = nullptr){ 
            animationFunc = f;
            backgroundFunc = bF;
            ledStrip = s;
            game = g;
            extra.color = ext.color;
            extra.uInteger = ext.uInteger;
            extra.minFrameNum = ext.minFrameNum;
        } 
        
        //returns animation complete
        inline bool RunAnimationFrame(uint16_t frameNum){
            ledStrip->ClearTo(0);
            if(backgroundFunc){
                backgroundFunc(ledStrip, frameNum, game, extra);
            }
            bool retBool = animationFunc(ledStrip, frameNum, game, extra);
            ledStrip->Show();
            return retBool;
        }
    private:
        bool loopUntilNext;
        uint16_t timePerFrame; //required time per frame in seconds
        NeoPixelStrip* ledStrip;
        NhlGame* game;
        AnimationExtra extra;
        bool (*animationFunc)(NeoPixelStrip*, uint16_t,  NhlGame*, AnimationExtra);
        bool (*backgroundFunc)(NeoPixelStrip*, uint16_t,  NhlGame*, AnimationExtra);
};

     
class AnimationEngine {
    public:
        AnimationEngine(NhlGame* g, NeoPixelStrip* s, uint16_t frameRate_in_ms){
            game = g;
            ledStrip = s;
            frameRate = frameRate_in_ms;
        }

        inline void StartAnimation(){            running = true;        };
        inline void StopAnimation(){            running = false;        };
        void handleAnimation(){
            unsigned long currentTime = millis();
            if(currentTime < startTime){
                //ROLL OVER
                startTime = currentTime;
            }
            if(running && queueItemTotal > 0 && ((currentTime >= startTime + (frameRate)) || startTime == currentTime)){
                Serial.println("Start Time " + String(startTime)+ " CurrentTime " + String(currentTime) + " Target " + String(startTime + frameRate));
                startTime = currentTime;
                bool complete = animationQueue[queueHead].RunAnimationFrame(currentFrame);
                if (complete){
                    currentFrame = 0;
                    if(queueHead+1 >= MAX_QUEUE){
                        queueHead = 0;
                    }else{
                        queueHead++;
                    }
                    queueItemTotal--;
                }else{
                    currentFrame++;
                }
            }
            //Serial.println("queueHead " + String(queueHead) + " queueTail " + String(queueTail)+ " queueLength " + String(queueItemTotal));
        };
        bool AddAnimation(Animation animation){
            if (queueItemTotal < MAX_QUEUE){
                animationQueue[queueTail] = animation;
                if(queueTail+1 >= MAX_QUEUE){
                    queueTail = 0;
                }else{
                    queueTail++;
                }
                
                queueItemTotal++;
                //Serial.println("queueHead " + String(queueHead) + " queueTail " + String(queueTail) +" queueLength " + String(queueItemTotal));
                return true;
            }
            return false;
        };
        inline bool Running(){            return running;        };

    private:
        static const uint8_t MAX_QUEUE = 10;
        bool running = false;
        uint16_t frameRate;
        uint32_t currentFrame;
        NhlGame* game;
        NeoPixelStrip* ledStrip;
        unsigned long startTime;
        Animation animationQueue[MAX_QUEUE];
        uint8_t queueHead = 0;
        uint8_t queueTail =0;
        uint8_t queueItemTotal =0;
        
};



#endif 
