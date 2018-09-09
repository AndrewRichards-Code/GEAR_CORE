#include "src/graphics/window.h"
#include "src/graphics/renderer/renderer.h"
#include "src/graphics/renderer/batchrenderer2d.h"
#include "src/graphics/texture.h"
#include "src/maths/ARMLib.h"
#include "src/graphics/camera.h"
#include "src/graphics/light.h"
#include "src/graphics/font.h"

#include "AL/al.h"


#if _DEBUG
#include "src/graphics/debugopengl.h"
#endif

int main()
{
	using namespace GEAR;
	using namespace GRAPHICS;
	using namespace ARM;
	Window window("GEAR", 1280, 720);
	//window.Fullscreen();

#if _DEBUG
	DebugOpenGL debug;
#endif

	Shader shader("res/shaders/basic.vert", "res/shaders/basic.frag");
	shader.SetLighting(GEAR_CALC_LIGHT_DIFFUSE + GEAR_CALC_LIGHT_SPECULAR + GEAR_CALC_LIGHT_AMIBIENT);

	Texture texture("res/img/stallTexture.png");
	Object stall("res/obj/stall.obj", shader, texture, Mat4::Translation(Vec3(5.0f, -2.0f, -5.0f)) * Mat4::Rotation(pi, Vec3(0, 1, 0)));

	Texture texture2("res/gear_core/GEAR_logo_square.png");
	Texture texture3("res/img/andrew_manga_3_square.png");
	Object quad1("res/obj/quad.obj", shader, texture2, Mat4::Translation(Vec3( 1.5f, 0.0f, -2.0f))); 
	Object quad2("res/obj/quad.obj", shader, texture2, Mat4::Translation(Vec3(-1.5f, 0.0f, -2.0f)));
	Object floor("res/obj/quad.obj", shader, texture3, Mat4::Translation(Vec3(0.0f, -2.0f, -2.0f)) * Mat4::Rotation(pi / 2, Vec3(1, 0, 0)) * Mat4::Scale(Vec3(10, 10, 1)));
	
	//For BatchRender2D
	Object quad3("res/obj/quad.obj", shader, texture, Vec3(0, 0, -2), Vec2(0.5, 0.5));
	Object quad4("res/obj/quad.obj", shader, texture2, Vec3(-1.5, 0, -2), Vec2(0.5, 0.5));
	Object quad5("res/obj/quad.obj", shader, texture3, Vec3(1.5, 0, -2), Vec2(0.5, 0.5));


	Camera cam_main(GEAR_CAMERA_PERSPECTIVE, shader, Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(0.0f, 1.0f, 0.0f));
	cam_main.DefineProjection((float)DegToRad(90), window.GetRatio(), 0.5f, 50.0f);

	Light light_main(GEAR_LIGHT_POINT, Vec3(0.0f, 0.0f, 0.0f), Vec4(1.0f, 1.0f, 1.0f, 1.0f), shader);
	light_main.Specular(32.0f, 1.0f);
	light_main.Ambient(0.5f);

	BatchRenderer2D br2d;
	Renderer renderer;

	Font testFont("This is a test!", "res/font/consola/consola.ttf", 40, Vec2(-0.75f, -0.75f), Vec4(1.0f, 1.0f, 1.0f, 1.0f), window);

	float x = 0, y = 0, z = 0;
	double yaw = 0;
	double pitch = 0;
	double roll = 0;

	double pos_x = 0, pos_y = 0;
	double last_pos_x = window.GetWidth() / 2.0;
	double last_pos_y = window.GetHeight() / 2.0;
	bool initMouse = true;

	float increment = 0.05f;

	//Logo Splashscreen
	{
		Shader logo_shader("res/shaders/basic.vert", "res/shaders/basic.frag");
		logo_shader.SetLighting(GEAR_CALC_LIGHT_DIFFUSE);

		Texture logo_texture("res/gear_core/GEAR_logo_square.png");
		Object logo("res/obj/quad.obj", logo_shader, logo_texture, Mat4::Translation(Vec3(0.0f, 0.0f, 0.0f)));

		Camera logo_cam(GEAR_CAMERA_ORTHOGRAPHIC, logo_shader, Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(0.0f, 1.0f, 0.0f));
		Light logo_light(GEAR_LIGHT_POINT, Vec3(0.0f, 0.0f, 5.0f), Vec4(1.0f, 1.0f, 1.0f, 1.0f), logo_shader);

		int time = 0;
		float increment = 0.017f;
		float r = 0.0f, g = 0.0f, b = 0.0f;
		while (time < 300)
		{
			window.Clear();
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			logo_shader.Enable();
			if (time < 60)
			{
				r += increment; g += increment; b += increment;
				logo_shader.SetUniform<float>("u_LightColour", { r, g, b, 1.0f });
			}
			else if (time > 240)
			{
				r -= increment; g -= increment; b -= increment;
				logo_shader.SetUniform<float>("u_LightColour", { r, g, b, 1.0f });
			}
			else
				logo_shader.SetUniform<float>("u_LightColour", { 1.0f, 1.0f, 1.0f, 1.0f });

			logo_cam.DefineProjection(-window.GetRatio(), window.GetRatio(), -1.0f, 1.0f, -1.0f, 1.0f);

			renderer.Draw(&logo);
			window.Update();
			if (window.IsKeyPressed(GLFW_KEY_ENTER)) break;
			time++;
		}
	}

	while (!window.Closed())
	{
		window.Clear();
		shader.Enable();
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

		if (window.IsKeyPressed(GLFW_KEY_D))
			cam_main.m_Position = cam_main.m_Position - Vec3::Normalise(Vec3::Cross(cam_main.m_Up, cam_main.m_Forward)) *  increment;
		if (window.IsKeyPressed(GLFW_KEY_A))
			cam_main.m_Position = cam_main.m_Position + Vec3::Normalise(Vec3::Cross(cam_main.m_Up, cam_main.m_Forward)) *  increment;
		if (window.IsKeyPressed(GLFW_KEY_S))
			cam_main.m_Position = cam_main.m_Position - cam_main.m_Forward *  increment;
		if (window.IsKeyPressed(GLFW_KEY_W))
			cam_main.m_Position = cam_main.m_Position + cam_main.m_Forward *  increment;

		cam_main.m_Position.y = 0.0f;
		window.GetMousePosition(pos_x, pos_y);

		if (initMouse)
		{
			last_pos_x = pos_x;
			last_pos_y = pos_y;
			initMouse = false;
		}

		double offset_pos_x = pos_x - last_pos_x;
		double offset_pos_y = -pos_y + last_pos_y;
		last_pos_x = pos_x;
		last_pos_y = pos_y;
		offset_pos_x *= increment * increment;
		offset_pos_y *= increment * increment;
		yaw += 2*offset_pos_x;
		pitch += offset_pos_y;
		if (pitch > pi / 2)
			pitch = pi / 2;
		if (pitch < -pi / 2)
			pitch = -pi / 2;


		cam_main.UpdateCameraPosition();
		cam_main.CalcuateLookAround(yaw, pitch, roll);
		cam_main.m_Position.y = 0.0f;
		cam_main.DefineView();
		cam_main.DefineProjection(DegToRad(90), window.GetRatio(), 0.5f, 1000.0f);

		//testFont.RenderText();

		stall.SetUniformModlMatrix(Mat4::Translation(Vec3(5.0f, -2.0f, -5.0f)) * Mat4::Rotation(pi, Vec3(0, 1, 0)));
		renderer.Submit(&stall);
		renderer.Flush();
		
		stall.SetUniformModlMatrix(Mat4::Translation(Vec3(-5.0f, -2.0f, -5.0f)) * Mat4::Rotation(pi, Vec3(0, 1, 0)));
		renderer.Submit(&stall);
		
		renderer.Submit(&floor);
		renderer.Submit(&quad1);
		renderer.Submit(&quad2);
		renderer.Flush();
		
		br2d.OpenMapBuffer();
		br2d.Submit(&quad3);
		br2d.Submit(&quad4);
		br2d.Submit(&quad5);
		br2d.CloseMapBuffer();
		br2d.Flush();
		
		/*Mat4 rot = Quat::QuatToMat4(Quat(10, Vec3(1, 0, 1)));
		std::cout << rot << std::endl;*/

		window.Update();
	}
}