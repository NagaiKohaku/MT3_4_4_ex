#include <Novice.h>
#include <Novice.h>
#include <imgui.h>
#include "math/MakeMatrix.h"
#include "math/MatrixMath.h"
#include "math/Vector3Math.h"
#include "math/Draw.h"
#include "algorithm"

#include "cmath"

const char kWindowTitle[] = "LE2B_17_ナガイ_コハク_MT3_4_4_ex 平面にボールを落とす(トンネリング対策)";

struct Ball {
	Sphere sphere;
	Vector3 acceleration;
	Vector3 velocity;
	uint32_t color;
};

struct Capsule {
	Segment segment;
	float radius;
};

Vector3 Reflect(const Vector3& input, const Vector3& normal);

bool IsCollision(const Sphere& sphere, const Plane& plane);

bool IsCollision(const Capsule& capsule, const Plane& plane);

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	//ウィンドウサイズ
	float kWindowWidth = 1280.0f;
	float kWindowHeight = 720.0f;

	//カメラ:平行移動
	Vector3 cameraTranslate{ 0.0f,1.9f,-6.49f };

	//カメラ:回転
	Vector3 cameraRotate{ 0.26f,0.0f,0.0f };

	//カメラ:距離
	Vector3 baseOffset{ 0.0f,0.0f,-6.0f };

	Plane plane;
	plane.normal = Normalize({ -0.2f,0.9f,-0.3f });
	plane.distance = 0.0f;

	Ball ball;
	ball.sphere.center = { 1.2f,1.2f,0.3f };
	ball.sphere.radius = 0.05f;
	ball.acceleration = { 0.0f,-9.8f,0.0f };
	ball.velocity = { 0.0f,0.0f,0.0f };
	ball.color = WHITE;

	//反発係数
	float e = 0.8f;

	//描画クラス
	Draw draw;

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///

		//1フレームの時間
		float deltaTime = 1.0f / 60.0f;

		Capsule capsule;
		capsule.segment.origin = ball.sphere.center;
		capsule.radius = ball.sphere.radius;

		//ボールの移動処理
		ball.velocity = ball.velocity + ball.acceleration * deltaTime;
		ball.sphere.center = ball.sphere.center + ball.velocity * deltaTime;

		capsule.segment.diff = ball.sphere.center - capsule.segment.origin;

		//当たり判定をとる
		if (IsCollision(capsule, plane)) {

			ball.sphere.center = ball.sphere.center + ball.sphere.radius * plane.normal;

			//反射ベクトルの計算
			Vector3 reflected = Reflect(ball.velocity, plane.normal);

			//反射量の計算
			Vector3 projectToNormal = Project(reflected, plane.normal);

			//反射方向の計算
			Vector3 movingDirection = reflected - projectToNormal;

			//反射量にのみ反発係数を掛けて、ボールの移動量を計算する
			ball.velocity = projectToNormal * e + movingDirection;
		}

		ImGui::Text("WASD : CameraRotate");
		ImGui::Text("UpArrow : ZoomIn");
		ImGui::Text("DownArrow : ZoomDown");

		ImGui::Text("");

		//ボールの位置をリセットする
		if (ImGui::Button("Start")) {

			ball.sphere.center = { 0.8f,1.2f,0.3f };
			ball.velocity = { 0.0f,0.0f,0.0f };
		}

		ImGui::Text("");

		if (ImGui::TreeNode("Camera")) {
			ImGui::DragFloat3("Translate", &cameraTranslate.x, 0.01f);
			ImGui::DragFloat3("Rotate", &cameraRotate.x, 0.01f);
			ImGui::DragFloat3("offset", &baseOffset.x, 0.01f);
			ImGui::TreePop();
		}

		ImGui::Text("");

		draw.CamaraRotate(baseOffset, cameraRotate, cameraTranslate, keys);

		draw.Pipeline(cameraTranslate, cameraRotate, kWindowWidth, kWindowHeight);

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		draw.DrawGrid();

		draw.DrawSphere(ball.sphere, ball.color);

		draw.DrawPlane(plane, WHITE);

		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}

Vector3 Reflect(const Vector3& input, const Vector3& normal) {

	return input - 2.0f * Dot(input, normal) * normal;
}

bool IsCollision(const Sphere& sphere, const Plane& plane) {

	float distance = fabsf(Dot(plane.normal, sphere.center) - plane.distance);

	if (distance <= sphere.radius) {
		return true;
	}

	return false;
}

bool IsCollision(const Capsule& capsule, const Plane& plane) {

	//法線と線の内積を求める
	float dot = Dot(plane.normal, capsule.segment.diff);

	//垂直 = 並行であるので、衝突しているはずがない
	if (dot == 0.0f) {
		return false;
	}

	//tを求める
	float t = (plane.distance - Dot(capsule.segment.origin, plane.normal)) / dot;

	//線分なので0~1の間だったら衝突している
	if (0.0f <= t && t <= 1.0f) {

		Vector3 closestPoint = (1.0f - t) * capsule.segment.origin + t * capsule.segment.diff;

		float distance = Dot(plane.normal, closestPoint) - plane.distance;

		if (distance <= capsule.radius) {
			return true;
		}
	}

	return false;
}