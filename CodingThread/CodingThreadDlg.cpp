#include "CodingThreadDlg.h"

CodingThreadDlg* CodingThreadDlg::ptr = NULL;
HANDLE hMutex;
HWND hEdit1;

CodingThreadDlg::CodingThreadDlg(void)
{
	ptr = this;
}

CodingThreadDlg::~CodingThreadDlg(void)
{
	
}

void CodingThreadDlg::Cls_OnClose(HWND hwnd)
{
	ReleaseMutex(hMutex);
	EndDialog(hwnd, 0);

}

DWORD WINAPI Coding_Thread(LPVOID lp)
{
	CodingThreadDlg *ptr = (CodingThreadDlg *)lp;
	char buf[100];
	ifstream in(TEXT("music.txt"),ios::in);
	if(!in)
	{
		MessageBox(ptr->hDialog, TEXT("Ошибка открытия файла!"), TEXT("Мьютекс"), MB_OK | MB_ICONINFORMATION);
		return 1;
	}

	hMutex = OpenMutex(MUTEX_ALL_ACCESS, false, TEXT("{C189244F-52C7-423B-B578-F4361D0F84DE}"));

	DWORD dwAnswer = WaitForSingleObject(hMutex, INFINITE);

		ofstream out("coding.txt", ios::out | ios::trunc);
		const int KEY = 100;
		while(!in.eof())
		{
			in.getline(buf,100);
			out << buf << endl;
		}
		
		out.close();
		in.close();

		ReleaseMutex(hMutex);

		MessageBox(ptr->hDialog, TEXT("Запись данных в файл coding.txt завершена!"), TEXT("Мьютекс"), MB_OK | MB_ICONINFORMATION);

	return 0;
}


DWORD WINAPI Coding_Thread_List(LPVOID lp) {
	CodingThreadDlg* ptr = (CodingThreadDlg*)lp;

	char buf[255];
	std::ifstream in("coding.txt", ios::in);

	if (!in) {
		MessageBox(ptr->hDialog, TEXT("Ошибка открытия файла!"), TEXT("Мьютекс"), MB_OK | MB_ICONINFORMATION);
		return 1;
	}

	hMutex = OpenMutex(MUTEX_ALL_ACCESS, false, TEXT("{C189244F-52C7-423B-B578-F4361D0F84DE}"));

	DWORD dwAnswer = WaitForSingleObject(hMutex, INFINITE);

	do {
		in.getline(buf, 255);

		int len = MultiByteToWideChar(CP_UTF8, 0, buf, -1, NULL, 0);
		wchar_t* wbuf = new wchar_t[len];
		MultiByteToWideChar(CP_UTF8, 0, buf, -1, wbuf, len);

		SendMessageW(hEdit1, EM_REPLACESEL, TRUE, (LPARAM)wbuf);
		SendMessageW(hEdit1, EM_REPLACESEL, TRUE, (LPARAM)L"\r\n");
		delete[] wbuf;
	} while (in);

	in.close();

	ReleaseMutex(hMutex); 

	return 0;
}


void CodingThreadDlg::Cls_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify) {
	if (id == IDC_BUTTON1) {
		ReleaseMutex(hMutex);
		EndDialog(hwnd, 0);
	}
}

BOOL CodingThreadDlg::Cls_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)  {
	hDialog = hwnd;

	hEdit1 = GetDlgItem(hwnd, IDC_EDIT1);

	CreateThread(NULL, 0, Coding_Thread, this, 0, NULL);
	CreateThread(NULL, 0, Coding_Thread_List, this, 0, NULL);
	return TRUE;
}


BOOL CALLBACK CodingThreadDlg::DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		HANDLE_MSG(hwnd, WM_CLOSE, ptr->Cls_OnClose);
		HANDLE_MSG(hwnd, WM_INITDIALOG, ptr->Cls_OnInitDialog);
		HANDLE_MSG(hwnd, WM_COMMAND, ptr->Cls_OnCommand);
	}
	return FALSE;
}