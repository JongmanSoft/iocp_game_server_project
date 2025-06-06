#include "stdafx.h"
#include "Scene.h"
#include "Framework.h"

void Scene::scene_change(int next_scene)
{
	Framework* FW = (Framework*)m_fw;
	FW->scene_change(next_scene);
}
