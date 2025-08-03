#include "DxLib.h"
#include "ShootingGame.h" // �w�b�_�[�t�@�C�����C���N���[�h

// �萔�̒�`
const int WIDTH = 1200, HEIGHT = 720; // �E�B���h�E�̕��ƍ����̃s�N�Z����
const int FPS = 60; // �t���[�����[�g
const int IMG_ENEMY_MAX = 5; // �G�̉摜�̖����i��ށj
const int BULLET_MAX = 100; // ���@�����˂���e�̍ő吔

// �O���[�o���ϐ�
// �����ŃQ�[���ɗp����ϐ���z����`����
int imgGalaxy, imgFloor, imgWallL, imgWallR; // �w�i�摜
int imgFighter, imgBullet; // ���@�Ǝ��@�̒e�̉摜
int imgEnemy[IMG_ENEMY_MAX]; // �G�@�̉摜
int imgExplosion; // �������o�̉摜
int imgItem; // �A�C�e���̉摜
int bgm, jinOver, jinClear, seExpl, seItem, seShot; // ���̓ǂݍ��ݗp

struct OBJECT player; // ���@�p�̍\���̕ϐ�
struct OBJECT bullet[BULLET_MAX]; // �e�p�̍\���̂̔z��

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

	while (1) // ���C�����[�v
	{
		ClearDrawScreen(); // ��ʂ��N���A����

		// �Q�[���̍��g�݂ƂȂ鏈�����A�����ɋL�ڂ���
		scrollBG(1); // �y���z�w�i�̃X�N���[��
		movePlayer(); // ���@�̑���
		moveBullet(); // �e�̐���

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