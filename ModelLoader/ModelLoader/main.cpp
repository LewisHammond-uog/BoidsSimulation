
#include "Scene.h"

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




