#ifndef __DECLARATIONS_H__
#define __DECLARATIONS_H__

#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <string.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <time.h>
#include <memory>
#include <cmath>
#include <functional>
#include <map>
#include "xml/rapidxml.hpp"
#include "xml/rapidxml_iterators.hpp"
#include "xml/rapidxml_print.hpp"
#include "xml/rapidxml_utils.hpp"
#include "Constants.h"

using namespace std;

class Texture;
class Image;
class Animation;
class AnimationSet;
class Object;
class VisibleObject;
class Unit;
class Dialog;
class Grid;
struct Node;
using TexturePtr = shared_ptr<Texture>;
using ImagePtr = shared_ptr<Image>;
using AnimationPtr = shared_ptr<Animation>;
using AnimationSetPtr = shared_ptr<AnimationSet>;
using ObjectPtr = shared_ptr<Object>;
using VisibleObjectPtr = shared_ptr<VisibleObject>;
using UnitPtr = shared_ptr<Unit>;
using NodePtr = shared_ptr<Node>;

using uint = unsigned int;

void        logSDLError( char* msg );
int         GameInitialize( const int winPosX, const int winPosY, Uint32 windowFlag = SDL_WINDOW_SHOWN );
SDL_Color   GetRGBA( eColor::Type );
SHOT_TYPE   StringToShotType( string name );
NodePtr     FindRoute( Grid ** from, Grid * to, int movepoints, TYPE::Name flyingUnit );

//OnClickFunctions
void    NextScreen();
void    PreviousScreen();
void    SwitchColor( PLAYER_COLOR::Color c );
void    SwitchCastle( int playerIndex, bool reverseOrder = false );
void    SwitchHero( int playerIndex, bool reverseOrder = false );
void    ChangePlayerName( int player );
void    ChangeText( ImagePtr & oldText, string newText, int time );
string  GetColorName( PLAYER_COLOR::Color );
void    ChangeIsAI();
void    SetUnitWaiting();
void    SetUnitDefend();
#endif