#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h>

#include <string>
#include "SceneManager.h"
#include "Debug.h"
#include "MemoryDiagnostics.h"

  
int main(int argc, char* args[]) {
	// Enable memory diagnostics tracking
	MemoryDiagnostics::EnableDebugMemoryTracking();

	Debug::DebugInit("GameEngineLog.txt");
	
	SceneManager* gsm = new SceneManager();
	if (gsm->Initialize("Game Engine", 1280, 720) ==  true) {
		gsm->Run();
	} 
	delete gsm;
	
	// Final memory leak check
	std::cout << "\n=== Final Memory Leak Report ===" << std::endl;
	_CrtDumpMemoryLeaks();
	exit(0);
}