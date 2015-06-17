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
#define PLOTS 5
#define PLOTS_COLS 3
#define PLOTS_ALL (PLOTS*PLOTS_COLS)
#define CANVAS_X 1024
#define CANVAS_Y 1280
#define LADDER_SIZE 5
#define LAST_EVENTS 1500
using namespace std;
extern "C" {
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>
typedef struct s_file_info {
	char file_name[PATH_MAX];
	int last_change, assigned:1, refresh:1;
} s_file_info;
typedef struct s_plot {
	TH1F *plot;
	unsigned short int ladder, pad, cumulative:1, logy:1;
} s_plot;
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
Long64_t v_counting_from = 0;
int f_load_ttree(char *file, struct s_plot plots[PLOTS_ALL]) {
	TTree *branch;
	TFile *stream = new TFile(file);
	Event *current_event;
	Cluster *current_cluster;
	RHClass *current_infos;
	Long64_t entries, index_event, index_cluster, first_event = 0;
	int result = -1, index_ladder, index_stip, current_address;
	bool good_one;
	if (stream->IsOpen()) {
		for (index_ladder = 0; index_ladder < PLOTS_ALL; ++index_ladder)
			if ((!plots[index_ladder].cumulative) || (v_counting_from == 0))
				plots[index_ladder].plot->Reset("MICE");
		if ((branch = (TTree *)(stream->Get("t4"))))
			if ((entries = branch->GetEntries()) > 0) {
				current_event = new Event();
				branch->SetBranchAddress("cluster_branch", &current_event);
				branch->GetEntry(0);
				if ((current_infos = (RHClass *)branch->GetUserInfo()->At(0))) {
					first_event = v_counting_from;
					for (index_event = first_event; index_event < entries; ++index_event) {
						branch->GetEntry(index_event);
						for (index_cluster = 0; index_cluster < current_event->NClusTot; ++index_cluster) {
							current_cluster = current_event->GetCluster(index_cluster);
							for (index_ladder = 0; index_ladder < LADDER_SIZE; ++index_ladder)
								if (plots[index_ladder].ladder == current_cluster->ladder) {
									good_one = true;
									switch (plots[index_ladder].ladder) {
										case 0:
											if (((current_cluster->address <= 192) && (current_cluster->address+current_cluster->length >= 192)) ||
													((current_cluster->address <= 256) && (current_cluster->address+current_cluster->length >= 256)))
												good_one = false;
											break;
										case 1:

											if (((current_cluster->address <= 63) && (current_cluster->address+current_cluster->length >= 63)) ||
													((current_cluster->address <= 125) && (current_cluster->address+current_cluster->length >= 125)) ||
													((current_cluster->address <= 127) && (current_cluster->address+current_cluster->length >= 127)) ||
													((current_cluster->address <= 191) && (current_cluster->address+current_cluster->length >= 191)) ||
													((current_cluster->address <= 256) && (current_cluster->address+current_cluster->length >= 256)) ||
													((current_cluster->address <= 255) && (current_cluster->address+current_cluster->length >= 255)) ||
													((current_cluster->address == 0)))
												good_one = false;
											break;
										case 2:
											if (((current_cluster->address <= 192) && (current_cluster->address+current_cluster->length >= 192)) ||
													((current_cluster->address <= 193) && (current_cluster->address+current_cluster->length >= 193)) ||
													((current_cluster->address <= 255) && (current_cluster->address+current_cluster->length >= 255)) ||
													((current_cluster->address <= 256) && (current_cluster->address+current_cluster->length >= 256)))
												good_one = false;
											break;
										case 6:
											if (((current_cluster->address <= 192) && (current_cluster->address+current_cluster->length >= 192)) ||
													((current_cluster->address <= 256) && (current_cluster->address+current_cluster->length >= 256)) ||
													((current_cluster->address <= 496) && (current_cluster->address+current_cluster->length >= 496)) ||
													((current_cluster->address <= 499) && (current_cluster->address+current_cluster->length >= 499)) ||
													((current_cluster->address <= 501) && (current_cluster->address+current_cluster->length >= 501)) ||
													((current_cluster->address <= 503) && (current_cluster->address+current_cluster->length >= 503)) ||
													((current_cluster->address <= 505) && (current_cluster->address+current_cluster->length >= 505)) ||
													((current_cluster->address <= 63) && (current_cluster->address+current_cluster->length >= 63))   ||
													((current_cluster->address <= 126) && (current_cluster->address+current_cluster->length >= 126)) ||
													((current_cluster->address <= 403) && (current_cluster->address+current_cluster->length >= 403)))
												good_one = false;
											break;
										default:
											break;
									}
									if (current_cluster->bad)
										good_one = false;
									if (good_one) {
										plots[index_ladder].plot->Fill(current_cluster->address, current_cluster->length);
										plots[PLOTS+index_ladder].plot->Fill(current_cluster->GetTotSig());
										plots[(PLOTS*2)+index_ladder].plot->Fill(current_cluster->length);
									}
									break;
								}
						}
					}
					v_counting_from = entries;
				}
				result = 0;
			}
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

TH1F *f_generate_plot(const char *name, const char *title, float low_x, float high_x, float bin_k) {
	TH1F *result = NULL;
	if ((result = new TH1F(name, title, (high_x-low_x)*bin_k, low_x, high_x))) {
		result->SetTitleSize(1.5);
		result->SetStats(kTRUE);
		gStyle->SetOptStat("emro");
		result->SetLineColor(kRed);
		result->SetLineWidth(2.0f);
		result->SetFillColor(kRed);
		result->SetFillStyle(3005);
	}
	return result; 
}

void f_refresh_plots(TCanvas *canvas, struct s_plot plots[PLOTS_ALL]) {
	int index;
	for (index = 0; index < PLOTS_ALL; ++index) {
		canvas->cd(plots[index].pad);
		plots[index].plot->Draw("hist");
		canvas->Modified();
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

int main (int argc, char *argv[]) {
	TApplication viewer("viewer", NULL, NULL);
	TCanvas *canvas[PLOTS_ALL];
	struct s_plot plots[PLOTS_ALL] = {
		{f_generate_plot("ladder_0_occ", "ladder GSI03 gravity", 0, 1024, 2), 0, 1, false, true},
		{f_generate_plot("ladder_1_occ", "ladder GSI02 gravity", 0, 1024, 2), 1, 4, false, true},
		{f_generate_plot("ladder_2_occ", "ladder GSI05 gravity", 0, 1024, 2), 2, 7, false, true},
		{f_generate_plot("ladder_3_occ", "ladder GSI06 gravity", 0, 1024, 2), 6, 10, false, true},
		{f_generate_plot("ladder_4_occ", "ladder GSI04 gravity", 0, 1024, 2), 5, 13, false, true},
		{f_generate_plot("ladder_0_sigtot", "ladder GSI03 signal", 0, 400, 2), 0, 3, true, true},
		{f_generate_plot("ladder_1_sigtot", "ladder GSI02 signal", 0, 400, 2), 1, 6, true, true},
		{f_generate_plot("ladder_2_sigtot", "ladder GSI05 signal", 0, 400, 2), 2, 9, true, true},
		{f_generate_plot("ladder_3_sigtot", "ladder GSI06 signal", 0, 400, 2), 6, 12, true, true},
		{f_generate_plot("ladder_4_sigtot", "ladder GSI04 signal", 0, 400, 2), 5, 15, true, true},
		{f_generate_plot("ladder_0_nchannels", "ladder GSI03 number of channels", 0, 40, 1), 0, 3, true, true},
		{f_generate_plot("ladder_1_nchannels", "ladder GSI02 number of channels", 0, 40, 1), 1, 6, true, true},
		{f_generate_plot("ladder_2_nchannels", "ladder GSI05 number of channels", 0, 40, 1), 2, 9, true, true},
		{f_generate_plot("ladder_3_nchannels", "ladder GSI06 number of channels", 0, 40, 1), 6, 12, true, true},
		{f_generate_plot("ladder_4_nchannels", "ladder GSI04 number of channels", 0, 40, 1), 5, 15, true, true},
	};
	char name[25], filename[256];
	printf("lading the ttre ...\n");
	f_load_ttree(argv[1], plots);
	printf("end!\n");
	for (int index = 0; index < PLOTS_ALL; ++index) {
		snprintf(name, 25, "canvas_%d", index);
		canvas[index] = f_generate_canvas(name, name, 1, 1);
		if (plots[index].logy)
			canvas[index]->SetLogy();
		plots[index].plot->Draw("hist");
		canvas[index]->Modified();
		canvas[index]->Update();
		if (index == 0)
			snprintf(filename, 256, "%s_pdf.pdf(", argv[1]);
		else if (index == (PLOTS_ALL-1))
			snprintf(filename, 256, "%s_pdf.pdf)", argv[1]);
		else
			snprintf(filename, 256, "%s_pdf.pdf", argv[1]);
		canvas[index]->Print(filename);
	}
	while (1) {
		for (int index = 0; index < PLOTS_ALL; ++index)
			canvas[index]->WaitPrimitive();
		gSystem->ProcessEvents();
	}

	return 0;
}
