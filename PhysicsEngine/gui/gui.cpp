#include "simulator/geometry.h"
#include "simulator/spawner.h"
#include <gui/gui.h>
#include <GLFW/glfw3.h>
#include <string>
#include <cfloat>
#include <queue>

using namespace gui;

GUI::GUI(GLFWwindow* window, unsigned int _textureBufferID)
	:shouldShowLoadPopup{ false }, isObjectAttributeWindowFocused{ false }, isObjectDetailsOpen{false}
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 410 core");

	textureBufferID = _textureBufferID;

	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowRounding = 0.0f;
}

void GUI::renderAll(
	unsigned* textures,
	std::queue<std::unique_ptr<Event>>& eventQueue,
	const std::vector<RigidObject*>& objects,
	const bool& isRunning,
	const std::vector<RigidObject*>& selectedObjects
)
{
	
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove;

	renderContentBrowser(textures, windowFlags, eventQueue);
	renderObjectList(windowFlags, eventQueue, objects);
	renderObjectDetails(textures, windowFlags, eventQueue, objects, selectedObjects, isRunning);
	renderSimulationControl(textures, windowFlags, eventQueue, isRunning);
	renderWorldSettings(windowFlags, eventQueue);
	renderScene(windowFlags, eventQueue);

	ImGui::ShowDemoWindow();
	
	//glClearColor(0.f, 0.f, 0.f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT);

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	ImGui::EndFrame();
}

void gui::GUI::showLoadMapPopup(std::queue<std::unique_ptr<Event>>& eventQueue) {

	const float windowWidth = 0.155f * ImGui::GetIO().DisplaySize.x;
	const float windowHeight = 0.1f * ImGui::GetIO().DisplaySize.y;
	const float xOffset = MAIN_SCREEN_WIDTH_PERCENTAGE * ImGui::GetIO().DisplaySize.x - windowWidth;
	const float yOffset = 0.f;
	ImGui::SetNextWindowPos(ImVec2(xOffset, yOffset), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight), ImGuiCond_Once);

	// Create the pop-up window.
	if (ImGui::Begin("Pop-up Window", &shouldShowLoadPopup, ImGuiWindowFlags_NoResize)) {
		ImGui::Text("Please select a preset");

		// Add buttons to the pop-up window based on the number of saved scenarios.
		for (int i{ 1 }; i <= SaveScenarioEvent::savedScenarios; ++i) {
			std::string label{ "preset_" };
			label += std::to_string(i);
			if (ImGui::Button(label.c_str())) {
				shouldShowLoadPopup = false;
				eventQueue.push(std::make_unique<ClearAllObjectsEvent>());
				eventQueue.push(std::make_unique<PresetLoadEvent>(i));
				break;
			}
		}

		ImGui::End();
	}
}

void GUI::renderScene(
	ImGuiWindowFlags windowFlags,
	std::queue<std::unique_ptr<Event>>& eventQueue
)
{
	const float windowWidth = MAIN_SCREEN_WIDTH_PERCENTAGE * ImGui::GetIO().DisplaySize.x;

	ImGui::SetNextWindowPos(ImVec2(0.f,0.f), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(windowWidth, ImGui::GetIO().DisplaySize.y), ImGuiCond_Once);

	ImGui::Begin("Scene", NULL, windowFlags);
	{
		ImGui::BeginChild("SceneRender");

		ImGui::Image(
			(ImTextureID)(uintptr_t)textureBufferID,
			ImGui::GetWindowSize(),
			ImVec2(0, 1),
			ImVec2(1, 0)
		);

		const ImGuiIO& io = ImGui::GetIO();
		ImVec2 clickedPos = io.MouseClickedPos[0];
		if (isInMainSceneWindow(clickedPos)&& isObjectDetailsOpen==false)
		{
			if (ImGui::IsMouseDragging(0))
			{
				ImVec2 dragDelta = io.MouseDelta;
				if (dragDelta.x != 0.0f && dragDelta.y != 0.0f) {
					eventQueue.push(std::make_unique<LeftMouseDragEvent>(dragDelta.x, -dragDelta.y));
				}
			}
			else if (ImGui::IsMouseReleased(0) && io.MouseDownDurationPrev[0] < 0.03f)
			{
				eventQueue.push(std::make_unique<LeftMouseClickEvent>(clickedPos.x, clickedPos.y, io.KeyCtrl));
			}
		}

		clickedPos = io.MouseClickedPos[1];
		if (isInMainSceneWindow(clickedPos) && ImGui::IsMouseDragging(1))
		{
			ImVec2 dragDelta = io.MouseDelta;
			if (dragDelta.x != 0.0f && dragDelta.y != 0.0f)
			{
				eventQueue.push(std::make_unique<RightMouseDragEvent>(
					clickedPos.x + dragDelta.x * 10.0f,
					clickedPos.y + dragDelta.y * 10.0f,
					clickedPos.x,
					clickedPos.y
					));
			}
		}
		ImVec2 mousePos = ImGui::GetMousePos();
		if (isInMainSceneWindow(mousePos))
		{
			float mouseWheel = io.MouseWheel;
			if (mouseWheel != 0.0f) {
				eventQueue.push(std::make_unique<MouseWheelEvent>(mouseWheel));
			}
		}

		ImGui::EndChild();
	}
	ImGui::End();
}

void GUI::renderContentBrowser(unsigned* textures, ImGuiWindowFlags windowFlags, std::queue<std::unique_ptr<Event>>& eventQueue)
{
	const float windowWidth = SETTINGS_SCREEN_WIDTH_PERCENTAGE * ImGui::GetIO().DisplaySize.x;
	const float windowHeight = 0.15f * ImGui::GetIO().DisplaySize.y;
	const float xOffset = MAIN_SCREEN_WIDTH_PERCENTAGE * ImGui::GetIO().DisplaySize.x;
	const float yOffset = 0.f;

	ImGui::SetNextWindowPos(ImVec2(xOffset,yOffset), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(windowWidth,windowHeight), ImGuiCond_Once);

	ImGui::Begin("ContentBrowser", NULL, windowFlags);
	{
		ImGui::BeginChild("ContentBrowserRender");

		if (ImGui::BeginTabBar("TabBar"))
		{
			if (ImGui::BeginTabItem("Add New"))
			{
				ImGui::Spacing();
				ImVec2 buttonSize(100, 100);
				if (ImGui::ImageButton(reinterpret_cast<ImTextureID>(textures[TextureID::SPHERE]), buttonSize))
				{
					eventQueue.push(std::make_unique<ObjectAddEvent>(ObjectType::SPHERE));
				}
				ImGui::SameLine();

				if (ImGui::ImageButton(reinterpret_cast<ImTextureID>(textures[TextureID::CUBE]), buttonSize))
				{
					eventQueue.push(std::make_unique<ObjectAddEvent>(ObjectType::BOX));
				}
				ImGui::SameLine();

				if (ImGui::ImageButton(reinterpret_cast<ImTextureID>(textures[TextureID::SPAWNER]), buttonSize))
				{
					eventQueue.push(std::make_unique<ObjectAddEvent>(ObjectType::SPAWNER));
				}
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
		ImGui::EndChild();
	}
	ImGui::End();
}

void GUI::renderSimulationControl(
	unsigned* textures,
	ImGuiWindowFlags windowFlags,
	std::queue<std::unique_ptr<Event>>& eventQueue,
	const bool& isRunning
)
{
	if (isRunning == true) {
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.15f, 0.85f, 0.05f, 0.5f));
	}
	else {
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.95f, 0.15f, 0.05f, 0.5f));
	}

	const float windowWidth = 0.065f * ImGui::GetIO().DisplaySize.x;
	const float windowHeight = 0.1f * ImGui::GetIO().DisplaySize.y;
	const float xOffset = MAIN_SCREEN_WIDTH_PERCENTAGE * ImGui::GetIO().DisplaySize.x;
	const float yOffset = 0.55f * ImGui::GetIO().DisplaySize.y;

	ImGui::SetNextWindowPos(ImVec2(xOffset, yOffset), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight), ImGuiCond_Once);

	ImGui::Begin("SimulationControl", NULL, windowFlags);
	{
		ImGui::BeginChild("SimulationControlRender");


		if (ImGui::Button("load")) {
			shouldShowLoadPopup = true;
		}
		if (shouldShowLoadPopup == true) {
			showLoadMapPopup(eventQueue);
		}
		ImGui::SameLine();

		if (ImGui::Button("save")) {
			eventQueue.push(std::make_unique<SaveScenarioEvent>());
		}

		// Display the appropriate image based on the simulation status
		unsigned buttonTextureID = isRunning ? textures[TextureID::PAUSE] : textures[TextureID::PLAY];
		ImVec2 buttonSize(100, 50);

		if (ImGui::ImageButton(reinterpret_cast<ImTextureID>(buttonTextureID), buttonSize))
		{
			eventQueue.push(std::make_unique<SimulationToggleEvent>());
		}
		ImGui::EndChild();
	}


	ImGui::End();
	ImGui::PopStyleColor();
}

void GUI::renderObjectList(
	ImGuiWindowFlags windowFlags,
	std::queue<std::unique_ptr<Event>>& eventQueue,
	const std::vector<RigidObject*>& objects
)
{
	const float windowWidth = SETTINGS_SCREEN_WIDTH_PERCENTAGE * ImGui::GetIO().DisplaySize.x;
	const float windowHeight = 0.3f * ImGui::GetIO().DisplaySize.y;
	const float xOffset = MAIN_SCREEN_WIDTH_PERCENTAGE * ImGui::GetIO().DisplaySize.x;
	const float yOffset = 0.15f * ImGui::GetIO().DisplaySize.y;

	ImGui::SetNextWindowPos(ImVec2(xOffset, yOffset), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight), ImGuiCond_Once);

	ImGui::Begin("ObjectList", NULL, windowFlags);
	{
		ImGui::BeginChild("ObjectListRender");
		if (ImGui::BeginTabBar("TabBar"))
		{
			if (ImGui::BeginTabItem("World Outliner"))
			{
				ImGui::Spacing();
				// Maintain separate counts for cube and sphere objects
				int boxCount{};
				int spawnerCount{};
				int sphereCount{};

				for (const auto& object : objects)
				{
					if (object == nullptr) {
						continue;
					}
					ImGui::PushID(object);

					std::string objectName;
					if (dynamic_cast<SphereObject*>(object) != nullptr)
					{
						objectName = "sphere";
						objectName += std::to_string(++sphereCount);
					}
					else if (dynamic_cast<SphereBoxSpawner*>(object) != nullptr)
					{
						objectName = "Spawner";
						objectName += std::to_string(++spawnerCount);
					}
					else if (dynamic_cast<BoxObject*>(object) != nullptr)
					{
						objectName = "cube";
						objectName += std::to_string(++boxCount);
					}

					//multiple select with the ctrl key
					if (ImGui::Selectable(objectName.c_str(), object->GetIsSelected(), 0, ImVec2(50, 20)))
					{
						if (ImGui::GetIO().KeyCtrl) {
							eventQueue.push(std::make_unique<ObjectSelectEvent>(object, true));
						}
						else {
							eventQueue.push(std::make_unique<ObjectSelectEvent>(object, false));
						}
					}

					ImGui::PopID();
				}

				if (ImGui::Button("remove")) {
					eventQueue.push(std::make_unique<ObjectRemoveEvent>());
				}

				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}

		ImGui::EndChild();
	}
	ImGui::End();
}

void GUI::renderObjectDetails(
	unsigned* textures,
	ImGuiWindowFlags windowFlags,
	std::queue<std::unique_ptr<Event>>& eventQueue,
	const std::vector<RigidObject*>& objects,
	const std::vector<RigidObject*>& selectedObjects,
	const bool& isRunning
)
{

	ImGui::Begin("Details", NULL, windowFlags);
	{
		ImGui::BeginChild("DetailsRender");

		if (ImGui::BeginTabBar("TabBar"))
		{
			if (ImGui::BeginTabItem("Details"))
			{
				ImGui::Spacing();
				renderObjectAttribute(textures, windowFlags, eventQueue, objects, selectedObjects, isRunning);
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}

		ImGui::EndChild();
	}
	ImGui::End();
}

void GUI::renderObjectAttribute(
	unsigned* textures,
	ImGuiWindowFlags windowFlags,
	std::queue<std::unique_ptr<Event>>& eventQueue,
	const std::vector<RigidObject*>& objects,
	const std::vector<RigidObject*>& selectedObjects,
	const bool& isRunning
)
{
	if (selectedObjects.size() != 1) {
		return;
	}

	RigidObject* object = selectedObjects[0];
	bool isObjectFixed = object->GetIsFixed();

	// Begin the main ImGui window
	isObjectDetailsOpen=ImGui::Begin("Object Attribute", NULL, windowFlags);
	{
		// Set isObjectAttributeWindowFocused flag
		isObjectAttributeWindowFocused = ImGui::IsWindowFocused() || ImGui::IsWindowHovered();
		// Check if Object Attribute window is not focused or hovered
		if (isObjectAttributeWindowFocused ==false
			&& isInMainSceneWindow(ImGui::GetIO().MousePos)==true
			&& ImGui::IsMouseDown(0)==true)
		{
			eventQueue.push(std::make_unique<DeselectObjectsEvent>());
		}
	
		math::Vector3 position = object->GetPosition();
		ImGui::Spacing();
		ImGui::Text("Position");
		ImGui::Text("x"); ImGui::SameLine();
		if (ImGui::DragFloat("##PosX", &position[0], 0.05f)) {
			eventQueue.push(std::make_unique<ObjectPositionEvent>(selectedObjects[0], position));
		}
		ImGui::Text("y"); ImGui::SameLine();
		if (ImGui::DragFloat("##PosY", &position[1], 0.05f)) {
			eventQueue.push(std::make_unique<ObjectPositionEvent>(selectedObjects[0], position));
		}
		ImGui::Text("z"); ImGui::SameLine();
		if (ImGui::DragFloat("##PosZ", &position[2], 0.05f)) {
			eventQueue.push(std::make_unique<ObjectPositionEvent>(selectedObjects[0], position));
		}

		math::Vector3 velocity = object->GetLinearVelocity();
		ImGui::Spacing();
		ImGui::Text("Velocity");
		if (!isRunning) {
			ImGui::Text("x"); ImGui::SameLine();
			if (ImGui::DragFloat("##VelX", &velocity[0], 0.1f)) {
				eventQueue.push(std::make_unique<ObjectVelocityEvent>(selectedObjects[0], velocity));
			}
			ImGui::Text("y"); ImGui::SameLine();
			if (ImGui::DragFloat("##VelY", &velocity[1], 0.1f)) {
				eventQueue.push(std::make_unique<ObjectVelocityEvent>(selectedObjects[0], velocity));
			}
			ImGui::Text("z"); ImGui::SameLine();
			if (ImGui::DragFloat("##VelZ", &velocity[2], 0.1f)) {
				eventQueue.push(std::make_unique<ObjectVelocityEvent>(selectedObjects[0], velocity));
			}
		}

		ImGui::Spacing();
		ImGui::Text("Orientation");
		ImGui::SameLine();
		if (ImGui::Button("Reset##Orientation")) {
			eventQueue.push(std::make_unique<OrientationResetEvent>(selectedObjects[0]));
		}
		ImVec2 buttonSize(static_cast<float>(ImGui::GetColumnWidth()*0.33), 0.0f);

		ImGui::Button("x", buttonSize);
		if (ImGui::IsItemActive()){
			eventQueue.push(std::make_unique<ObjectRotateEvent>(selectedObjects[0], 1.0f, 0.0f, 0.0f, ImGui::GetIO().MouseDelta.x));
			eventQueue.push(std::make_unique<ToggleWorldAxisRenderEvent>(selectedObjects[0], 0));
		}
		if (ImGui::IsItemHovered())
		{
			eventQueue.push(std::make_unique<ToggleWorldAxisRenderEvent>(selectedObjects[0], 0));
		}
		ImGui::SameLine();

		ImGui::Button("y", buttonSize);
		if (ImGui::IsItemActive()) {
			eventQueue.push(std::make_unique<ObjectRotateEvent>(selectedObjects[0], 0.0f, 1.0f, 0.0f, ImGui::GetIO().MouseDelta.x));
			eventQueue.push(std::make_unique<ToggleWorldAxisRenderEvent>(selectedObjects[0], 1));
		}
		if (ImGui::IsItemHovered())
		{
			eventQueue.push(std::make_unique<ToggleWorldAxisRenderEvent>(selectedObjects[0], 1));
		}
		ImGui::SameLine();

		ImGui::Button("Z", buttonSize);
		if (ImGui::IsItemActive()) {
			eventQueue.push(std::make_unique<ObjectRotateEvent>(selectedObjects[0], 0.0f, 0.0f, 1.0f, ImGui::GetIO().MouseDelta.x));
			eventQueue.push(std::make_unique<ToggleWorldAxisRenderEvent>(selectedObjects[0], 2));		
		}
		if (ImGui::IsItemHovered())
		{
			eventQueue.push(std::make_unique<ToggleWorldAxisRenderEvent>(selectedObjects[0], 2));
		}

		// Mass
		float mass=object->GetMass();
		ImGui::Spacing();
		ImGui::Text("Mass");
		if (ImGui::DragFloat("##Mass", &mass, 0.1f, 0.1f, FLT_MAX)) {
			eventQueue.push(std::make_unique<ObjectMassEvent>(selectedObjects[0], mass));
		}

		// Scale
		math::Vector3 GRID_SCALE = object->GetScale();
		math::Vector3 halfSize{ GRID_SCALE.x, GRID_SCALE.y, GRID_SCALE.z };
		if (dynamic_cast<const BoxObject*>(object) != nullptr) {
			ImGui::Spacing();
			ImGui::Text("Extents");
			ImGui::SameLine();
			if (ImGui::Button("Reset##halfSize")) {
				halfSize = { 0.5f,0.5f,0.5f };
				eventQueue.push(std::make_unique<ObjectScaleEvent>(selectedObjects[0], halfSize));
			}
			ImGui::Text("x");
			ImGui::SameLine();
			if (ImGui::DragFloat("##extents-X", &halfSize[0], 0.01f, 0.1f, FLT_MAX)) {
				eventQueue.push(std::make_unique<ObjectScaleEvent>(selectedObjects[0], halfSize));
			}

			ImGui::Text("y");
			ImGui::SameLine();
			if (ImGui::DragFloat("##extents-Y", &halfSize[1], 0.01f, 0.1f, FLT_MAX)) {
				eventQueue.push(std::make_unique<ObjectScaleEvent>(selectedObjects[0], halfSize));
			}

			ImGui::Text("z");
			ImGui::SameLine();
			if (ImGui::DragFloat("##extents-Z", &halfSize[2], 0.01f, 0.1f, FLT_MAX)) {
				eventQueue.push(std::make_unique<ObjectScaleEvent>(selectedObjects[0], halfSize));
			}
		}
		else if (dynamic_cast<const SphereObject*>(object) != nullptr) {
			ImGui::Spacing();
			ImGui::Text("Radius");
			ImGui::SameLine();
			if (ImGui::Button("Reset##radius")) {
				halfSize[0] = 1.0f;
				eventQueue.push(std::make_unique<ObjectScaleEvent>(selectedObjects[0], halfSize));
			}
			if (ImGui::DragFloat("##Radius", &halfSize[0], 0.01f, 0.1f, FLT_MAX)) {
				eventQueue.push(std::make_unique<ObjectScaleEvent>(selectedObjects[0], halfSize));
			}
		}

		// Texture selection
		ImGui::Spacing();
		ImGui::Text("Select a texture for the object:");
		for (int i = 0; i < graphics::NUM_OBJECT_TEXTURES; ++i) {
			if (ImGui::ImageButton((ImTextureID)textures[i], ImVec2(64, 64))) {
				// Update the texture ID for the selected object
				object->GetShape()->SetTextureID(i);
			}
			if ((i + 1 != graphics::NUM_OBJECT_TEXTURES )//not the last one
				&&((i + 1) % 3 != 0)){					 //3 in a row
				ImGui::SameLine();
			}
		}

		if (ImGui::Checkbox("Fixed", &isObjectFixed)) {
			eventQueue.push(std::make_unique<ObjectFixPositionEvent>(selectedObjects[0], isObjectFixed));
		}

		//if (dynamic_cast<SphereBoxSpawner*>(selectedObjects[0]) != nullptr) {
		//	if (ImGui::Button("Spawn All")) {
		//		dynamic_cast<SphereBoxSpawner*>(selectedObjects[0])->spawnAll();
		//	}
		//}
	}
	// End the main ImGui window
	ImGui::End();
}


void GUI::renderAttributeDetails(ImGuiWindowFlags windowFlags, std::queue<std::unique_ptr<Event>>& eventQueue)
{
	static bool shouldRenderContactInfo = false;
	static float timeStep = 1.0f;
	static float gravity = 9.8f;
	static float groundRestitution = 0.2f;
	static float objectRestitution = 0.3f;

	// Begin the main ImGui window
	ImGui::Begin("Attribute", NULL, windowFlags);
	{
		// Begin a child window for the physics attributes
		ImGui::BeginChild("Physics");

		// Begin the tab bar
		if (ImGui::BeginTabBar("TabBar"))
		{
			// Gravity tab
			if (ImGui::BeginTabItem("Gravity"))
			{
				ImGui::Spacing();
				ImGui::Text("Gravity");
				if (ImGui::SliderFloat("##Gravity", &gravity, 0.0f, 30.0f))
				{
					eventQueue.push(std::make_unique<GravityEvent>(gravity));
				}
				ImGui::SameLine();
				if (ImGui::Button("Reset##Gravity"))
				{
					gravity = 9.8f;
					eventQueue.push(std::make_unique<GravityEvent>(gravity));
				}

				ImGui::EndTabItem();
			}

			// Restitution tab
			if (ImGui::BeginTabItem("Restitution"))
			{
				ImGui::Spacing();
				ImGui::Text("Ground Restitution");
				if (ImGui::SliderFloat("##Ground_Restitution", &groundRestitution, 0.0f, 1.0f))
				{
					eventQueue.push(std::make_unique<GroundRestitutionEvent>(groundRestitution));
				}
				ImGui::SameLine();
				if (ImGui::Button("Reset##GroundRestitution"))
				{
					groundRestitution = 0.2f;
					eventQueue.push(std::make_unique<GroundRestitutionEvent>(groundRestitution));
				}

				ImGui::Text("Object Restitution");
				if (ImGui::SliderFloat("##Object_Restitution", &objectRestitution, 0.0f, 1.0f))
				{
					eventQueue.push(std::make_unique<ObjectRestitutionEvent>(objectRestitution));
				}
				ImGui::SameLine();
				if (ImGui::Button("Reset##ObjectRestitution"))
				{
					objectRestitution = 0.3f;
					eventQueue.push(std::make_unique<ObjectRestitutionEvent>(objectRestitution));
				}

				ImGui::EndTabItem();
			}

			// Contact Info tab
			if (ImGui::BeginTabItem("Contact Info"))
			{
				ImGui::Spacing();
				if (ImGui::Checkbox("Render contact info.", &shouldRenderContactInfo))
				{
					eventQueue.push(std::make_unique<ToggleContactRenderEvent>(shouldRenderContactInfo));
				}

				ImGui::EndTabItem();
			}

			// Time Step tab
			if (ImGui::BeginTabItem("Time Step"))
			{
				ImGui::Spacing();
				ImGui::Text("Time step");
				if (ImGui::SliderFloat("##TimeStep", &timeStep, 0.3f, 1.0f))
				{
					eventQueue.push(std::make_unique<TimeStepChangeEvent>(timeStep));
				}
				ImGui::SameLine();
				if (ImGui::Button("Reset##TimeStep"))
				{
					timeStep = 1.0f;
					eventQueue.push(std::make_unique<TimeStepChangeEvent>(timeStep));
				}

				ImGui::EndTabItem();
			}

			// End the tab bar
			ImGui::EndTabBar();
		}

		// End the child window for the physics attributes
		ImGui::EndChild();
	}

	// End the main ImGui window
	ImGui::End();
}

void gui::GUI::renderWorldSettings(ImGuiWindowFlags windowFlags, std::queue<std::unique_ptr<Event>>& eventQueue)
{
	const float windowWidth = SETTINGS_SCREEN_WIDTH_PERCENTAGE * ImGui::GetIO().DisplaySize.x;
	const float windowHeight = 0.09f * ImGui::GetIO().DisplaySize.y;
	const float xOffset = MAIN_SCREEN_WIDTH_PERCENTAGE * ImGui::GetIO().DisplaySize.x;
	const float yOffset = 0.45f * ImGui::GetIO().DisplaySize.y;

	ImGui::SetNextWindowPos(ImVec2(xOffset, yOffset), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight), ImGuiCond_Once);

	ImGui::Begin("Attribute", NULL, windowFlags);
	{
		ImGui::BeginChild("AttributeRender");

		if (ImGui::BeginTabBar("TabBar"))
		{
			if (ImGui::BeginTabItem("World Settings"))
			{
				ImGui::Spacing();
				renderAttributeDetails(windowFlags, eventQueue);
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}

		ImGui::EndChild();
	}
	ImGui::End();
}

bool GUI::isInMainSceneWindow(ImVec2 vec)
{
	float mainScreenWidth = MAIN_SCREEN_WIDTH_PERCENTAGE * ImGui::GetIO().DisplaySize.x;
	if (vec.x > 0.0f && vec.x < mainScreenWidth
		&& vec.y > 0.0f && vec.y < ImGui::GetIO().DisplaySize.y) {
		return true;
	}
	else {
		return false;
	}
}
