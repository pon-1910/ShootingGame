#include "DxLib.h"
#include "ShootingGame.h" // ヘッダーファイルをインクルード

// 定数の定義
const int WIDTH = 1200, HEIGHT = 720; // ウィンドウの幅と高さのピクセル数
const int FPS = 60; // フレームレート
const int IMG_ENEMY_MAX = 5; // 敵の画像の枚数（種類）

// グローバル変数
// ここでゲームに用いる変数や配列を定義する
int imgGalaxy, imgFloor, imgWallL, imgWallR; // 背景画像
int imgFighter, imgBullet; // 自機と自機の弾の画像
int imgEnemy[IMG_ENEMY_MAX]; // 敵機の画像
int imgExplosion; // 爆発演出の画像
int imgItem; // アイテムの画像
int bgm, jinOver, jinClear, seExpl, seItem, seShot; // 音の読み込み用


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	SetWindowText("シューティングゲーム"); // ウィンドウのタイトル
	SetGraphMode(WIDTH, HEIGHT, 32); // ウィンドウの大きさとカラービット数の指定
	ChangeWindowMode(TRUE); // ウィンドウモードで起動
	if (DxLib_Init() == -1) return -1; // ライブラリ初期化 エラーが起きたら終了
	SetBackgroundColor(0, 0, 0); // 背景色の指定
	SetDrawScreen(DX_SCREEN_BACK); // 描画面を裏画面にする

	initGame(); // 初期化用の関数を呼び出す
	PlaySoundMem(bgm, DX_PLAYTYPE_LOOP); // 【仮】BGMの出力

	while (1) // メインループ
	{
		ClearDrawScreen(); // 画面をクリアする

		// ゲームの骨組みとなる処理を、ここに記載する
		scrollBG(1); // 【仮】背景のスクロール

		ScreenFlip(); // 裏画面の内容を表画面に反映する
		WaitTimer(1000 / FPS); // 一定時間保つ
		if (ProcessMessage() == -1) break; // Windows から情報を受け取りエラーが起きたら終了
		if (CheckHitKey(KEY_INPUT_ESCAPE) == 1) break; // ESCキーが押されたら終了
	}

	DxLib_End(); // DXライブラリ使用の終了処理
	return 0; // ソフトの終了
}

// ここから下に自作した関数を定義する
// 初期化用の関数
void initGame(void)
{
	// 背景用の画像の読み込み
	imgGalaxy = LoadGraph("image/bg0.png");
	imgFloor = LoadGraph("image/bg1.png");
	imgWallL = LoadGraph("image/bg2.png");
	imgWallR = LoadGraph("image/bg3.png");
	// 自機と自機の弾の画像の読み込み
	imgFighter = LoadGraph("image/fighter.png");
	imgBullet = LoadGraph("image/bullet.png");
	// 敵機の画像の読み込み
	for (int i = 0; i < IMG_ENEMY_MAX; i++) {
		char file[] = "image/enemy*.png";
		file[11] = (char)('0' + i);
		imgEnemy[i] = LoadGraph(file);
	}
	// その他の画像の読み込み
	imgExplosion = LoadGraph("image/explosion.png"); // 爆発演出
	imgItem = LoadGraph("image/item.png"); // アイテム

	// サウンドの読み込みと音量設定
	bgm = LoadSoundMem("sound/bgm.mp3");
	jinOver = LoadSoundMem("sound/gameover.mp3");
	jinClear = LoadSoundMem("sound/stageclear.mp3");
	seExpl = LoadSoundMem("sound/explosion.mp3");
	seItem = LoadSoundMem("sound/item.mp3");
	seShot = LoadSoundMem("sound/shot.mp3");
	ChangeVolumeSoundMem(128, bgm);
	ChangeVolumeSoundMem(128, jinOver);
	ChangeVolumeSoundMem(128, jinClear);
}

// 背景のスクロール
void scrollBG(int spd)
{
	static int galaxyY, floorY, wallY; // スクロール位置を管理する変数(静的記憶領域に保持される)
	galaxyY = (galaxyY + spd) % HEIGHT; // 星空（宇宙）
	DrawGraph(0, galaxyY - HEIGHT, imgGalaxy, FALSE);
	DrawGraph(0, galaxyY, imgGalaxy, FALSE);
	floorY = (floorY + spd * 2) % 120; // 床
	for (int i = -1; i < 6; i++) DrawGraph(240, floorY + i * 120, imgFloor, TRUE);
	wallY = (wallY + spd * 4) % 240; // 左右の壁
	DrawGraph(0, wallY - 240, imgWallL, TRUE);
	DrawGraph(WIDTH - 300, wallY - 240, imgWallR, TRUE);
}