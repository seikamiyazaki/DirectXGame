#pragma once

#include "Audio.h"
#include "DebugText.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "SafeDelete.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <DirectXMath.h>

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

  public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	// パーツID
	enum PartId {
		Root,
		Spine,
		Chest,
		Head,
		ArmL,
		ArmR,
		Hip,
		LegL,
		LegR,
	};

  private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
	DebugText* debugText_ = nullptr;
	uint32_t textureHandle_ = 0; // テクスチャハンドル
	Sprite* sprite_ = nullptr;   // スプライト
	Model* model_ = nullptr;     // 3Dモデル

	WorldTransform worldTransform_[100];
	ViewProjection viewProjection_;

	uint32_t soundDataHandle_ = 0; // サウンドデータハンドル
	uint32_t voiceHandle_ = 0;     // 音声再生ハンドル

	int32_t value_ = 0; // 値を表示したい変数

	float viewAngle = 0.0f; //カメラ上方向の角度

	/// <summary>
	/// ゲームシーン用
	/// </summary>
};
