#pragma once

// �\���̂̐錾
struct OBJECT // ���@��G�@�p
{
	int x; // X���W
	int y; // Y���W
	int vx; // X�������̑���
	int vy; // Y�������̑���
	int state; // ���݂��邩
	int pattern; // �G�@�̓����̃p�^�[��
	int image; // �摜
	int wid; // �摜�̕�(�s�N�Z����)
	int hei; // �摜�̍���
	int shield; // �V�[���h(�ϋv��)
	int timer; // �^�C�}�[
};

// �֐��v���g�^�C�v�錾
// �����Ƀv���g�^�C�v�錾���L�q����
void initGame(void);