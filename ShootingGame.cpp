#include "DxLib.h"
#include "ShootingGame.h" // �w�b�_�[�t�@�C�����C���N���[�h
#include <stdlib.h>

// �萔�̒�`
const int WIDTH = 1200, HEIGHT = 720; // �E�B���h�E�̕��ƍ����̃s�N�Z����
const int FPS = 60; // �t���[�����[�g
const int IMG_ENEMY_MAX = 5; // �G�̉摜�̖����i��ށj
const int BULLET_MAX = 100; // ���@�����˂���e�̍ő吔
const int ENEMY_MAX = 100; // �G�@�̐��̍ő�l
const int STAGE_DISTANCE = FPS * 60; // �X�e�[�W�̒���
const int PLAYER_SHIELD_MAX = 8; // ���@�̃V�[���h�̍ő�l
const int EFFECT_MAX = 100; // �G�t�F�N�g�̍ő吔
const int ITEM_TYPE = 3; // �A�C�e���̎��
const int WEAPON_LV_MAX = 3; // ���탌�x���̍ő�l
const int PLAYER_SPEED_MAX = 20; // ���@�̑����̍ő�l
enum { ENE_BULLET, ENE_ZAKO1, ENE_ZAKO2, ENE_ZAKO3, ENE_BOSS }; // �G�@�̎��
enum { EFF_EXPLODE, EFF_RECOVER }; // �G�t�F�N�g�̎��
enum { TITLE, PLAY, OVER, CLEAR }; // �V�[���𕪂��邽�߂̗񋓒萔

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
int stage = 1; // �X�e�[�W
int score = 0; // �X�R�A
int hisco = 10000; // �n�C�X�R�A
int noDamage = 0; // ���G���
int weaponLv = 1; // ���@�̕���̃��x���i�����ɔ��˂����e���j
int scene = TITLE; // �V�[�����Ǘ�
int timer = 0; // ���Ԃ̐i�s���Ǘ�

struct OBJECT player; // ���@�p�̍\���̕ϐ�
struct OBJECT bullet[BULLET_MAX]; // �e�p�̍\���̂̔z��
struct OBJECT enemy[ENEMY_MAX]; // �G�@�p�̍\���̂̔z��
struct OBJECT effect[EFFECT_MAX]; // �G�t�F�N�g�p�̍\���̂̔z��
struct OBJECT item; // �A�C�e���p�̍\����

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
		int spd = 1; // �X�N���[���̑���
		if (scene == PLAY && distance == 0) spd = 0; // �{�X��̓X�N���[����~
		scrollBG(spd); // �w�i�̃X�N���[��
		moveEnemy(); // �G�@�̐���
		moveBullet(); // �e�̐���
		moveItem(); // �A�C�e���̐���
		drawEffect(); // �G�t�F�N�g
		stageMap(); // �X�e�[�W�}�b�v
		drawParameter(); // ���@�̃V�[���h�Ȃǂ̃p�����[�^�[��\��

		timer++; // �^�C�}�[���J�E���g
		switch (scene) // �V�[�����Ƃɏ����𕪊�
		{
		case TITLE: // �^�C�g�����
			drawTextC(WIDTH * 0.5, HEIGHT * 0.3, "Shooting Game", 0xffffff, 80);
			drawTextC(WIDTH * 0.5, HEIGHT * 0.7, "Press SPACE to start", 0xffffff, 30);
			if (CheckHitKey(KEY_INPUT_SPACE))
			{
				initVariable();
				scene = PLAY;
			}
			break;
		case PLAY: // �Q�[���v���C���
			movePlayer(); // ���@�̑���
		}
		/* �������܂ŕύX�ς݁� */

		if (distance > 0) distance--; // �����̌v�Z
		DrawFormatString(0, 0, 0xffff00, "SCORE %d HI-SCO %d", score, hisco); // �y���z�m�F�p
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
		if (distance % 800 == 1) setItem(); // �A�C�e���̏o��

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
	player.shield = PLAYER_SHIELD_MAX;
	GetGraphSize(imgFighter, &player.wid, &player.hei); // ���@�̉摜�̕��ƍ�������
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
	if (noDamage > 0) noDamage--; // ���G���Ԃ̃J�E���g
	if (noDamage % 4 < 2) drawImage(imgFighter, player.x, player.y); // ���@�̕`��
}

// �e�̃Z�b�g�i���ˁj
void setBullet(void)
{
	for (int n = 0; n < weaponLv; n++) {
		int x = player.x - (weaponLv - 1) * 5 + n * 10;
		int y = player.y - 20;
		for (int i = 0; i < BULLET_MAX; i++) {
			if (bullet[i].state == 0) { // �󂢂Ă���z��ɒe���Z�b�g
				bullet[i].x = x;
				bullet[i].y = y;
				bullet[i].vx = 0;
				bullet[i].vy = -40; // y�������̑����i1��̌v�Z�ňړ��ł���s�N�Z�����j
				bullet[i].state = 1; // �e�����݂����Ԃɂ���
				break;
			}
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
			enemy[i].shield = sld * stage; // �X�e�[�W���i�ނقǓG���ł��Ȃ�
			GetGraphSize(img, &enemy[i].wid, &enemy[i].hei); // �摜�̕��ƍ�������
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
		// �����蔻��̃A���S���Y��
		if (enemy[i].shield > 0) // �q�b�g�`�F�b�N���s���G�@�i�e�ȊO�j
		{
			for (int j = 0; j < BULLET_MAX; j++) { // ���@�̒e�ƃq�b�g�`�F�b�N
				if (bullet[j].state == 0) continue;
				int dx = abs((int)(enemy[i].x - bullet[j].x)); // �����S���W�Ԃ̃s�N�Z����
				int dy = abs((int)(enemy[i].y - bullet[j].y)); // ��
				if (dx < enemy[i].wid / 2 && dy < enemy[i].hei / 2) // �ڐG���Ă��邩
				{
					bullet[j].state = 0; // �e������
					damageEnemy(i, 1); // �G�@�Ƀ_���[�W
				}
			}
		}
		if (noDamage == 0) // ���G��ԂłȂ����A���@�ƃq�b�g�`�F�b�N
		{
			int dx = abs(enemy[i].x - player.x); // �����S���W�Ԃ̃s�N�Z����
			int dy = abs(enemy[i].y - player.y); // ��
			if (dx < enemy[i].wid / 2 + player.wid / 2 && dy < enemy[i].hei / 2 + player.hei / 2)
			{
				if (player.shield > 0) player.shield--; // �V�[���h�����炷
				noDamage = FPS; // ���G��Ԃ��Z�b�g
				damageEnemy(i, 1); // �G�Ƀ_���[�W
			}
		}
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

// �G�@�̃V�[���h�����炷�i�_���[�W��^����j
void damageEnemy(int n, int dmg)
{
	SetDrawBlendMode(DX_BLENDMODE_ADD, 192); // ���Z�ɂ��`��̏d�ˍ��킹
	DrawCircle(enemy[n].x, enemy[n].y, (enemy[n].wid + enemy[n].hei) / 4, 0xff0000, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0); // �u�����h���[�h������
	score += 100; // �X�R�A�̉��Z
	if (score > hisco) hisco = score; // �n�C�X�R�A�̍X�V
	enemy[n].shield -= dmg; // �V�[���h�����炷
	if (enemy[n].shield <= 0)
	{
		enemy[n].state = 0; // �V�[���h0�ȉ��ŏ���
		setEffect(enemy[n].x, enemy[n].y, EFF_EXPLODE); // �������o
	}
}

// �e��t�����������l��\������֐�
void drawText(int x, int y, const char* txt, int val, int col, int siz)
{
	SetFontSize(siz); // �t�H���g�̑傫�����w��
	DrawFormatString(x + 1, y + 1, 0x000000, txt, val); // ���ŕ������\��
	DrawFormatString(x, y, col, txt, val); // �����̐F�ŕ������\��
}

// ���@�Ɋւ���p�����[�^�[��\��
void drawParameter(void)
{
	int x = 10, y = HEIGHT - 30; // �\���ʒu
	DrawBox(x, y, x + PLAYER_SHIELD_MAX * 30, y + 20, 0x000000, TRUE);
	for (int i = 0; i < player.shield; i++) // �V�[���h�̃��[�^�[
	{
		int r = 128 * (PLAYER_SHIELD_MAX - i) / PLAYER_SHIELD_MAX; // RGB�l���v�Z
		int g = 255 * i / PLAYER_SHIELD_MAX;
		int b = 160 + 96 * i / PLAYER_SHIELD_MAX;
		DrawBox(x + 2 + i * 30, y + 2, x + 28 + i * 30, y + 18, GetColor(r, g, b), TRUE);
	}
	drawText(x, y - 25, "SHIELD Lv %02d", player.shield, 0xffffff, 20); // �V�[���h�l
	drawText(x, y - 50, "WEAPON Lv %02d", weaponLv, 0xffffff, 20); // ���탌�x��
	drawText(x, y - 75, "SPEED %02d", player.vx, 0xffffff, 20); // �ړ����x
}

// �G�t�F�N�g�̃Z�b�g
void setEffect(int x, int y, int ptn)
{
	static int eff_num;
	effect[eff_num].x = x;
	effect[eff_num].y = y;
	effect[eff_num].state = 1;
	effect[eff_num].pattern = ptn;
	effect[eff_num].timer = 0;
	eff_num = (eff_num + 1) % EFFECT_MAX;
	if (ptn == EFF_EXPLODE) PlaySoundMem(seExpl, DX_PLAYTYPE_BACK); // ���ʉ�
}

// �G�t�F�N�g�̕`��
void drawEffect(void)
{
	int ix;
	for (int i = 0; i < EFFECT_MAX; i++)
	{
		if (effect[i].state == 0) continue;
		switch (effect[i].pattern) // �G�t�F�N�g���Ƃɏ����𕪂���
		{
		case EFF_EXPLODE: // �������o
			ix = effect[i].timer * 128; // �摜�̐؂�o���ʒu
			DrawRectGraph(effect[i].x - 64, effect[i].y - 64, ix, 0, 128, 128, imgExplosion, TRUE, FALSE);
			effect[i].timer++;
			if (effect[i].timer == 7) effect[i].state = 0;
			break;

		case EFF_RECOVER: // �񕜉��o
			if (effect[i].timer < 30) // ���Z�ɂ��`��̏d�ˍ��킹
				SetDrawBlendMode(DX_BLENDMODE_ADD, effect[i].timer*8);
			else
				SetDrawBlendMode(DX_BLENDMODE_ADD, (60 - effect[i].timer) * 8);
			for (int i = 3; i < 8; i++) DrawCircle(player.x, player.y, (player.wid + player.hei) / i, 0x2040c0, TRUE);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0); // �u�����h���[�h������
			effect[i].timer++;
			if (effect[i].timer == 60) effect[i].state = 0;
			break;
		}
	}
}

// �A�C�e�����Z�b�g
void setItem(void)
{
	item.x = (WIDTH / 4) * (1 + rand() % 3);
	item.y = -16;
	item.vx = 15;
	item.vy = 1;
	item.state = 1;
	item.timer = 0;
}

// �A�C�e���̏���
void moveItem(void)
{
	if (item.state == 0) return;
	item.x += item.vx;
	item.y += item.vy;
	if (item.timer % 60 < 30)
		item.vx -= 1;
	else
		item.vx += 1;
	if (item.y > HEIGHT + 16) item.state = 0;
	item.pattern = (item.timer / 120) % ITEM_TYPE; // ���݁A�ǂ̃A�C�e���ɂȂ��Ă��邩
	item.timer++;
	DrawRectGraph(item.x - 20, item.y - 16, item.pattern * 40, 0, 40, 32, imgItem, TRUE, FALSE);
	// if (scene == OVER) return; // �Q�[���I�[�o�[��ʂł͉���ł��Ȃ�
	int dis = (item.x - player.x) * (item.x - player.x) + (item.y - player.y) * (item.y - player.y);
	if (dis < 60 * 60) // �A�C�e���Ǝ��@�Ƃ̃q�b�g�`�F�b�N�i�~�ɂ�铖���蔻��j
	{
		item.state = 0;
		if (item.pattern == 0) // �X�s�[�h�A�b�v
		{
			if (player.vx < PLAYER_SPEED_MAX)
			{
				player.vx += 3;
				player.vy += 3;
			}
		}
		if (item.pattern == 1) // �V�[���h��
		{
			if (player.shield < PLAYER_SHIELD_MAX) player.shield++;
			setEffect(player.x, player.y, EFF_RECOVER); // �񕜃G�t�F�N�g��\��
		}
		if (item.pattern == 2) // ���탌�x���A�b�v
		{
			if (weaponLv < WEAPON_LV_MAX) weaponLv++;
		}
		PlaySoundMem(seItem, DX_PLAYTYPE_BACK); // ���ʉ�
	}
}