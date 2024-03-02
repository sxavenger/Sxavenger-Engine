#include <MyEngine.h> // sxavenger engine

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
/// lib
#include <Environment.h>
// Geometry
#include <Vector4.h>
#include <Matrix4x4.h>
#include <Vector2.h>
// Camera
#include <Camera3D.h>
#include <Camera2D.h>
// Light
#include <DirectionalLight.h>

// Object_Ptr
#include "ObjectPtr.h"

// c++
#include <list>

/// Game
// Object
#include <Triangle.h>
#include <Sphere.h>
#include <Sprite.h>
#include <Plane.h>
#include <Teapot.h>
#include <Bunny.h>
#include <MultiMesh.h>
#include <MultiMaterial.h>
#include <Suzanne.h>

////////////////////////////////////////////////////////////////////////////////////////////
// メイン関数
////////////////////////////////////////////////////////////////////////////////////////////
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	//=========================================================================================
	// 初期化
	//=========================================================================================
	MyEngine::Initialize(kWindowWidth, kWindowHeight, kWindowTitle);

	ID3D12GraphicsCommandList* commandList = MyEngine::GetCommandList();
	commandList;

	//-----------------------------------------------------------------------------------------
	// Camera
	//-----------------------------------------------------------------------------------------
	Object_Ptr<Camera3D> camera3D = new Camera3D("camera3D.json");
	MyEngine::camera3D_ = camera3D.Get();
	Object_Ptr<Camera2D> camera2D = new Camera2D();
	MyEngine::camera2D_ = camera2D.Get();

	//-----------------------------------------------------------------------------------------
	// Light
	//-----------------------------------------------------------------------------------------
	Object_Ptr<Directional> directional = new Directional(MyEngine::GetDevice());

	//-----------------------------------------------------------------------------------------
	// Object
	//-----------------------------------------------------------------------------------------
	std::list<Object*> objects;

	// list
	const char* items[] = { "Triangle", "Sphere", "Sprite", "Plane", "Teapot", "Bunny", "MultiMesh", "MultiMaterial", "Suzanne" };
	enum ItemType {
		Type_Traingle,
		Type_Sphere,
		Type_Sprite,
		Type_Plane,
		Type_Teapot,
		Type_Bunny,
		Type_MultiMesh,
		Type_MultiMaterial,
		Type_Suzanne,

		kItemTypeCount
	};

	// select
	int currentType = Type_Traingle;

	////////////////////////////////////////////////////////////////////////////////////////////
	// メインループ
	////////////////////////////////////////////////////////////////////////////////////////////
	while (MyEngine::ProcessMessage() == 0) {

		MyEngine::BeginFrame();

		//=========================================================================================
		// 更新処理
		//=========================================================================================
		camera3D->UpdateImGui();
		directional->UpdateImGui();

		ImGui::Begin("object");

		// object追加処理
		if (ImGui::BeginCombo("object", items[currentType])) {

			for (int i = 0; i < ItemType::kItemTypeCount; ++i) {
				// item[i]が選択されてるかどうか
				bool isSelected = (i == currentType);

				if (ImGui::Selectable(items[i], isSelected)) {
					currentType = i;
				}
			}

			ImGui::EndCombo();
		}

		ImGui::SameLine();

		if (ImGui::Button("create")) {
			switch (currentType) {
				case Type_Traingle:
					objects.push_back(new Triangle());
					break;

				case Type_Sphere:
					objects.push_back(new Sphere());
					break;

				case Type_Sprite:
					objects.push_back(new Sprite());
					break;

				case Type_Plane:
					objects.push_back(new Plane());
					break;

				case Type_Teapot:
					objects.push_back(new Teapot());
					break;

				case Type_Bunny:
					objects.push_back(new Bunny());
					break;

				case Type_MultiMesh:
					objects.push_back(new MultiMesh());
					break;

				case Type_MultiMaterial:
					objects.push_back(new MultiMaterial());
					break;

				case Type_Suzanne:
					objects.push_back(new Suzanne());
					break;

				default:
					break;
			}
			
		}

		ImGui::Separator();

		// object単体の処理
		int identifier = 0;
		for (auto it = objects.begin(); it != objects.end();) {

			(*it)->EditorImGui(identifier);
			identifier++;
			ImGui::Separator();
			
			if ((*it)->GetIsDelete()) {
				// listからの削除
				delete* it;
				it = objects.erase(it);
				continue;
			}

			++it;
		}

		ImGui::Spacing();

		if (ImGui::Button("clear")) {
			// object
			for (auto& it : objects) {
				delete it;
				it = nullptr;
			}
			objects.clear();
		}

		ImGui::End();

		//=========================================================================================
		// 描画処理
		//=========================================================================================
		

		// object 描画処理
		for (const auto& it : objects) {
			it->Draw(commandList, directional.Get());
		}

		MyEngine::EndFrame();

	}

	// camera
	camera3D.Release();
	camera2D.Release();
	// light
	directional.Release();

	// object
	for (auto& it : objects) {
		delete it;
		it = nullptr;
	}
	objects.clear();

	MyEngine::Finalize();
	return 0;
}