#include "DxLib.h"
#include "ShootingGame.h" // �w�b�_�[�t�@�C�����C���N���[�h

// �萔�̒�`
const int WIDTH = 1200, HEIGHT = 720; // �E�B���h�E�̕��ƍ����̃s�N�Z����
const int FPS = 60; // �t���[�����[�g
const int IMG_ENEMY_MAX = 5; // �G�̉摜�̖����i��ށj

// �O���[�o���ϐ�
// �����ŃQ�[���ɗp����ϐ���z����`����
int imgGalaxy, imgFloor, imgWallL, imgWallR; // �w�i�摜
int imgFighter, imgBullet; // ���@�Ǝ��@�̒e�̉摜
int imgEnemy[IMG_ENEMY_MAX]; // �G�@�̉摜
int imgExplosion; // �������o�̉摜
int imgItem; // �A�C�e���̉摜
int bgm, jinOver, jinClear, seExpl, seItem, seShot; // ���̓ǂݍ��ݗp


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	SetWindowText("�V���[�e�B���O�Q�[��"); // �E�B���h�E�̃^�C�g��
	SetGraphMode(WIDTH, HEIGHT, 32); // �E�B���h�E�̑傫���ƃJ���[�r�b�g���̎w��
	ChangeWindowMode(TRUE); // �E�B���h�E���[�h�ŋN��
	if (DxLib_Init() == -1) return -1; // ���C�u���������� �G���[���N������I��
	SetBackgroundColor(0, 0, 0); // �w�i�F�̎w��
	SetDrawScreen(DX_SCREEN_BACK); // �`��ʂ𗠉�ʂɂ���

	initGame(); // �������p�̊֐����Ăяo��
	PlaySoundMem(bgm, DX_PLAYTYPE_LOOP); // �y���zBGM�̏o��

	while (1) // ���C�����[�v
	{
		ClearDrawScreen(); // ��ʂ��N���A����

		// �Q�[���̍��g�݂ƂȂ鏈�����A�����ɋL�ڂ���
		scrollBG(1); // �y���z�w�i�̃X�N���[��

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