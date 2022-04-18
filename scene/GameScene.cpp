#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include <random>

using namespace DirectX;

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete sprite_;
	delete model_;
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	debugText_ = DebugText::GetInstance();
	textureHandle_ = TextureManager::Load("mario.jpg");
	sprite_ = Sprite::Create(textureHandle_, {100, 50});
	model_ = Model::Create();           // 3Dモデルの生成
	std::random_device seed_gen;        // 乱数シード生成器
	std::mt19937_64 engine(seed_gen()); // メルセンヌ・ツイスター
	std::uniform_real_distribution<float> rotDist(0.0f, XM_2PI);  // 乱数範囲（回転用）
	std::uniform_real_distribution<float> posDist(-10.0f, 10.0f); // 乱数範囲（座標用）
	for (int i = 0; i < _countof(worldTransform_); i++) {
		worldTransform_[i].scale_ = {1.0f, 1.0f, 1.0f}; // スケーリング
		worldTransform_[i].rotation_ = {rotDist(engine), rotDist(engine), rotDist(engine)}; // 回転
		worldTransform_[i].translation_ = {
		  posDist(engine), posDist(engine), posDist(engine)}; // 平行移動
		worldTransform_[i].Initialize();                      // 初期化
	}
	viewProjection_.eye = {0, 0, -10};   // カメラ支店座標を設定
	viewProjection_.target = {10, 0, 0}; //カメラ注視点座標を設定
	viewProjection_.up = {cosf(XM_PI / 4.0f), sinf(XM_PI / 4.0f), 0.0f};//カメラ上方向ベクトル設定（右上45度指定）
	viewProjection_.Initialize();
	soundDataHandle_ = audio_->LoadWave("se_sad03.wav");     // サウンドデータの読み込み
	audio_->PlayWave(soundDataHandle_);                      // 音声再生
	voiceHandle_ = audio_->PlayWave(soundDataHandle_, true); // 音声再生
}

void GameScene::Update() {
	XMFLOAT2 position = sprite_->GetPosition(); // スプライトの今の座標を取得
	// position.x += 2.0f;
	// position.y += 1.0f;
	// sprite_->SetPosition(position);	// 移動した座標をスプライトに反映
	// if (input_->TriggerKey(DIK_SPACE))// スペースキーを押した瞬間
	//	audio_->StopWave(voiceHandle_); // 音声停止
	// デバッグテキストの表示
	// debugText_->Print("Kaizokuou ni oreha naru.", 50, 50, 1.0f);
	// 書式指定付き表示
	// debugText_->SetPos(50, 70);
	// debugText_->Printf(
	//  "translation:(%f,%f,%f)", worldTransform_.translation_.x, worldTransform_.translation_.y,
	//  worldTransform_.translation_.z);
	// debugText_->SetPos(50, 90);
	// debugText_->Printf(
	//  "rotation:(%f,%f,%f)", worldTransform_.rotation_.x, worldTransform_.rotation_.y,
	//  worldTransform_.rotation_.z);
	// debugText_->SetPos(50, 110);
	// debugText_->Printf(
	//  "scale:(%f,%f,%f)", worldTransform_.scale_.x, worldTransform_.scale_.y,
	//  worldTransform_.scale_.z);

	// std::string strDebug = std::string("value:") + std::to_string(value_);	// 値を含んだ文字列
	// debugText_->Print(strDebug, 50, 90, 1.0f);	// デバッグテキストの表示

	XMFLOAT3 move = {0, 0, 0}; //視点移動ベクトル

	const float kEyeSpeed = 0.2f; //視点移動の速さ

	if (input_->PushKey(DIK_W)) //押した方向で移動ベクトル変更
		move = {0, 0, kEyeSpeed};
	else if (input_->PushKey(DIK_S))
		move = {0, 0, -kEyeSpeed};

	viewProjection_.eye.x += move.x; //視点移動（ベクトルの加算）
	viewProjection_.eye.y += move.y;
	viewProjection_.eye.z += move.z;

	viewProjection_.UpdateMatrix(); //行列の再計算

	debugText_->SetPos(50, 50); //デバッグ用表示
	debugText_->Printf(
	  "eye:(%f,%f,%f)", viewProjection_.eye.x, viewProjection_.eye.y, viewProjection_.eye.z);

	const float kTargetSpeed = 0.2f; //注視点の移動速さ

	if (input_->PushKey(DIK_LEFT)) {
		move = {-kTargetSpeed, 0, 0};
	} else if (input_->PushKey(DIK_RIGHT)) {
		move = {kTargetSpeed, 0, 0};
	}

	viewProjection_.target.x += move.x; //注視点移動（ベクトルの加算）
	viewProjection_.target.y += move.y;
	viewProjection_.target.z += move.z;

	viewProjection_.UpdateMatrix(); //行列の再計算

	debugText_->SetPos(50, 70); //デバッグ用表示
	debugText_->Printf(
	  "target:(%f,%f,%f)", viewProjection_.target.x, viewProjection_.target.y,
	  viewProjection_.target.z);

	const float kUpRotSpeed = 0.5f;//上方向の回転速さ

	if (input_->PushKey(DIK_SPACE)) {//押した方向で移動ベクトルを変更
		viewAngle += kUpRotSpeed;
		viewAngle = fmodf(viewAngle, XM_2PI);//2πを超えたら0に戻す
	}

	viewProjection_.up = {cosf(viewAngle), sinf(viewAngle), 0.0f};//上方向ベクトルを計算

	viewProjection_.UpdateMatrix();//行列の再計算

	debugText_->SetPos(50, 90);
	debugText_->Printf(
	  "up:(%f,%f,%f)", viewProjection_.up.x, viewProjection_.up.y, viewProjection_.up.z);
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	for (size_t i = 0; i < _countof(worldTransform_); i++)
		model_->Draw(worldTransform_[i], viewProjection_, textureHandle_);
	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>
	// sprite_->Draw();

	// デバッグテキストの描画
	debugText_->DrawAll(commandList);
	//
	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
