#include "DxLib.h"
#include "ShootingGame.h" // �w�b�_�[�t�@�C�����C���N���[�h

// �萔�̒�`
const int WIDTH = 1200, HEIGHT = 720; // �E�B���h�E�̕��ƍ����̃s�N�Z����
const int FPS = 60; // �t���[�����[�g
const int IMG_ENEMY_MAX = 5; // �G�̉摜�̖����i��ށj
const int BULLET_MAX = 100; // ���@�����˂���e�̍ő吔
const int ENEMY_MAX = 100; // �G�@�̐��̍ő�l
const int STAGE_DISTANCE = FPS * 60; // �X�e�[�W�̒���
enum { ENE_BULLET, ENE_ZAKO1, ENE_ZAKO2, ENE_ZAKO3, ENE_BOSS }; // �G�@�̎��

// �O���[�o���ϐ�
// �����ŃQ�[���ɗp����ϐ���z����`����
int imgGalaxy, imgFloor, imgWallL, imgWallR; // �w�i�摜
int imgFighter, imgBullet; // ���@�Ǝ��@�̒e�̉摜
int imgEnemy[IMG_ENEMY_MAX]; // �G�@�̉摜
int imgExplosion; // �������o�̉摜
int imgItem; // �A�C�e���̉摜
int bgm, jinOver, jinClear, seExpl, seItem, seShot; // ���̓ǂݍ��ݗp
int distance = 0; // �X�e�[�W�I�[�܂ł̋���
int bossIdx = 0; // �{�X���������z��̃C���f�b�N�X

struct OBJECT player; // ���@�p�̍\���̕ϐ�
struct OBJECT bullet[BULLET_MAX]; // �e�p�̍\���̂̔z��
struct OBJECT enemy[ENEMY_MAX]; // �G�@�p�̍\���̂̔z��

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	SetWindowText("�V���[�e�B���O�Q�[��"); // �E�B���h�E�̃^�C�g��
	SetGraphMode(WIDTH, HEIGHT, 32); // �E�B���h�E�̑傫���ƃJ���[�r�b�g���̎w��
	ChangeWindowMode(TRUE); // �E�B���h�E���[�h�ŋN��
	if (DxLib_Init() == -1) return -1; // ���C�u���������� �G���[���N������I��
	SetBackgroundColor(0, 0, 0); // �w�i�F�̎w��
	SetDrawScreen(DX_SCREEN_BACK); // �`��ʂ𗠉�ʂɂ���

	initGame(); // �������p�̊֐����Ăяo��
	initVariable(); // �y���z�Q�[��������������ۂɌĂяo���ʒu��ς���
	distance = STAGE_DISTANCE; // �y�L�q�ʒu�͉��z�X�e�[�W�̒�������

	while (1) // ���C�����[�v
	{
		ClearDrawScreen(); // ��ʂ��N���A����

		// �Q�[���̍��g�݂ƂȂ鏈�����A�����ɋL�ڂ���
		scrollBG(1); // �y���z�w�i�̃X�N���[��
		if (distance > 0) distance--; // �����̌v�Z
		DrawFormatString(0, 0, 0xffff00, "distance=%d", distance); // �y���z�m�F�p
		if (distance % 60 == 1) // �y���z�U�R�G�̏o��
		{
			int x = 100 + rand() % (WIDTH - 200); // �o���ʒu�@x���W
			int y = -50;						  // �o���ʒu�@y���W
			int e = 1 + rand() % 2; // �o������U�R�@�̎��
			if (e == ENE_ZAKO1) setEnemy(x, y, 0, 3, ENE_ZAKO1, imgEnemy[ENE_ZAKO1], 1);
			if (e == ENE_ZAKO2) {
				int vx = 0;
				if (player.x < x - 50) vx = -3;
				if (player.x > x + 50) vx = 3;
				setEnemy(x, -100, vx, 5, ENE_ZAKO2, imgEnemy[ENE_ZAKO2], 3);
			}
		}
		if (distance % 120 == 1) // �y���z�U�R�G3 �̏o��
		{
			int x = 100 + rand() % (WIDTH - 200); // �o���ʒu x���W
			setEnemy(x, -100, 0, 40 + rand() % 20, ENE_ZAKO3, imgEnemy[ENE_ZAKO3], 5);
		}
		if (distance == 1) bossIdx = setEnemy(WIDTH / 2, -120, 0, 1, ENE_BOSS, imgEnemy[ENE_BOSS], 200); // �{�X�o��
		moveEnemy(); // �G�@�̐���
		movePlayer(); // ���@�̑���
		moveBullet(); // �e�̐���
		stageMap(); // �X�e�[�W�}�b�v

		ScreenFlip(); // ����ʂ̓��e��\��ʂɔ��f����
		WaitTimer(1000 / FPS); // ��莞�ԕۂ�
		if (ProcessMessage() == -1) break; // Windows ��������󂯎��G���[���N������I��
		if (CheckHitKey(KEY_INPUT_ESCAPE) == 1) break; // ESC�L�[�������ꂽ��I��
	}

	DxLib_End(); // DX���C�u�����g�p�̏I������
	return 0; // �\�t�g�̏I��
}

// �������牺�Ɏ��삵���֐����`����
// �������p�̊֐�
void initGame(void)
{
	// �w�i�p�̉摜�̓ǂݍ���
	imgGalaxy = LoadGraph("image/bg0.png");
	imgFloor = LoadGraph("image/bg1.png");
	imgWallL = LoadGraph("image/bg2.png");
	imgWallR = LoadGraph("image/bg3.png");
	// ���@�Ǝ��@�̒e�̉摜�̓ǂݍ���
	imgFighter = LoadGraph("image/fighter.png");
	imgBullet = LoadGraph("image/bullet.png");
	// �G�@�̉摜�̓ǂݍ���
	for (int i = 0; i < IMG_ENEMY_MAX; i++) {
		char file[] = "image/enemy*.png";
		file[11] = (char)('0' + i);
		imgEnemy[i] = LoadGraph(file);
	}
	// ���̑��̉摜�̓ǂݍ���
	imgExplosion = LoadGraph("image/explosion.png"); // �������o
	imgItem = LoadGraph("image/item.png"); // �A�C�e��

	// �T�E���h�̓ǂݍ��݂Ɖ��ʐݒ�
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

// �w�i�̃X�N���[��
void scrollBG(int spd)
{
	static int galaxyY, floorY, wallY; // �X�N���[���ʒu���Ǘ�����ϐ�(�ÓI�L���̈�ɕێ������)
	galaxyY = (galaxyY + spd) % HEIGHT; // ����i�F���j
	DrawGraph(0, galaxyY - HEIGHT, imgGalaxy, FALSE);
	DrawGraph(0, galaxyY, imgGalaxy, FALSE);
	floorY = (floorY + spd * 2) % 120; // ��
	for (int i = -1; i < 6; i++) DrawGraph(240, floorY + i * 120, imgFloor, TRUE);
	wallY = (wallY + spd * 4) % 240; // ���E�̕�
	DrawGraph(0, wallY - 240, imgWallL, TRUE);
	DrawGraph(WIDTH - 300, wallY - 240, imgWallR, TRUE);
}

// �Q�[���J�n���̏����l��������֐�
void initVariable(void)
{
	player.x = WIDTH / 2;
	player.y = HEIGHT / 2;
	player.vx = 5;
	player.vy = 5;
}

// ���S���W���w�肵�ĉ摜��\������֐�
void drawImage(int img, int x, int y)
{
	int w, h;
	GetGraphSize(img, &w, &h);
	DrawGraph(x - w / 2, y - h / 2, img, TRUE);
}

// ���@�𓮂����֐�
void movePlayer(void)
{
	static char oldSpcKey; // 1�O�̃X�y�[�X�L�[�̏�Ԃ�ێ�����ϐ�
	static int countSpcKey; // �X�y�[�X�L�[�����������Ă���ԁA�J�E���g�A�b�v����ϐ�
	if (CheckHitKey(KEY_INPUT_UP)) { // ��L�[
		player.y -= player.vy;
		if (player.y < 30) player.y = 30;
	}
	if (CheckHitKey(KEY_INPUT_DOWN)) { // ���L�[
		player.y += player.vy;
		if (player.y > HEIGHT - 30) player.y = HEIGHT - 30;
	}
	if (CheckHitKey(KEY_INPUT_LEFT)) { // ���L�[
		player.x -= player.vx;
		if (player.x < 30) player.x = 30;
	}
	if (CheckHitKey(KEY_INPUT_RIGHT)) { // �E�L�[
		player.x += player.vx;
		if (player.x > WIDTH - 30) player.x = WIDTH - 30;
	}
	if (CheckHitKey(KEY_INPUT_SPACE)) { // �X�y�[�X�L�[
		if (oldSpcKey == 0) setBullet(); // �������u�ԁA����
		else if (countSpcKey % 20 == 0) setBullet(); // ���Ԋu�Ŕ���
		countSpcKey++;
	}
	else {
		countSpcKey = 0;
	}
	oldSpcKey = CheckHitKey(KEY_INPUT_SPACE); // �X�y�[�X�L�[�̏�Ԃ�ۑ�
	drawImage(imgFighter, player.x, player.y); // ���@�̕`��
}

// �e�̃Z�b�g�i���ˁj
void setBullet(void)
{
	for (int i = 0; i < BULLET_MAX; i++) {
		if (bullet[i].state == 0) { // �󂢂Ă���z��ɒe���Z�b�g
			bullet[i].x = player.x;
			bullet[i].y = player.y - 20;
			bullet[i].vx = 0;
			bullet[i].vy = -40; // y�������̑����i1��̌v�Z�ňړ��ł���s�N�Z�����j
			bullet[i].state = 1; // �e�����݂����Ԃɂ���
			break;
		}
	}
	PlaySoundMem(seShot, DX_PLAYTYPE_BACK); // ���ʉ�
}

// �e�̈ړ�
void moveBullet(void)
{
	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (bullet[i].state == 0) continue; // �󂢂Ă���z��Ȃ珈�����Ȃ�
		bullet[i].x += bullet[i].vx; // �� ���W���ړ�������
		bullet[i].y += bullet[i].vy; // ��
		drawImage(imgBullet, bullet[i].x, bullet[i].y); // �e�̕`��
		if (bullet[i].y < -100) bullet[i].state = 0; // ��ʊO�ɏo���瑶�݂��Ȃ���Ԃɂ���
	}
}

// �G�@���Z�b�g����
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
			// enemy[i].shield = sld * stage; // �X�e�[�W���i�ނقǓG���ł��Ȃ�
			// GetGraphSize(img, &enemy[i].wid, &enemy[i].hei); // �摜�̕��ƍ�������
			return i;
		}
	}
	return -1;
}

// �G�@�𓮂���
void moveEnemy(void)
{
	for (int i = 0; i < ENEMY_MAX; i++) {
		if (enemy[i].state == 0) continue; // �󂢂Ă���z��Ȃ珈�����Ȃ�
		if (enemy[i].pattern == ENE_ZAKO3) // �U�R�@3
		{
			if (enemy[i].vy > 1) // ����
			{
				enemy[i].vy *= 0.9;
			}
			else if (enemy[i].vy > 0) // �e���ˁA��ы���
			{
				setEnemy(enemy[i].x, enemy[i].y, 0, 6, ENE_BULLET, imgEnemy[ENE_BULLET], 0); // �e
				enemy[i].vx = 8;
				enemy[i].vy = -4;
			}
		}
		if (enemy[i].pattern == ENE_BOSS) // �{�X�@
		{
			if (enemy[i].y > HEIGHT - 120) enemy[i].vy = -2;
			if (enemy[i].y < 120) // ��ʏ�[
			{
				if (enemy[i].vy < 0) // �e����
				{
					for (int bx = -2; bx <= 2; bx++) // ��d���[�v�� for
						for (int by = 0; by <= 3; by++)
						{
							if (bx == 0 && by == 0) continue;
							setEnemy(enemy[i].x, enemy[i].y, bx * 2, by * 3, ENE_BULLET, imgEnemy[ENE_BULLET], 0);
						}
				}
				enemy[i].vy = 2;
			}
		}
		enemy[i].x += enemy[i].vx; // �� �G�@�̈ړ�
		enemy[i].y += enemy[i].vy; // ��
		drawImage(enemy[i].image, enemy[i].x, enemy[i].y); // �G�@�̕`��
		// ��ʊO�ɏo�����H
		if (enemy[i].x < -200 || WIDTH + 200 < enemy[i].x || enemy[i].y < -200 || HEIGHT + 200 < enemy[i].y) enemy[i].state = 0;
	}
}

// �X�e�[�W�}�b�v
void stageMap(void)
{
	int mx = WIDTH - 30, my = 60; // �}�b�v�̕\���ʒu
	int wi = 20, he = HEIGHT - 120; // �}�b�v�̕��A����
	int pos = (HEIGHT - 140) * distance / STAGE_DISTANCE; // ���@�̔�s���Ă���ʒu
	SetDrawBlendMode(DX_BLENDMODE_SUB, 128); // ���Z�ɂ��`��̏d�ˍ��킹
	DrawBox(mx, my, mx + wi + 1, my + he, 0xffffff, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0); // �u�����h���[�h������
	DrawBox(mx-1, my-1, mx + wi + 1, my + he + 1, 0xffffff, FALSE); // �g��
	DrawBox(mx, my + pos, mx + wi, my + pos + 20, 0x0080ff, TRUE); // ���@�̈ʒu
}