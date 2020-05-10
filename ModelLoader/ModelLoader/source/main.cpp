/// <summary>
/// Boids Application created for CT 5037 by Lewis Hammond (s1800707)
/// Generates a number of boid like objects and applies flocking, steering and collision avoidance
/// behavours to them.
///
/// Uses:
/// ReactPhysics3D
/// DearImgui
/// GLAD
/// GLFW
/// OpenGL
/// AssImp
/// STB Image
/// Gizmos.h
///
/// Fish Models from Turbo Squid: https://www.turbosquid.com/FullPreview/Index.cfm/ID/652729
/// </summary>


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
		const bool bIsInitalised = pScene->Initialize(true);

		if (bIsInitalised) {

			bool bKeepRunning = true;

			while (bKeepRunning) {
				bKeepRunning = pScene->Update();
				pScene->Render();
			}

			pScene->DeInitialize(true);
		}
	}

	delete pScene;


    return 0;
}




