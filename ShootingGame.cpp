#include "DxLib.h"
#include "ShootingGame.h" // ヘッダーファイルをインクルード

// 定数の定義
const int WIDTH = 1200, HEIGHT = 720; // ウィンドウの幅と高さのピクセル数
const int FPS = 60; // フレームレート
const int IMG_ENEMY_MAX = 5; // 敵の画像の枚数（種類）
const int BULLET_MAX = 100; // 自機が発射する弾の最大数
const int ENEMY_MAX = 100; // 敵機の数の最大値
const int STAGE_DISTANCE = FPS * 60; // ステージの長さ
enum { ENE_BULLET, ENE_ZAKO1, ENE_ZAKO2, ENE_ZAKO3, ENE_BOSS }; // 敵機の種類

// グローバル変数
// ここでゲームに用いる変数や配列を定義する
int imgGalaxy, imgFloor, imgWallL, imgWallR; // 背景画像
int imgFighter, imgBullet; // 自機と自機の弾の画像
int imgEnemy[IMG_ENEMY_MAX]; // 敵機の画像
int imgExplosion; // 爆発演出の画像
int imgItem; // アイテムの画像
int bgm, jinOver, jinClear, seExpl, seItem, seShot; // 音の読み込み用
int distance = 0; // ステージ終端までの距離
int bossIdx = 0; // ボスを代入した配列のインデックス

struct OBJECT player; // 自機用の構造体変数
struct OBJECT bullet[BULLET_MAX]; // 弾用の構造体の配列
struct OBJECT enemy[ENEMY_MAX]; // 敵機用の構造体の配列

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	SetWindowText("シューティングゲーム"); // ウィンドウのタイトル
	SetGraphMode(WIDTH, HEIGHT, 32); // ウィンドウの大きさとカラービット数の指定
	ChangeWindowMode(TRUE); // ウィンドウモードで起動
	if (DxLib_Init() == -1) return -1; // ライブラリ初期化 エラーが起きたら終了
	SetBackgroundColor(0, 0, 0); // 背景色の指定
	SetDrawScreen(DX_SCREEN_BACK); // 描画面を裏画面にする

	initGame(); // 初期化用の関数を呼び出す
	initVariable(); // 【仮】ゲームを完成させる際に呼び出し位置を変える
	distance = STAGE_DISTANCE; // 【記述位置は仮】ステージの長さを代入

	while (1) // メインループ
	{
		ClearDrawScreen(); // 画面をクリアする

		// ゲームの骨組みとなる処理を、ここに記載する
		scrollBG(1); // 【仮】背景のスクロール
		if (distance > 0) distance--; // 距離の計算
		DrawFormatString(0, 0, 0xffff00, "distance=%d", distance); // 【仮】確認用
		if (distance % 60 == 1) // 【仮】ザコ敵の出現
		{
			int x = 100 + rand() % (WIDTH - 200); // 出現位置　x座標
			int y = -50;						  // 出現位置　y座標
			int e = 1 + rand() % 2; // 出現するザコ機の種類
			if (e == ENE_ZAKO1) setEnemy(x, y, 0, 3, ENE_ZAKO1, imgEnemy[ENE_ZAKO1], 1);
			if (e == ENE_ZAKO2) {
				int vx = 0;
				if (player.x < x - 50) vx = -3;
				if (player.x > x + 50) vx = 3;
				setEnemy(x, -100, vx, 5, ENE_ZAKO2, imgEnemy[ENE_ZAKO2], 3);
			}
		}
		if (distance % 120 == 1) // 【仮】ザコ敵3 の出現
		{
			int x = 100 + rand() % (WIDTH - 200); // 出現位置 x座標
			setEnemy(x, -100, 0, 40 + rand() % 20, ENE_ZAKO3, imgEnemy[ENE_ZAKO3], 5);
		}
		if (distance == 1) bossIdx = setEnemy(WIDTH / 2, -120, 0, 1, ENE_BOSS, imgEnemy[ENE_BOSS], 200); // ボス出現
		moveEnemy(); // 敵機の制御
		movePlayer(); // 自機の操作
		moveBullet(); // 弾の制御
		stageMap(); // ステージマップ

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

// ゲーム開始時の初期値を代入する関数
void initVariable(void)
{
	player.x = WIDTH / 2;
	player.y = HEIGHT / 2;
	player.vx = 5;
	player.vy = 5;
}

// 中心座標を指定して画像を表示する関数
void drawImage(int img, int x, int y)
{
	int w, h;
	GetGraphSize(img, &w, &h);
	DrawGraph(x - w / 2, y - h / 2, img, TRUE);
}

// 自機を動かす関数
void movePlayer(void)
{
	static char oldSpcKey; // 1つ前のスペースキーの状態を保持する変数
	static int countSpcKey; // スペースキーを押し続けている間、カウントアップする変数
	if (CheckHitKey(KEY_INPUT_UP)) { // 上キー
		player.y -= player.vy;
		if (player.y < 30) player.y = 30;
	}
	if (CheckHitKey(KEY_INPUT_DOWN)) { // 下キー
		player.y += player.vy;
		if (player.y > HEIGHT - 30) player.y = HEIGHT - 30;
	}
	if (CheckHitKey(KEY_INPUT_LEFT)) { // 左キー
		player.x -= player.vx;
		if (player.x < 30) player.x = 30;
	}
	if (CheckHitKey(KEY_INPUT_RIGHT)) { // 右キー
		player.x += player.vx;
		if (player.x > WIDTH - 30) player.x = WIDTH - 30;
	}
	if (CheckHitKey(KEY_INPUT_SPACE)) { // スペースキー
		if (oldSpcKey == 0) setBullet(); // 押した瞬間、発射
		else if (countSpcKey % 20 == 0) setBullet(); // 一定間隔で発射
		countSpcKey++;
	}
	else {
		countSpcKey = 0;
	}
	oldSpcKey = CheckHitKey(KEY_INPUT_SPACE); // スペースキーの状態を保存
	drawImage(imgFighter, player.x, player.y); // 自機の描画
}

// 弾のセット（発射）
void setBullet(void)
{
	for (int i = 0; i < BULLET_MAX; i++) {
		if (bullet[i].state == 0) { // 空いている配列に弾をセット
			bullet[i].x = player.x;
			bullet[i].y = player.y - 20;
			bullet[i].vx = 0;
			bullet[i].vy = -40; // y軸方向の速さ（1回の計算で移動できるピクセル数）
			bullet[i].state = 1; // 弾が存在する状態にする
			break;
		}
	}
	PlaySoundMem(seShot, DX_PLAYTYPE_BACK); // 効果音
}

// 弾の移動
void moveBullet(void)
{
	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (bullet[i].state == 0) continue; // 空いている配列なら処理しない
		bullet[i].x += bullet[i].vx; // ┬ 座標を移動させる
		bullet[i].y += bullet[i].vy; // ┘
		drawImage(imgBullet, bullet[i].x, bullet[i].y); // 弾の描画
		if (bullet[i].y < -100) bullet[i].state = 0; // 画面外に出たら存在しない状態にする
	}
}

// 敵機をセットする
int setEnemy(int x, int y, int vx, int vy, int ptn, int img, int sld)
{
	for (int i = 0; i < ENEMY_MAX; i++) {
		if (enemy[i].state == 0) {
			enemy[i].x = x;
			enemy[i].y = y;
			enemy[i].vx = vx;
			enemy[i].vy = vy;
			enemy[i].state = 1;
			enemy[i].pattern = ptn;
			enemy[i].image = img;
			// enemy[i].shield = sld * stage; // ステージが進むほど敵が固くなる
			// GetGraphSize(img, &enemy[i].wid, &enemy[i].hei); // 画像の幅と高さを代入
			return i;
		}
	}
	return -1;
}

// 敵機を動かす
void moveEnemy(void)
{
	for (int i = 0; i < ENEMY_MAX; i++) {
		if (enemy[i].state == 0) continue; // 空いている配列なら処理しない
		if (enemy[i].pattern == ENE_ZAKO3) // ザコ機3
		{
			if (enemy[i].vy > 1) // 減速
			{
				enemy[i].vy *= 0.9;
			}
			else if (enemy[i].vy > 0) // 弾発射、飛び去る
			{
				setEnemy(enemy[i].x, enemy[i].y, 0, 6, ENE_BULLET, imgEnemy[ENE_BULLET], 0); // 弾
				enemy[i].vx = 8;
				enemy[i].vy = -4;
			}
		}
		if (enemy[i].pattern == ENE_BOSS) // ボス機
		{
			if (enemy[i].y > HEIGHT - 120) enemy[i].vy = -2;
			if (enemy[i].y < 120) // 画面上端
			{
				if (enemy[i].vy < 0) // 弾発射
				{
					for (int bx = -2; bx <= 2; bx++) // 二重ループの for
						for (int by = 0; by <= 3; by++)
						{
							if (bx == 0 && by == 0) continue;
							setEnemy(enemy[i].x, enemy[i].y, bx * 2, by * 3, ENE_BULLET, imgEnemy[ENE_BULLET], 0);
						}
				}
				enemy[i].vy = 2;
			}
		}
		enemy[i].x += enemy[i].vx; // ┬ 敵機の移動
		enemy[i].y += enemy[i].vy; // ┘
		drawImage(enemy[i].image, enemy[i].x, enemy[i].y); // 敵機の描画
		// 画面外に出たか？
		if (enemy[i].x < -200 || WIDTH + 200 < enemy[i].x || enemy[i].y < -200 || HEIGHT + 200 < enemy[i].y) enemy[i].state = 0;
	}
}

// ステージマップ
void stageMap(void)
{
	int mx = WIDTH - 30, my = 60; // マップの表示位置
	int wi = 20, he = HEIGHT - 120; // マップの幅、高さ
	int pos = (HEIGHT - 140) * distance / STAGE_DISTANCE; // 自機の飛行している位置
	SetDrawBlendMode(DX_BLENDMODE_SUB, 128); // 減算による描画の重ね合わせ
	DrawBox(mx, my, mx + wi + 1, my + he, 0xffffff, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0); // ブレンドモードを解除
	DrawBox(mx-1, my-1, mx + wi + 1, my + he + 1, 0xffffff, FALSE); // 枠線
	DrawBox(mx, my + pos, mx + wi, my + pos + 20, 0x0080ff, TRUE); // 自機の位置
}