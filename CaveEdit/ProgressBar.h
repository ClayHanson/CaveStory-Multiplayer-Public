#pragma once

namespace ProgressWindow {
	typedef void(*ProgressFunc)(void**);

	void StartTask(const char* task_name, const char* window_name, void** user_data, ProgressFunc run_func);
	void EndTask();

	void UpdateProgressCurrent(int percent, const char* text = NULL);
	void UpdateProgressOverall(int percent, const char* text = NULL);

	void SetCurrentProgressMax(int max);
	void SetOverallProgressMax(int max);
};

#define DECLARE_PROGRESS_TASK(task_name, display_text, window_name) \
	namespace ProgressWindow {\
		static const char* __pt_disptext_##task_name  = display_text;\
		static const char* __pt_wind_name_##task_name = window_name;\
		static void task_##task_name(void**);\
	}\
	static void ProgressWindow::task_##task_name(void** user_data)

#define START_PROGRESS_TASK(task_name, ...) \
	{\
		void* __arg_list_##task_name##[] = { __VA_ARGS__, NULL }; \
		ProgressWindow::StartTask(ProgressWindow::__pt_disptext_##task_name, ProgressWindow::__pt_wind_name_##task_name, __arg_list_##task_name, ProgressWindow::task_##task_name);\
	}