/* a quick and very dirty AMS viewer */
#include "TSystem.h"
#include "TApplication.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TH1F.h"
#include "TStyle.h"
#include "TTree.h"
#include "TFile.h"
#include "TROOT.h"
/* from the Matteo 'Bozzochet' Duranti's API */
#include "Cluster.hh"
#include "Event.hh"
/* end */
#define PLOTS 8
#define PLOTS_COLS 3
#define PLOTS_ALL (PLOTS*PLOTS_COLS)
#define CANVAS_X 1024
#define CANVAS_Y 1280
#define LADDER_SIZE 8
#define LAST_EVENTS 15000
#define LOCK_FILE "/tmp/decoder.ams.lock"
#define DEFAULT_SLEEP 20000 /* useconds */
#define DEFAULT_REFRESH 30 /* seconds */
#define STRING_SIZE 512
using namespace std;
extern "C" {
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/file.h>
#include <limits.h>
typedef struct s_file_info {
	char file_name[PATH_MAX];
	int last_change, assigned, refresh;
} s_file_info;
typedef struct s_plot {
	TH1F *plot;
	unsigned short int ladder, pad, values, cumulative, logarithmic;
} s_plot;
struct s_file_info last_file, bigger_file, current_file;
pthread_mutex_t v_mutex = PTHREAD_MUTEX_INITIALIZER;
int p_lock_file(const char *name) {
	int result = -1, descriptor;
	if ((descriptor = open(name, O_CREAT|O_RDWR, 0666)) >= 0) {
		if (flock(descriptor, LOCK_EX|LOCK_NB) >= 0)
			result = descriptor;
		else
			close(descriptor);
	}
	return result;
}

void p_unlock_file(int *descriptor) {
	flock(*descriptor, LOCK_UN);
	close(*descriptor);
	*descriptor = -1;
}

void f_analyze_file(char *file) {
	char *extension;
	struct stat file_attribute;
	if ((extension = strrchr(file, '.')) && (strcmp(extension, ".root") == 0)) {
		stat(file, &file_attribute);
		if ((!current_file.assigned) || (current_file.last_change < file_attribute.st_mtime)) {
			strcpy(current_file.file_name, file);
			current_file.last_change = file_attribute.st_mtime;
			current_file.assigned = 1;
		}
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
Long64_t v_counting_from = 0;
int f_load_ttree(char *file, struct s_plot plots[PLOTS_ALL]) {
	TTree *branch;
	TFile *stream = new TFile(file);
	Event *current_event;
	Cluster *current_cluster;
	RHClass *current_infos;
	Long64_t entries, index_event, index_cluster, first_event = 0;
	int result = -1, index_ladder, last_strip, current_address;
	if (stream->IsOpen())
		if ((branch = (TTree *)(stream->Get("t4"))))
			if ((entries = branch->GetEntries()) > 0) {
				current_event = new Event();
				branch->SetBranchAddress("cluster_branch", &current_event);
				branch->GetEntry(0);
				if ((current_infos = (RHClass *)branch->GetUserInfo()->At(0))) {
					if ((first_event = v_counting_from) < entries)
						for (index_ladder = 0; index_ladder < PLOTS_ALL; ++index_ladder)
							if ((!plots[index_ladder].cumulative) || (v_counting_from == 0)) {
								plots[index_ladder].plot->Reset();
								plots[index_ladder].values++;
							}
					for (index_event = first_event; index_event < entries; ++index_event) {
						branch->GetEntry(index_event);
						for (index_cluster = 0; index_cluster < current_event->NClusTot; ++index_cluster) {
							current_cluster = current_event->GetCluster(index_cluster);
							for (index_ladder = 0; index_ladder < LADDER_SIZE; ++index_ladder)
								if (plots[index_ladder].ladder == current_cluster->ladder) {
									for (current_address = current_cluster->address,
											last_strip = (current_cluster->address+current_cluster->GetLength());
											current_address < last_strip; current_address++) {
										plots[index_ladder].plot->Fill(current_address);
										plots[index_ladder].values++;
									}
									if (current_cluster->address > 640) {
										plots[PLOTS+index_ladder].plot->Fill(current_cluster->GetTotSig());
										plots[PLOTS+index_ladder].values++;
										plots[(PLOTS*2)+index_ladder].plot->Fill(current_cluster->GetTotSig());
										plots[(PLOTS*2)+index_ladder].values++;
									}
									break;
								}
						}
					}
					v_counting_from = entries;
				}
				result = 0;
			}
	delete stream;
	return result;
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

TH1F *f_generate_plot(const char *name, const char *title, float low_x, float high_x, float bin_k, Color_t color) {
	TH1F *result = NULL;
	if ((result = new TH1F(name, title, (high_x-low_x)*bin_k, low_x, high_x))) {
		result->SetStats(kFALSE);
		gStyle->SetTitleFontSize(0.1);
		gStyle->SetOptStat("emro");
		result->SetLineColor(color);
		result->SetLineWidth(2.0f);
		result->SetFillColor(color);
		result->SetFillStyle(3005);
	}
	return result; 
}

void f_refresh_plots(TCanvas *canvas, struct s_plot plots[PLOTS_ALL]) {
	int index;
	for (index = 0; index < PLOTS_ALL; ++index) {
		if (plots[index].values > 0) {
			canvas->cd(plots[index].pad);
			if (plots[index].logarithmic)
				gPad->SetLogy();
			gPad->SetBottomMargin(0.05);
			gPad->SetLeftMargin(0.05);
			gPad->SetRightMargin(0.05);
			gPad->SetTopMargin(0.05);
			if (plots[index].values > 0)
				plots[index].plot->Draw("hist");
			canvas->Modified();
		}
	}
}

void f_check_file(void) {
	if (current_file.assigned)
		if ((!last_file.assigned) || (strcmp(last_file.file_name, current_file.file_name) != 0) || 
				(last_file.last_change < current_file.last_change))  {
			current_file.refresh = 1;
			if (strcmp(last_file.file_name, current_file.file_name) != 0)
				v_counting_from = 0;
		}
}

void *f_thread_exec(void *arg) {
	pthread_mutex_lock(&v_mutex);
	printf("-[thread]: hello, running: '%s'\n", (const char *)arg);
	system((const char *)arg);
	printf("-[thread]: done\n");
	pthread_mutex_unlock(&v_mutex);
	return NULL;
}

int main (int argc, char *argv[]) {
	TApplication viewer("viewer", NULL, NULL);
	TCanvas *canvas = f_generate_canvas("real_time_viewer", "viewer", PLOTS_COLS, PLOTS);
	struct s_plot plots[PLOTS_ALL] = {
		{f_generate_plot("ladder_0_occ", "ladder 0 occupancy", 0, 1024, 1, kRed), 0, 7, false, false},
		{f_generate_plot("ladder_1_occ", "ladder 1 occupancy", 0, 1024, 1, kRed), 1, 4, false, false},
		{f_generate_plot("ladder_2_occ", "ladder 2 occupancy", 0, 1024, 1, kRed), 2, 1, false, false},
		{f_generate_plot("ladder_3_occ", "ladder 3 occupancy", 0, 1024, 1, kRed), 3, 22, false, false},
		{f_generate_plot("ladder_4_occ", "ladder 4 occupancy", 0, 1024, 1, kRed), 4, 13, false, false},
		{f_generate_plot("ladder_5_occ", "ladder 5 occupancy", 0, 1024, 1, kRed), 5, 16, false, false},
		{f_generate_plot("ladder_6_occ", "ladder 6 occupancy", 0, 1024, 1, kRed), 6, 19, false, false},
		{f_generate_plot("ladder_7_occ", "ladder 7 occupancy", 0, 1024, 1, kRed), 7, 10, false, false},
		{f_generate_plot("ladder_0_sig", "ladder 0 signal distribution", 50, 4096, 0.2, kBlue), 0, 8, false, true},
		{f_generate_plot("ladder_1_sig", "ladder 1 signal distribution", 50, 4096, 0.2, kBlue), 1, 5, false, true},
		{f_generate_plot("ladder_2_sig", "ladder 2 signal distribution", 50, 4096, 0.2, kBlue), 2, 2, false, true},
		{f_generate_plot("ladder_3_sig", "ladder 3 signal distribution", 50, 4096, 0.2, kBlue), 3, 23, false, true},
		{f_generate_plot("ladder_4_sig", "ladder 4 signal distribution", 50, 4096, 0.2, kBlue), 4, 14, false, true},
		{f_generate_plot("ladder_5_sig", "ladder 5 signal distribution", 50, 4096, 0.2, kBlue), 5, 17, false, true},
		{f_generate_plot("ladder_6_sig", "ladder 6 signal distribution", 50, 4096, 0.2, kBlue), 6, 20, false, true},
		{f_generate_plot("ladder_7_sig", "ladder 7 signal distribution", 50, 4096, 0.2, kBlue), 7, 11, false, true},
		{f_generate_plot("ladder_0_sigtot", "ladder 0 signal TOTAL", 50, 4096, 0.2, kGreen), 0, 9, true, true},
		{f_generate_plot("ladder_1_sigtot", "ladder 1 signal TOTAL", 50, 4096, 0.2, kGreen), 1, 6, true, true},
		{f_generate_plot("ladder_2_sigtot", "ladder 2 signal TOTAL", 50, 4096, 0.2, kGreen), 2, 3, true, true},
		{f_generate_plot("ladder_3_sigtot", "ladder 3 signal TOTAL", 50, 4096, 0.2, kGreen), 3, 24, true, true},
		{f_generate_plot("ladder_4_sigtot", "ladder 4 signal TOTAL", 50, 4096, 0.2, kGreen), 4, 15, true, true},
		{f_generate_plot("ladder_5_sigtot", "ladder 5 signal TOTAL", 50, 4096, 0.2, kGreen), 5, 18, true, true},
		{f_generate_plot("ladder_6_sigtot", "ladder 6 signal TOTAL", 50, 4096, 0.2, kGreen), 6, 21, true, true},
		{f_generate_plot("ladder_7_sigtot", "ladder 7 signal TOTAL", 50, 4096, 0.2, kGreen), 7, 12, true, true}
	};
	time_t last_run = time(NULL);
	bool is_alive = false, refresh_me = false;
	char buffer[STRING_SIZE];
	pthread_t thread;
	if (argc >= 3) {
		snprintf(buffer, STRING_SIZE, "%s &>/dev/null", argv[2]);
		while (true) {
			if ((!is_alive) && ((time(NULL)-last_run) > DEFAULT_REFRESH))
				if ((pthread_mutex_trylock(&v_mutex)) == 0) {
					pthread_create(&thread, NULL, f_thread_exec, (void *)buffer);
					pthread_mutex_unlock(&v_mutex);
				}
			if (current_file.refresh) {
				current_file.refresh = 0;
				f_load_ttree(current_file.file_name, plots);
				refresh_me = true;
			} else  if ((pthread_mutex_trylock(&v_mutex)) == 0) {
				if (is_alive) {
					printf("-[main]: thread has done\n");
					last_run = time(NULL);
					is_alive = false;
				}
				if (current_file.assigned) {
					memcpy(&(last_file), &(current_file), sizeof(s_file_info));
					last_file.assigned = 1;
					current_file.assigned = 0;
				}
				f_search_file(argv[1]);
				f_check_file();
				pthread_mutex_unlock(&v_mutex);
			} else
				is_alive = true;
			if ((refresh_me) && (v_counting_from > 0))
				f_refresh_plots(canvas, plots);
			canvas->Update();
			gSystem->ProcessEvents();
			usleep(DEFAULT_SLEEP);
		}
	} else
		fprintf(stderr, "usage:\n%s <directory with .root> <application to execute every %d seconds>\n", argv[0], DEFAULT_REFRESH);
	return 0;
}
