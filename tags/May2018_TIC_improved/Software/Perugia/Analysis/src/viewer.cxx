/* a quick and very dirty viewer */
#include "TApplication.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TH1F.h"
#include "TStyle.h"
#include "TROOT.h"
#define PLOTS 5
#define CANVAS_X 300
#define CANVAS_Y 800
extern "C" {
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <linux/limits.h>
using namespace std;
typedef struct s_file_info {
	char file_name[PATH_MAX];
	int last_change, assigned:1, refresh:1;
} s_file_info;
struct s_file_info last_file, current_file;
void f_analyze_file(char *file) {
	struct stat file_attribute;
	stat(file, &file_attribute);
	if ((last_file.last_change < file_attribute.st_ctime) || (!last_file.assigned)) {
		strcpy(last_file.file_name, file);
		last_file.last_change = file_attribute.st_mtime;
		last_file.assigned = 1;
	}
}

void f_search_file(char *directory) {
	DIR *stream;
	struct dirent *descriptor;
	char next_directory[PATH_MAX];
	size_t length;
	if ((stream = opendir(directory))) {
		while ((descriptor = readdir(stream)))
			if ((descriptor->d_name[0] != '.')) {
				length = strlen(directory);
				if (directory[length-1] == '/')
					directory[length-1] = '\0';
				snprintf(next_directory, PATH_MAX, "%s/%s", directory, descriptor->d_name);
				f_search_file(next_directory);
			}
		closedir(stream);
	} else
		f_analyze_file(directory);
}
}
TCanvas *f_generate_canvas(const char *name, const char *title, int divisions_x=1, int divisions_y=1) {
	TCanvas *result = NULL;
	if ((result = new TCanvas(name, title, CANVAS_X, CANVAS_Y))) {
		result->SetFillColor(0);
		result->SetBorderMode(0);
		result->SetFrameBorderMode(0);
		if ((divisions_x != 1) || (divisions_y != 1))
			result->Divide(divisions_x, divisions_y, 0, 0);
	}
	return result;
}

TH1F *f_generate_plot(const char *name, const char *title, float low_x, float high_x, float bin_k) {
	TH1F *result = NULL;
	if ((result = new TH1F(name, title, (high_x-low_x)*bin_k, low_x, high_x))) {
		result->SetStats(kTRUE);
		gStyle->SetOptStat("emro");
		result->SetLineColor(kRed);
		result->SetLineWidth(2.0f);
		result->SetFillColor(kRed);
		result->SetFillStyle(3005);
	}
	return result; 
}

void f_refresh_plots(TCanvas *canvas, TH1F *plots[PLOTS]) {
	int index;
	for (index = 0; index < PLOTS; ++index) {
		plots[index]->FillRandom("gaus");
		canvas->cd(index);
		plots[index]->Draw();
	}
}

bool p_check_file(struct s_file_info left, struct s_file_info right) {
	bool result = true;
	if (left.assigned)
		if ((!right.assigned) || (strcmp(left.file_name, right.file_name) != 0) || (left.last_change > right.last_change))
			result = false;
	return result;
}

void f_check_file(void) {
	if (last_file.assigned)
		if (!p_check_file(last_file, current_file)) {
			memcpy(&current_file, &last_file, sizeof(struct s_file_info));
			last_file.assigned = 0;
			current_file.refresh = 1;
		}
}

int run_me (int argc, char *argv[]) {
	TApplication viewer("viewer", NULL, NULL);
	TCanvas *canvas = f_generate_canvas("real_time_viewer", "real time viewer", 1, PLOTS);
	TH1F *plots[PLOTS] = {
		f_generate_plot("ladder_0", "ladder 0", 0, 1024, 2),
		f_generate_plot("ladder_1", "ladder 1", 0, 1024, 2),
		f_generate_plot("ladder_2", "ladder 2", 0, 1024, 2),
		f_generate_plot("ladder_3", "ladder 3", 0, 1024, 2),
		f_generate_plot("ladder_4", "ladder 4", 0, 1024, 2)
	};
	if (argc >= 2) {
		f_search_file(argv[1]);
		f_check_file();
		f_refresh_plots(canvas, plots);
		canvas->Update();
		viewer.Run();

	}
	return 0;
}
