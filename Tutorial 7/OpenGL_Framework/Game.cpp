#include "Game.h"
#include "ResourceManager.h"
#include "TextureCube.h"
#include "UI.h"
#include "SatMath.h"

#include <vector>
#include <string>
#include <fstream>
#include <random>

Game::Game()
{
	updateTimer = new Timer();
}

Game::~Game()
{
	delete updateTimer;
}

int activeToonRamp = 0;
bool toonActive = false;

constexpr int frameTimeNumSamples = 600;
int frameTimeCurrSample = 0;
float frameTimeSamples[frameTimeNumSamples];

bool guiGrayscaleActive = false;
float guiGrayscaleAmount = 0.5f;
bool guiGammaActive = true;
float guiGammaAmount = 2.2f;
bool guiWaveActive = false;
vec2 guiWaveCount = vec2(4.0f);
vec2 guiWaveIntensity = vec2(0.03f);
vec2 guiWaveSpeed = vec2(1.00f);
bool guiChromAberActive = false;
vec3 guiChromAberOffset = vec3(-0.01f, 0.0f, 0.01f);
float guiChromAberExponent = 2.0f;
bool guiRGBActive = false;
float guiRGBRange = 0.0f;
vec2 guiRGBrOffset = vec2(-0.005f, 0.0f);
vec2 guiRGBgOffset = vec2(0.0f, 0.0f);
vec2 guiRGBbOffset = vec2(0.005f, 0.0f);

int guiViewGBuffer = 0;
bool guiViewGBufferOverride = false;

void Game::initializeGame()
{
	for (int i = 0; i < frameTimeNumSamples; ++i)
		frameTimeSamples[i] = 0.016f;
	if(guiEnabled && !UI::isInit)
		UI::InitImGUI();
	
	Camera::initCameras();
	ShaderProgram::initDefault();
	Framebuffer::initFrameBuffers();
	Material::initDefaultTextures();
	gbuffer.init(windowWidth, windowHeight);
	gbuffer.setDebugNames();
	shadowFramebuffer.addDepthTarget();
	shadowFramebuffer.init(2048, 2048);
	framebufferDeferredLight.addColorTarget(GL_R11F_G11F_B10F);
	framebufferDeferredLight.init(windowWidth, windowHeight);
	ppBuffer.setFormat(GL_R11F_G11F_B10F);
	ppBuffer.init(windowWidth, windowHeight);
	framebufferTV.addDepthTarget();
	framebufferTV.addColorTarget(GL_RGB8);
	framebufferTV.init(128, 128);

	meshStan.loadFromObj("stan_big.obj");
	meshSphere.initMeshSphere(32U, 32U);
	meshSkybox.initMeshSphere(32U, 32U, true);
	meshLight.initMeshSphere(6U, 6U);
	meshPlane.initMeshPlane(32U, 32U);
	
	shaderForward.load("shader.vert", "shaderForward.frag");
	shaderSky.load("shaderSky.vert", "shaderSky.frag");
	shaderPassthrough.load("PassThrough.vert", "PassThrough.frag");
	shaderGrayscale.load("PassThrough.vert", "Post/GreyscalePost.frag");
	shaderGbuffer	.load("shader.vert", "gBuffer.frag");
	shaderDeferredPointLight.load("PassThrough.vert", "dPointLight.frag");
	shaderDeferredDirectionalLight.load("PassThrough.vert", "dDirectionalLight.frag");
	shaderDeferredEmissive.load("PassThrough.vert", "dEmissive.frag");
	shaderPostWave.load("PassThrough.vert", "Post/WavePost.frag");
	shaderPostGamma.load("PassThrough.vert", "Post/Gamma.frag");
	
	shaderPostDOFX.load("PassThrough.vert", "Post/dofHorizontal.frag");
	shaderPostDOFY.load("PassThrough.vert", "Post/dofVertical.frag");
	uniformBufferDOF.allocateMemory(sizeof(vec4) * 2);
	
	shaderPostSplitRGB.load("PassThrough.vert", "Post/RGBSplitPost.frag");
	shaderPostChromAber.load("PassThrough.vert", "Post/ChromaticAberrationPostFast.frag");

	
	ResourceManager::Shaders.push_back(&shaderForward);
	ResourceManager::Shaders.push_back(&shaderSky);
	ResourceManager::Shaders.push_back(&shaderPassthrough);
	ResourceManager::Shaders.push_back(&shaderGrayscale);
	ResourceManager::Shaders.push_back(&shaderGbuffer);
	ResourceManager::Shaders.push_back(&shaderDeferredPointLight);
	ResourceManager::Shaders.push_back(&shaderDeferredDirectionalLight);
	ResourceManager::Shaders.push_back(&shaderDeferredEmissive);
	
	uniformBufferTime.allocateMemory(sizeof(float));
	uniformBufferTime.bind(1);
	uniformBufferLightScene.allocateMemory(sizeof(vec4));
	uniformBufferLightScene.bind(2);
	
	lightKey.init();
	lightFill.init();
	lightRim.init();
	lightKey.color.xyz = vec3(1.0f, 1.0f, 0.5f) * 2.0f;
	lightFill.color.xyz = vec3(1.0f, 1.0f, 0.5f) * 0.25f;
	lightRim.color.xyz = vec3(0.5f, 0.5f, 1.0f) * 4.0f;
	lightShadow.init();
	light.init();
	light.bind();
	light.quadAtten = 0.01f;
	lights.reserve(64);
	for (int i = 0; i < 64; ++i)
	{
		lights.push_back(Light());
		lights[i].init();
		lights[i].color.rgb = random3(0.3f, 1.0f);
		lights[i].color.a = 1.0f;
		lights[i].constantAtten = 0.1f;
		lights[i].linearAtten = 0.05f;
		lights[i].quadAtten = 0.05f;
		//lights[i]
	}

	uniformBufferToon.allocateMemory(sizeof(int) * 4);
	uniformBufferToon.bind(5);
	uniformBufferToon.sendUInt(false, 0);

	uniformBufferLightScene.sendVector(vec3(0.0f), 0);
	//uniformBufferLight.sendVector(vec3(1.0f), sizeof(vec4)); // Set color of light to white

	Texture* texBlack = new Texture("black.png");
	//Texture* texWhite = new Texture("white.png");
	shadowTexture.load("flashlight.png");

	textureToonRamp.push_back(new Texture("tinyramp.png", false));
	textureToonRamp[0]->setWrapParameters(GL_CLAMP_TO_EDGE);
	textureToonRamp[0]->sendTexParameters();

	std::vector<Texture*> texTV = { texBlack, &framebufferTV._Color._Tex[0] , texBlack };
	//goStan = GameObject("stan.obj", { "stan_tex.png", "stan_emit.png", "black.png" });
	goStan = GameObject("stan.obj", "stan.material");
	goShip = GameObject("ship/SpaceShip.obj", "ship.material");
	goSun = GameObject(&meshSphere, "sun.material");
	goEarth = GameObject(&meshSphere, "earth.material");
	goPlane = GameObject(&meshPlane, "floor.material");
	goSaturn = GameObject(&meshSphere, "saturn.material");
	goSaturnRings = GameObject(&meshPlane, "saturnRing.material");
	goTV = GameObject(&meshPlane, "tv.material");
	goTV.material->addTexture(&framebufferTV._Color._Tex[0], pbrEmissive);

	std::vector<std::string> skyboxTex;
	skyboxTex.push_back("sky2/sky_c00.bmp");
	skyboxTex.push_back("sky2/sky_c01.bmp");
	skyboxTex.push_back("sky2/sky_c02.bmp");
	skyboxTex.push_back("sky2/sky_c03.bmp");
	skyboxTex.push_back("sky2/sky_c04.bmp");
	skyboxTex.push_back("sky2/sky_c05.bmp");
	skyCubeTexture = new TextureCube(skyboxTex);
	goSkybox = GameObject(&meshSkybox, skyCubeTexture);
	goSkybox.setShaderProgram(&shaderSky);
	camera2.m_pSkybox = camera.m_pSkybox = &goSkybox;

	rm::addEntity(&goStan);
	rm::addEntity(&goShip);	
	rm::addEntity(&goSun);
	rm::addEntity(&goEarth);
	rm::addEntity(&goPlane);
	rm::addEntity(&goSaturn);
	rm::addEntity(&goSaturnRings);
	rm::addEntity(&camera);
	rm::addEntity(&camera2);
	rm::addEntity(&goTV);	

	goSun.addChild(&light);
	
	goTV.setLocalPos(vec3(0, 1, 0));
	goStan.setLocalPos(vec3(2, 0, -10));
	goShip.setLocalPos(vec3(-4, 4, -8));
	goSun.setLocalPos(vec3(4, 7, 0));
	goEarth.setLocalPos(vec3(-2, 2, 0));
	goPlane.setLocalPos(vec3(0, 0, -5));
	goSaturn.setLocalPos(vec3(-2, 2, -3));
	goSaturnRings.setLocalPos(vec3(-2, 2, -3));

	//std::uniform_real_distribution<float> randomPositionX(-100.0f, 100.0f);
	//std::uniform_real_distribution<float> randomPositionY(-100.0f, 100.0f);
	//std::uniform_real_distribution<float> randomPositionZ(-100.0f, 100.0f);
	//std::uniform_real_distribution<float> randomRotation(0.0f, 360.0f);
	//std::uniform_real_distribution<float> randomScale(0.5f, 4.0f);
	//std::default_random_engine generator;// (std::_Random_device());
	//
	//for (int i = 0; i < 1000; i++)
	//{
	//	GameObject *object = new GameObject(&meshSphere, texMoon);
	//	object->setLocalPos(vec3(randomPositionX(generator), randomPositionY(generator), randomPositionZ(generator)));
	//	object->setScale(vec3(randomScale(generator)));
	//	object->setLocalRot(vec3(randomRotation(generator), randomRotation(generator), randomRotation(generator)));
	//	object->setShaderProgram(&shaderGbuffer);
	//	ResourceManager::addEntity(object);
	//	goPlanets.push_back(object);
	//}

	for (int i = 0; i < 11; i++)
	{
		for (int k = 0; k < 11; k++)
		{
			GameObject *object = new GameObject(&meshSphere, "sphereExample.material");
			object->material = new Material(*object->material);
			object->material->floatUniforms["uRoughness"] = i * 0.1f;
			object->material->floatUniforms["uMetallic"] = k * 0.1f;
			object->setLocalPos(vec3(i * 4.f - 20.f, k * 4.f, -30.f));
			object->setScale(vec3(1.5f));
			object->setLocalRot(vec3(0.0f, 0.0f, 0.0f));
			ResourceManager::addEntity(object);
			goPlanets.push_back(object);
		}
	}

	goStan.setScale(10.0f);
	goSun.setScale(1.50f);
	goEarth.setScale(0.50f);
	goPlane.setScale(20.0f);
	goSaturn.setScale(1.0f);
	goSaturnRings.setScale(2.0f);
	goSaturnRings.setLocalRotZ(-20.0f);
		   	 
	// These Render flags can be set once at the start (No reason to waste time calling these functions every frame).
	// Tells OpenGL to respect the depth of the scene. Fragments will not render when they are behind other geometry.
	glEnable(GL_DEPTH_TEST); 
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	   
	// Setup our main scene objects...
	float aspect = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);
	camera.perspective(90.0f, aspect, 0.1f, 1000.0f);
	camera.setLocalPos(vec3(0.0f, 4.0f, 4.0f));
	camera.setLocalRotX(-15.0f);
	camera.attachFrameBuffer(&gbuffer);
	camera.m_pClearFlag = ClearFlag::SolidColor;
	camera.setRenderList(ResourceManager::Transforms);

	camera2.perspective(60.0f, 1.0f, 0.1f, 60.0f);
	camera2.setLocalRotX(-15.0f);
	camera2.setLocalRotY(180.0f);
	camera2.attachFrameBuffer(&framebufferTV);
	camera2.m_pClearFlag = ClearFlag::SolidColor;
	camera2.setRenderList(ResourceManager::Transforms);
	
	// The camera is parented to the light, so that it can follow the light's movements
	lightShadow.setLocalRot(vec3(-30, 45, 0));

	lightKey .setLocalRot(vec3(-10, -70, 0));
	lightFill.setLocalRot(vec3(-10, 20, 0));
	lightRim .setLocalRot(vec3(-10, 140, 0));
}

void Game::update()
{
	// update our clock so we have the delta time since the last update
	updateTimer->tick();

	float deltaTime = updateTimer->getElapsedTimeSeconds();
	TotalGameTime += deltaTime;
	uniformBufferTime.sendFloat(TotalGameTime, 0);

	// Gather multiple samples of framerate
	frameTimeSamples[frameTimeCurrSample] = min(deltaTime, 0.1f);
	frameTimeCurrSample = (frameTimeCurrSample + 1) % frameTimeNumSamples;
	frameTimeSamples[frameTimeCurrSample] = 1.0f;

#pragma region movementCode
	float cameraSpeedMult = 4.0f;
	float cameraRotateSpeed = 90.0f;
	float cameraMouseSpeed = 0.15f;

	if (input.shiftL || input.shiftR)
	{
		cameraSpeedMult *= 4.0f;
	}

	if (input.ctrlL || input.ctrlR)
	{
		cameraSpeedMult *= 0.5f;
	}

	if (input.moveUp)
	{
		camera.m_pLocalPosition += camera.m_pLocalRotation.up() * cameraSpeedMult * deltaTime;
	}
	if (input.moveDown)
	{
		camera.m_pLocalPosition -= camera.m_pLocalRotation.up() * cameraSpeedMult * deltaTime;
	}
	if (input.moveForward)
	{
		camera.m_pLocalPosition -= camera.m_pLocalRotation.forward() * cameraSpeedMult * deltaTime;
	}
	if (input.moveBackward)
	{
		camera.m_pLocalPosition += camera.m_pLocalRotation.forward() * cameraSpeedMult * deltaTime;
	}
	if (input.moveRight)
	{
		camera.m_pLocalPosition += camera.m_pLocalRotation.right() *  cameraSpeedMult * deltaTime;
	}
	if (input.moveLeft)
	{
		camera.m_pLocalPosition -= camera.m_pLocalRotation.right() * cameraSpeedMult * deltaTime;
	}
	if (input.rotateUp)
	{
		camera.m_pLocalRotationEuler.x += cameraRotateSpeed * deltaTime;
		camera.m_pLocalRotationEuler.x = min(camera.m_pLocalRotationEuler.x, 85.0f);
	}
	if (input.rotateDown)
	{
		camera.m_pLocalRotationEuler.x -= cameraRotateSpeed * deltaTime;
		camera.m_pLocalRotationEuler.x = max(camera.m_pLocalRotationEuler.x, -85.0f);
	}
	if (input.rotateRight)
	{
		camera.m_pLocalRotationEuler.y -= cameraRotateSpeed * deltaTime;
	}
	if (input.rotateLeft)
	{
		camera.m_pLocalRotationEuler.y += cameraRotateSpeed * deltaTime;
	}
	if (!guiEnabled)
	{
		camera.m_pLocalRotationEuler.x -= cameraMouseSpeed * input.mouseMovement.y;
		camera.m_pLocalRotationEuler.y -= cameraMouseSpeed * input.mouseMovement.x;
		camera.m_pLocalRotationEuler.x = clamp(camera.m_pLocalRotationEuler.x, -85.0f, 85.0f);
		input.mouseMovement = vec2(0.0f);
	}
#pragma endregion movementCode

	// Make the deferred lights dance around
	for (int i = 0; i < (int)lights.size(); ++i)
	{
		lights[i].m_pLocalPosition = vec3(
			sin(i + TotalGameTime) * 0.1f + ((i / 4) % 4),
			sin(TotalGameTime) * 0.1f + (i / 16),
			cos(i + TotalGameTime) * 0.1f + (i % 4)) * vec3(60,40,-20) - vec3(30,25,0);
	}
	
	lightShadow.update(deltaTime);
	lightKey.update(deltaTime);
	lightFill.update(deltaTime);
	lightRim.update(deltaTime);

	// Make the camera move around
	camera2.setLocalPos(vec3(sin(TotalGameTime) * 10.0f, sin(TotalGameTime * 4) + 4.0f, -50.0f));

	// Give the earth some rotation over time.
	goEarth.setLocalRotY(TotalGameTime * 15.0f);

	// Give our Transforms a chance to compute the latest matrices
	for (Transform* object : ResourceManager::Transforms)
	{
		object->update(deltaTime);
	}
	goSkybox.update(deltaTime);
}

void Game::draw()
{
	glClear(GL_DEPTH_BUFFER_BIT); // Clear the backbuffer

	textureToonRamp[0]->bind(31);
	
	// Render the scene from both camera's perspective
	// this will render all objects that are in the camera list
	camera2.render();	
	camera.render();	
	
	drawLights();
	// After drawing lights, begin post processing
	drawPostProcessing();
	

#if DEBUG_LOG
	if(guiEnabled)
		GUI();	
#endif

	// Commit the Back-Buffer to swap with the Front-Buffer and be displayed on the monitor.
	glutSwapBuffers();
}

void Game::drawLights()
{
	// Clear the post processing buffer
	framebufferDeferredLight.clear();

	glDisable(GL_DEPTH_TEST); // Disable depth so we don't read/write from/to the depth
	glBlendFunc(GL_ONE, GL_ONE); // Enable additive blending for the lights
	glEnable(GL_BLEND);

	shaderDeferredPointLight.bind();
	gbuffer.bindDepthAsTexture(0);
	gbuffer.bindColorAsTexture(0, 1);
	gbuffer.bindColorAsTexture(1, 2);
	gbuffer.bindColorAsTexture(2, 3);
	gbuffer.bindResolution();

	light.bind();
	light.position = camera.getView() * vec4(light.getWorldPos(), 1.0f);
	light.sendToGPU();
	framebufferDeferredLight.renderToFSQ();

	//for (int i = 0; i < (int)lights.size(); ++i)
	//{
	//	lights[i].bind();
	//	lights[i].position = camera.getView() * vec4(lights[i].m_pLocalPosition, 1.0f);
	//	lights[i].sendToGPU();
	//	framebufferDeferredLight.renderToFSQ();
	//}

	////////////////////////
   // START SHADOWS HERE //
  ////////////////////////
	shaderDeferredDirectionalLight.bind();

	// Don't forget to convert it into viewspace!
	lightShadow.bind();
	lightShadow.direction.xyz = normalize((camera.getView() * lightShadow.getLocalToWorld()).forward());
	lightShadow.sendToGPU();
	// Draw the directional light!
	framebufferDeferredLight.renderToFSQ();
	shaderDeferredDirectionalLight.unbind();
	/////////////////////////
   // FINISH SHADOWS HERE //
  /////////////////////////
	
	shaderDeferredDirectionalLight.bind();
	lightKey.bind();
	lightKey.direction.xyz = normalize((camera.getView() * lightKey.getLocalToWorld()).forward());
	lightKey.sendToGPU();
	// Draw the directional light!
	framebufferDeferredLight.renderToFSQ();
	lightFill.bind();
	lightFill.direction.xyz = normalize((camera.getView() * lightFill.getLocalToWorld()).forward());
	lightFill.sendToGPU();
	// Draw the directional light!
	framebufferDeferredLight.renderToFSQ();
	lightRim.bind();
	lightRim.direction.xyz = normalize((camera.getView() * lightRim.getLocalToWorld()).forward());
	lightRim.sendToGPU();
	// Draw the directional light!
	framebufferDeferredLight.renderToFSQ();
	shaderDeferredDirectionalLight.unbind();

  // Add on the emissive buffer
	shaderDeferredEmissive.bind();
	skyCubeTexture->bind(6);
	framebufferDeferredLight.renderToFSQ();
	skyCubeTexture->unbind(6);

	// Unbind gbuffer textures
	gbuffer.unbindTexture(5);
	gbuffer.unbindTexture(4);
	gbuffer.unbindTexture(3);
	gbuffer.unbindTexture(2);
	gbuffer.unbindTexture(1);
	gbuffer.unbindTexture(0);
	   
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // normal blend function
	glDisable(GL_BLEND); // Disable blending so everything else can draw
}

void Game::drawPostProcessing()
{
	shaderPassthrough.bind();
	framebufferDeferredLight.bindColorAsTexture(0, 0);
	ppBuffer.drawToPost();
	shaderPassthrough.unbind();

#pragma region GrayscalePost
	if (guiGrayscaleActive)
	{
		shaderGrayscale.bind();
		shaderGrayscale.sendUniform("uAmount", guiGrayscaleAmount);
		ppBuffer.draw();
		shaderGrayscale.unbind();
	}
#pragma endregion GrayscalePost
	
#pragma region depthOfFieldPost
	
#pragma endregion depthOfFieldPost


#pragma region wavePost
	if (guiWaveActive)
	{
		shaderPostWave.bind();
		shaderPostWave.sendUniform("uWaveCount", guiWaveCount);
		shaderPostWave.sendUniform("uWaveIntensity", guiWaveIntensity);
		shaderPostWave.sendUniform("uWaveSpeed", guiWaveSpeed);
		ppBuffer.draw();
		shaderPostWave.unbind();
	}
#pragma endregion wavePost

#pragma region RGBSplitPost
	if (guiRGBActive)
	{
		shaderPostSplitRGB.bind();
		shaderPostSplitRGB.sendUniform("uRandom", random(0.0f, 1.0f));
		shaderPostSplitRGB.sendUniform("uRange", guiRGBRange);
		shaderPostSplitRGB.sendUniform("rOffset", guiRGBrOffset);
		shaderPostSplitRGB.sendUniform("gOffset", guiRGBgOffset);
		shaderPostSplitRGB.sendUniform("bOffset", guiRGBbOffset);
		ppBuffer.draw();
		shaderPostSplitRGB.unbind();
	}
	
#pragma endregion RGBSplitPost

#pragma region ChromAbPost
	if (guiChromAberActive)
	{
		shaderPostChromAber.bind(); 
		shaderPostChromAber.sendUniform("uDispersal", guiChromAberExponent);
		shaderPostChromAber.sendUniform("rOffset", guiChromAberOffset.r);
		shaderPostChromAber.sendUniform("gOffset", guiChromAberOffset.g);
		shaderPostChromAber.sendUniform("bOffset", guiChromAberOffset.b);
		ppBuffer.draw();
		shaderPostChromAber.unbind();
	}
#pragma endregion ChromAbPost

#pragma region gammaPost

	if (guiGammaActive)
	{
		shaderPostGamma.bind();
		shaderPostGamma.sendUniform("uGamma", guiGammaAmount);
		ppBuffer.draw();
		shaderPostGamma.unbind();
	}

#pragma endregion gammaPost

	shaderPassthrough.bind();
	framebufferDeferredLight.unbind();
	ppBuffer.drawToScreen();
	shaderPassthrough.unbind();
	
	if (guiViewGBufferOverride)
	{
		shaderPassthrough.bind();
		gbuffer.bindColorAsTexture(guiViewGBuffer, 0);
		Framebuffer::drawFSQ();
		shaderPassthrough.unbind();
	}

	glEnable(GL_DEPTH_TEST);
}

void Game::GUI()
{
	UI::Start(windowWidth, windowHeight);
	
	// Framerate Visualizer
	ImGui::Begin("Framerate");
	{
		float averageFramerate = 0.0f;
		for (int i = 0; i < frameTimeNumSamples; ++i)
			averageFramerate += frameTimeSamples[i];
		averageFramerate = 1.0f / ((averageFramerate-1.0f) / (frameTimeNumSamples-1));
		std::string framerate = "Framerate: " + std::to_string(averageFramerate);

		ImGui::PlotHistogram("", frameTimeSamples, frameTimeNumSamples, 0, framerate.c_str(), 0.005f, 0.2f, ImVec2(frameTimeNumSamples, 32));
	}
	ImGui::End();

	ImGui::Checkbox("Draw Camera Volumes", &Camera::_cameraWireframe);

	// TODO: initialize shadow camera controls

	if (ImGui::TreeNode("Post Processing"))
	{
		ImGui::Text("Toggle All");
		ImGui::SameLine();
		if (ImGui::Button("On"))
		{
			guiGrayscaleActive = true;
			guiWaveActive = true;
			guiChromAberActive = true;
			guiRGBActive = true;			
		}
		ImGui::SameLine();
		if (ImGui::Button("Off"))
		{
			guiGrayscaleActive = false;
			guiWaveActive = false;
			guiChromAberActive = false;
			guiRGBActive = false;
		}

		if (ImGui::TreeNode("Grayscale"))
		{
			ImGui::Checkbox("Active", &guiGrayscaleActive);
			ImGui::SliderFloat("Grayscale Amount", &guiGrayscaleAmount, 0.0f, 1.0f);
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Wave"))
		{
			ImGui::Checkbox("Active", &guiWaveActive);
			ImGui::SliderFloat2("Count", &guiWaveCount.x, 0.001f, 100.0f, "%.4f", f_e);
			ImGui::SliderFloat2("Intensity", &guiWaveIntensity.x, 0.0f, 1.0f, "%.4f", f_e);
			ImGui::SliderFloat2("Speed", &guiWaveSpeed.x, 0.0f, 50.0f, "%.4f", f_e);
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Chromatic Aberration"))
		{
			ImGui::Checkbox("Active", &guiChromAberActive);
			ImGui::SliderFloat("Exponent", &guiChromAberExponent, 1.0f, 10.0f, "%.4f", f_e);
			
			ImGui::SliderFloat("rOffset", &guiChromAberOffset.r, -0.1f, 0.1f, "%.4f", f_e);
			ImGui::SliderFloat("gOffset", &guiChromAberOffset.g, -0.1f, 0.1f, "%.4f", f_e);
			ImGui::SliderFloat("bOffset", &guiChromAberOffset.b, -0.1f, 0.1f, "%.4f", f_e);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("RGB Split"))
		{
			ImGui::Checkbox("Active", &guiRGBActive);
			ImGui::SliderFloat("Range", &guiRGBRange, 0.0f, 1.0f);
			
			ImGui::SliderFloat2("rOffset", &guiRGBrOffset.x, -0.1f, 0.1f, "%.4f", f_e);
			ImGui::SliderFloat2("gOffset", &guiRGBgOffset.x, -0.1f, 0.1f, "%.4f", f_e);
			ImGui::SliderFloat2("bOffset", &guiRGBbOffset.x, -0.1f, 0.1f, "%.4f", f_e);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Gamma"))
		{
			ImGui::Checkbox("Active", &guiGammaActive);
			ImGui::SliderFloat("Gamma Amount", &guiGammaAmount, 0.5f, 4.2f, "%.4f", 1.25f);
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}
	
	if (ImGui::TreeNode("Toon Shading"))
	{
		if (ImGui::Checkbox("Toon Shading Active", &toonActive))
		{
			uniformBufferToon.sendUInt(toonActive, 0);
		}
		ImGui::SliderInt("Ramp Selection", &activeToonRamp, 0, (int)textureToonRamp.size() - 1);

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Point Light"))
	{
		ImGui::DragFloat3("Light Position", &goSun.m_pLocalPosition.x, 0.5f);
		ImGui::SliderFloat3("Color", &light.color.x, 0.0f, 10.0f, "%.4f", f_e);
		ImGui::SliderFloat3("Attenuation", &light.constantAtten, 0.0f, 1.0f, "%.8f", 6.0f);
		ImGui::Text("Radius: %f", light.radius);

		ImGui::TreePop();
	}
	
	if (ImGui::TreeNode("Directional Light"))
	{
		ImGui::DragFloat3("Rotation", &lightShadow.m_pLocalRotationEuler.x, 0.5f);
		ImGui::SliderFloat3("Color", &lightShadow.color.x, 0.0f, 10.0f, "%.4f", f_e);

		ImGui::TreePop();
	}	

	if (ImGui::TreeNode("Ship"))
	{
		ImGui::DragFloat3("Position", &goShip.m_pLocalPosition.x, 0.05f);
		ImGui::DragFloat3("Rotation", &goShip.m_pLocalRotationEuler.x, 0.5f);

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Framebuffer Override"))
	{
		ImGui::Checkbox("View Override", &guiViewGBufferOverride);
		//ImGui::SliderInt("View Buffer", &guiViewFramebuffer, 0, 5);
		const char* buffers[] = { "Albedo", "Normal", "Emissive", "Roughness", "Metallic" };
		if (ImGui::ListBox("View Buffer", &guiViewGBuffer, buffers, sizeof(buffers) / sizeof(*buffers)))
		{

		}
		
		ImGui::TreePop();
	}


	UI::End();
}

void Game::keyboardDown(unsigned char key, int mouseX, int mouseY)
{
	if (guiEnabled)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.KeysDown[(int)key] = true;
		io.AddInputCharacter((int)key); // this is what makes keyboard input work in imgui
		// This is what makes the backspace button work
		int keyModifier = glutGetModifiers();
		switch (keyModifier)
		{
		case GLUT_ACTIVE_SHIFT:
			io.KeyShift = true;
			break;

		case GLUT_ACTIVE_CTRL:
			io.KeyCtrl = true;
			break;

		case GLUT_ACTIVE_ALT:
			io.KeyAlt = true;
			break;
		}
	}

	switch(key)
	{
	case 27: // the escape key
		break;
	case 'w':
	case 'W':
	case 'w' - 96:
		input.moveForward = true;
		break;
	case 's':
	case 'S':
	case 's' - 96:
		input.moveBackward = true;
		break;
	case 'd':
	case 'D':
	case 'd' - 96:
		input.moveRight = true;
		break;
	case 'a':
	case 'A':
	case 'a' - 96:
		input.moveLeft = true;
		break;
	case 'e':
	case 'E':
	case 'e' - 96:
		input.moveUp = true;
		break;
	case 'q':
	case 'Q':
	case 'q' - 96:
		input.moveDown = true;
		break;
	case 'l':
	case 'L':
	case 'l' - 96:
		input.rotateRight = true;
		break;
	case 'j':
	case 'J':
	case 'j' - 96:
		input.rotateLeft = true;
		break;
	case 'i':
	case 'I':
	case 'i' - 96:
		input.rotateUp = true;
		break;
	case 'k':
	case 'K':
	case 'k' - 96:
		input.rotateDown = true;
		break;
	}
}

void Game::keyboardUp(unsigned char key, int mouseX, int mouseY)
{
	if (guiEnabled)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.KeysDown[key] = false;

		int keyModifier = glutGetModifiers();
		io.KeyShift = false;
		io.KeyCtrl = false;
		io.KeyAlt = false;
		switch (keyModifier)
		{
		case GLUT_ACTIVE_SHIFT:
			io.KeyShift = true;
			break;

		case GLUT_ACTIVE_CTRL:
			io.KeyCtrl = true;
			break;

		case GLUT_ACTIVE_ALT:
			io.KeyAlt = true;
			break;
		}
	}

	switch(key)
	{
	case 32: // the space bar
		camera.cullingActive = !camera.cullingActive;
		break;
	case 27: // the escape key
		exit(1);
		break;
	case 'w':
	case 'W':
	case 'w' - 96:
		input.moveForward = false;
		break;
	case 's':
	case 'S':
	case 's' - 96:
		input.moveBackward = false;
		break;
	case 'd':
	case 'D':
	case 'd' - 96:
		input.moveRight = false;
		break;
	case 'a':
	case 'A':
	case 'a' - 96:
		input.moveLeft = false;
		break;
	case 'e':
	case 'E':
	case 'e' - 96:
		input.moveUp = false;
		break;
	case 'q':
	case 'Q':
	case 'q' - 96:
		input.moveDown = false;
		break;
	case 'l':
	case 'L':
	case 'l' - 96:
		input.rotateRight = false;
		break;
	case 'j':
	case 'J':
	case 'j' - 96:
		input.rotateLeft = false;
		break;
	case 'i':
	case 'I':
	case 'i' - 96:
		input.rotateUp = false;
		break;
	case 'k':
	case 'K':
	case 'k' - 96:
		input.rotateDown = false;
		break;
	}
}

void Game::keyboardSpecialDown(int key, int mouseX, int mouseY)
{
	switch (key)
	{
	case GLUT_KEY_F1:
		guiEnabled = !guiEnabled;
		if (guiEnabled)
		{
			glutWarpPointer((int)input.mousePosGUI.x, (int)input.mousePosGUI.y);
			glutSetCursor(GLUT_CURSOR_INHERIT);
		}
		else 
		{
			input.f11 = true;
			glutWarpPointer(windowWidth / 2, windowHeight / 2);
			glutSetCursor(GLUT_CURSOR_NONE);
		}
		if (!UI::isInit)
		{
			UI::InitImGUI();
		}
		break;
	case GLUT_KEY_F5:
		for (ShaderProgram* shader : ResourceManager::Shaders)
		{
			shader->reload();
		}
		break;
	case GLUT_KEY_CTRL_L:
		input.ctrlL = true;
		break;
	case GLUT_KEY_CTRL_R:
		input.ctrlR = true;
		break;
	case GLUT_KEY_SHIFT_L:
		input.shiftL = true;
		break;
	case GLUT_KEY_SHIFT_R:
		input.shiftR = true;
		break;
	case GLUT_KEY_ALT_L:
		input.altL = true;
		break;
	case GLUT_KEY_ALT_R:
		input.altR = true;
		break;
	case GLUT_KEY_UP:
		input.moveForward = true;
		break;
	case GLUT_KEY_DOWN:
		input.moveBackward = true;
		break;
	case GLUT_KEY_RIGHT:
		input.moveRight = true;
		break;
	case GLUT_KEY_LEFT:
		input.moveLeft = true;
		break;
	case GLUT_KEY_PAGE_UP:
		input.moveUp = true;
		break;
	case GLUT_KEY_PAGE_DOWN:
		input.moveDown = true;
		break;
	case GLUT_KEY_END:
		exit(1);
		break;
	}
}

void Game::keyboardSpecialUp(int key, int mouseX, int mouseY)
{
	switch (key)
	{
	case GLUT_KEY_CTRL_L:
		input.ctrlL = false;
		break;
	case GLUT_KEY_CTRL_R:
		input.ctrlR = false;
		break;
	case GLUT_KEY_SHIFT_L:
		input.shiftL = false;
		break;
	case GLUT_KEY_SHIFT_R:
		input.shiftR = false;
		break;
	case GLUT_KEY_ALT_L:
		input.altL = false;
		break;
	case GLUT_KEY_ALT_R:
		input.altR = false;
		break;
	case GLUT_KEY_UP:
		input.moveForward = false;
		break;
	case GLUT_KEY_DOWN:
		input.moveBackward = false;
		break;
	case GLUT_KEY_RIGHT:
		input.moveRight = false;
		break;
	case GLUT_KEY_LEFT:
		input.moveLeft = false;
		break;
	case GLUT_KEY_PAGE_UP:
		input.moveUp = false;
		break;
	case GLUT_KEY_PAGE_DOWN:
		input.moveDown = false;
		break;
	}
}

void Game::mouseClicked(int button, int state, int x, int y)
{
	if (guiEnabled)
	{
		ImGui::GetIO().MousePos = ImVec2((float)x, (float)y);
		ImGui::GetIO().MouseDown[0] = !state;
		if (state == GLUT_DOWN)
		{
			if (button == 3)
				ImGui::GetIO().MouseWheel += 1.0f;
			else if (button == 4)
				ImGui::GetIO().MouseWheel -= 1.0f;

		}
	}
	else
	{
	}

	if(state == GLUT_DOWN) 
	{
		switch(button)
		{
		case GLUT_LEFT_BUTTON:

			break;
		case GLUT_RIGHT_BUTTON:
		
			break;
		case GLUT_MIDDLE_BUTTON:

			break;
		}
	}
	else
	{

	}
}

void Game::mouseMoved(int x, int y)
{
	if (guiEnabled)
	{
		ImGui::GetIO().MousePos = ImVec2((float)x, (float)y);

		if (!ImGui::GetIO().WantCaptureMouse)
		{

		}
	}

	
}

void Game::mousePassive(int x, int y)
{
	if (guiEnabled)
	{
		ImGui::GetIO().MousePos = ImVec2((float)x, (float)y);
		input.mousePosGUI = vec2((float)x, (float)y);
	}
	else
	{
		if (input.f11)
		{
			//glutWarpPointer(windowWidth / 2, windowHeight / 2);
			input.f11 = false;
			input.mousePos = vec2((float)(windowWidth / 2), (float)(windowHeight / 2));
			input.mousePosOld = input.mousePos;
		}
		else
		{
			//input.mousePosOld = vec2((float)(windowWidth / 2), (float)(windowHeight / 2));
			input.mousePos = vec2((float)x, (float)y);
			input.mouseMovement += input.mousePos - input.mousePosOld;
			input.mousePosOld = input.mousePos;
		}

		if (x < 100 || y < 100 || x > windowWidth - 100 || y > windowHeight - 100)
		{
			glutWarpPointer(windowWidth / 2, windowHeight / 2);
			input.mousePosOld = vec2((float)(windowWidth / 2), (float)(windowHeight / 2));
			input.f11 = true;
		}
	}
}

void Game::reshapeWindow(int w, int h)
{
	windowWidth = w;
	windowHeight = h;

	float aspect = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);
	camera.perspective(90.0f, aspect, 0.05f, 1000.0f);
	glViewport(0, 0, w, h);
	framebufferDeferredLight.reshape(w, h);
	gbuffer.reshape(w, h);
	gbuffer.setDebugNames();
	ppBuffer.reshape(w, h);
}
