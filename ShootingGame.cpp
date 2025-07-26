#include "DxLib.h"
#include "ShootingGame.h" // �w�b�_�[�t�@�C�����C���N���[�h

// �萔�̒�`
const int WIDTH = 1200, HEIGHT = 720; // �E�B���h�E�̕��ƍ����̃s�N�Z����
const int FPS = 60; // �t���[�����[�g

// �O���[�o���ϐ�
// �����ŃQ�[���ɗp����ϐ���z����`����

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	SetWindowText("�V���[�e�B���O�Q�[��"); // �E�B���h�E�̃^�C�g��
	SetGraphMode(WIDTH, HEIGHT, 32); // �E�B���h�E�̑傫���ƃJ���[�r�b�g���̎w��
	ChangeWindowMode(TRUE); // �E�B���h�E���[�h�ŋN��
	if (DxLib_Init() == -1) return -1; // ���C�u���������� �G���[���N������I��
	SetBackgroundColor(0, 0, 0); // �w�i�F�̎w��
	SetDrawScreen(DX_SCREEN_BACK); // �`��ʂ𗠉�ʂɂ���

	while (1) // ���C�����[�v
	{
		ClearDrawScreen(); // ��ʂ��N���A����

		// �Q�[���̍��g�݂ƂȂ鏈�����A�����ɋL�ڂ���

		ScreenFlip(); // ����ʂ̓��e��\��ʂɔ��f����
		WaitTimer(1000 / FPS); // ��莞�ԕۂ�
		if (ProcessMessage() == -1) break; // Windows ��������󂯎��G���[���N������I��
		if (CheckHitKey(KEY_INPUT_ESCAPE) == 1) break; // ESC�L�[�������ꂽ��I��
	}

	DxLib_End(); // DX���C�u�����g�p�̏I������
	return 0; // �\�t�g�̏I��
}

// �������牺�Ɏ��삵���֐����`����