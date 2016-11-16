#include "App.h"
using namespace std;


//------------------------------------------------------------------------------
// Constructors - Destructor
//------------------------------------------------------------------------------
App::App(){
	clog<<"[INFO] Create application."<<endl;
	isRunning = false;
}

App::~App(){
	clog<<"[INFO] Destroys application."<<endl;
}


//------------------------------------------------------------------------------
// Body function (Initialization - Stop)
//------------------------------------------------------------------------------
int App::executeApp(){
	//App should be already running
	if(isRunning){ return false; }
	clog<<"[INFO] Start running application..."<<endl;

	//Initialize application
	if(initApp()==false){
		clog<<"[ERR] Unable to run application (Init failed)..."<<endl;
		return -1;
	}
	isRunning = true; //Here, actually start running

	//Main execution loop
	SDL_Event sdlevent;
	while(isRunning){
		while(SDL_PollEvent(&sdlevent)){
			doEvent(&sdlevent);
		}
		doLoop();
		doRender();
	}

	//Stop application
	doCleanup();
	return 0;
}

bool App::initApp(){
	//Init SDL
	if(SDL_Init(SDL_INIT_EVERYTHING)<0){
		clog<<"[ERR] Unable to init App. SDL_Error: "<<SDL_GetError()<<endl;
		return false;
	}

	//Create SDL window for screen
	window = SDL_CreateWindow(
		WINDOWS_TITLE,
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		SCREEN_WIDTH,
		SCREEN_HEIGHT,
		SDL_WINDOW_SHOWN
	);
	if(window==NULL){
		clog<<"[ERR] :: Unable to start window. SDL_Error: "<<SDL_GetError()<<endl;
		return false;
	}

	//Load SDL_Surface from the window
	screen = SDL_GetWindowSurface(window);
	SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0xFF, 0xFF, 0xFF));
	SDL_UpdateWindowSurface(window);

	//LOAD CONTENT
	//Load entities
	if(entity1.loadEntity("./data/images/yoshi.bmp", 64, 64, 8)==false){
		return false;
	}
	Entity::listEntities.push_back(&entity1);
	//Load area
	if(Area::areaControl.loadArea("./data/maps/area1.area")==false){
		clog<<"[ERR] :: Unable to load the area (./data/maps/area1.area)."<<endl;
		return false;
	}
	return true;
}

void App::doCleanup(){
	//Cleanup each entity
	for(int i = 0; i<Entity::listEntities.size(); i++){
		if(!Entity::listEntities[i]) { continue; }
		Entity::listEntities[i]->doCleanup();
	}
	Entity::listEntities.clear();
	//Cleanup area
	Area::areaControl.cleanupArea();
	//Cleanup App
	SDL_FreeSurface(screen);
	SDL_DestroyWindow(window);
	SDL_Quit();
}


//------------------------------------------------------------------------------
// Body function (Execution)
//------------------------------------------------------------------------------
void App::doEvent(SDL_Event* sdlevent){
	EventManager::onEvent(sdlevent);
}

void App::doLoop(){
	//Loop on each Entity
	for(int i = 0; i<Entity::listEntities.size(); i++){
		if(!Entity::listEntities[i]) { continue; }
		Entity::listEntities[i]->doLoop();
	}
}

void App::doRender(){
	//Refresh background to white
	SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0x00, 0x00, 0x00));
	//Render Map
	Area::areaControl.renderArea(screen, Camera::cameraControl.getX(), Camera::cameraControl.getY());
	//Render each Entity
	for(int i = 0; i<Entity::listEntities.size(); i++){
		if(!Entity::listEntities[i]) { continue; }
		Entity::listEntities[i]->doRender(screen);
	}
	//General refresh
	SDL_UpdateWindowSurface(window);
}


//------------------------------------------------------------------------------
// Override function (EventManager)
//------------------------------------------------------------------------------
void App::onExit(){
	isRunning = false;
}

void App::onKeyDown(SDL_Keysym keysym){
	switch(keysym.sym){
		//Arrow keys
		case SDLK_UP: Camera::cameraControl.moveCamera(0, 5); break;
		case SDLK_DOWN: Camera::cameraControl.moveCamera(0, -5); break;
		case SDLK_LEFT: Camera::cameraControl.moveCamera(5, 0); break;
		case SDLK_RIGHT: Camera::cameraControl.moveCamera(-5, 0); break;
		default: break;
	}
}