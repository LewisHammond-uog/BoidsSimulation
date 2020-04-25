
#include "Scene.h"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif
// ReSharper disable once CppUnusedIncludeDirective
#include <stb/stb_image.h>

int main()
{
    
	Scene* pScene = Scene::GetInstance();

	if (pScene) {

		bool bIsInitalised = pScene->Initalise();

		if (bIsInitalised) {

			bool bKeepRunning = true;

			while (bKeepRunning) {
				bKeepRunning = pScene->Update();
				pScene->Render();
			}

			pScene->DeInitlise();
		}
	}

	delete pScene;


    return 0;
}




