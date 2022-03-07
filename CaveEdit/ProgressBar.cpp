#include "stdafx.h"
#include "ProgressMessageBox.h"
#include "ProgressBar.h"

using namespace ProgressWindow;

namespace ProgressWindow {
	ProgressMessageBox* cPMB = NULL;
};

void ProgressWindow::StartTask(const char* task_name, const char* window_name, void** user_data, ProgressFunc run_func) {
	int user_data_len = 0;

	// Count user_data
	if (user_data) {
		for (int i = 0;; i++) {
			if (user_data[i] == NULL)
				break;

			user_data_len++;
		}
	}

	cPMB = new ProgressMessageBox();

	// Show the window
	cPMB->Create(ProgressMessageBox::IDD, NULL);
	cPMB->EnableWindow(TRUE);
	cPMB->ShowWindow(TRUE);
	cPMB->SetWindowTextA(window_name);
	cPMB->CenterWindow();
	cPMB->BeginModalState();
	cPMB->SetTaskName(task_name);

	// Init
	cPMB->SetCurrentProgress(0);
	cPMB->SetOverallProgress(0);

	// Configure the window
	cPMB->mFunc     = run_func;
	cPMB->mUserArgs = user_data_len ? new void*[user_data_len] : NULL;
	cPMB->mRunning  = true;

	if (user_data_len)
		memcpy(cPMB->mUserArgs, user_data, user_data_len * sizeof(void*));
	
	// Start the timer
	cPMB->StartMainTimer();
}

void ProgressWindow::EndTask() {
	if (!cPMB)
		return;

	cPMB->EndModalState();
	cPMB->EnableWindow(FALSE);
	cPMB->DestroyWindow();

	cPMB->mRunning = false;
	cPMB           = NULL;
}

void ProgressWindow::UpdateProgressCurrent(int percent, const char* text) {
	if (!cPMB)
		return;

	cPMB->SetCurrentProgress(percent, text);
}

void ProgressWindow::UpdateProgressOverall(int percent, const char* text) {
	if (!cPMB)
		return;

	cPMB->SetOverallProgress(percent, text);
}

void ProgressWindow::SetCurrentProgressMax(int max) {
	if (!cPMB)
		return;

	cPMB->SetMaxCurrentProgress(max);
}

void ProgressWindow::SetOverallProgressMax(int max) {
	if (!cPMB)
		return;

	cPMB->SetMaxOverallProgress(max);
}